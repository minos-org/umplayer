/*  umplayer, GUI front-end for mplayer.
    Copyright (C) 2010 Ori Rejwan

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <QVBoxLayout>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QTimeLine>
#include "ytdialog.h"
#include "yttabbar.h"
#include "ytdelegate.h"
#include "ytdataapi.h"
#include "retrievevideourl.h"
#include "images.h"
#include "myborder.h"
#include "searchbox.h"
#include "recordingdialog.h"
#include "preferences.h"
#include "global.h"


using namespace Global;

#define PAGE_RESULT_COUNT 25

PixmapLoader* PixmapLoader::m_instance = 0;
YTDialog* YTDialog::m_instance = 0;

OverlayWidget::OverlayWidget(QWidget* parent) : QWidget(parent)
{
    loadingOverlay = QPixmap(":/Control/bg-youtube-loading-overlay.png");
    exclaimPix = QPixmap(":/Control/bg-alert.png");    
    timeLine = new QTimeLine(1000, this);
    timeLine->setLoopCount(0);
    timeLine->setFrameRange(1, 13);
    for(int i=0; i < 12; ++i)
    {
        stages[i] = Images::icon(QString("stage%1").arg(i+1, 2, 10, QChar('0')));
    }
    timeLine->setCurveShape(QTimeLine::LinearCurve);
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(frameHasChanged(int)));
}

void OverlayWidget::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    p.drawPixmap(rect(), loadingOverlay  );
    if(m_text.isEmpty())
    {        
        const QPixmap pix = stages[qMax(1,qMin(12,timeLine->currentFrame()))-1];
        QPoint trans = rect().center() - pix.rect().center();
        p.drawPixmap(trans, pix);
        return;
    }
    p.setPen(Qt::white);
    QFont font = p.font();
    font.setPointSize(12);
    p.setFont(font);
    QRect textRect(0, 0, 280, 40);
    textRect.translate(rect().center() - textRect.center());
    QRect alertRect = exclaimPix.rect();
    alertRect.setHeight(textRect.height());
    alertRect.moveTopLeft(textRect.topLeft());
    p.drawPixmap( alertRect, exclaimPix);
    textRect.setX(alertRect.right() + 10);
    p.drawText(textRect, Qt::AlignTop|Qt::AlignLeft|Qt::TextWordWrap| Qt::TextDontClip ,m_text);
}

void OverlayWidget::showEvent(QShowEvent* s)
{
    if(!s->spontaneous() && m_text.isEmpty())
    {
        timeLine->start();
    }
}

void OverlayWidget::hideEvent(QHideEvent *h)
{
    if(!h->spontaneous() && m_text.isEmpty())
    {
        timeLine->stop();
    }
}

void OverlayWidget::setText(QString text)
{
    m_text = text;
    update();
}

void OverlayWidget::frameHasChanged(int)
{
    update();
}


/******************************************************************************/


YTButton::YTButton(QWidget *parent)
    : QAbstractButton(parent), hovered(false)
{

}

void YTButton::setPix(QPixmap pix)
{
   normalPix = pix.copy(0, 0, pix.width(), pix.height()/2);
   hoverPix = pix.copy(0, pix.height()/2, pix.width(), pix.height()/2);
   setFixedSize(normalPix.size());
}

void YTButton::enterEvent(QEvent *e)
{
    hovered = true;
    update();
}

void YTButton::leaveEvent(QEvent *e)
{
    hovered = false;
    update();
}

void YTButton::paintEvent(QPaintEvent *e)
{    
    QPainter p(this);
    if(hovered)
    {
        p.drawPixmap(0, 0, hoverPix);
    }
    else
    {
        p.drawPixmap(0, 0, normalPix);
    }
}
/*******************************************************************************************************
***********************************************************************************************************/
PixmapLoader* PixmapLoader::instance()
{
    if(!m_instance)
    {
        m_instance = new PixmapLoader;
    }
    return m_instance;
}

PixmapLoader::~PixmapLoader()
{
    manager->deleteLater();
}

int PixmapLoader::getPixmap(QString url)
{
    QNetworkRequest req(url);
    QNetworkReply* rep = manager->get(req);
    rep->setObjectName(url);
    return (qint64)rep;

}

PixmapLoader::PixmapLoader(QObject *parent)
    :QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotPixmap(QNetworkReply*)));
}

void PixmapLoader::gotPixmap(QNetworkReply *reply)
{
    QByteArray arr = reply->readAll();
    QPixmap pix;
    pix.loadFromData(arr);
    reply->deleteLater();
    emit pixmapResult(pix, (qint64)reply);
}

void PixmapLoader::reset()
{
    manager->deleteLater();
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotPixmap(QNetworkReply*)));
}

/*******************************************************************************************************
***********************************************************************************************************/

YTDialog::YTDialog(QWidget *parent) :
    QWidget(parent), overlayVisible(false)
{
    QStringList logos;
    logos << "Umplayer-16" << "Umplayer-24" <<"Umplayer-32" << "Umplayer-48" << "Umplayer-256" << "Umplayer-512" ;
    setWindowIcon( Images::icon(logos) );
    setAutoFillBackground(true);
    setWindowTitle("YouTube Browser - UMPlayer");    
    tabBar = new YTTabBar(this);
    connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(gotCurrentTab(int)));
    overlay = new OverlayWidget(this);
    videoList = new QListWidget(this);

    MyBorder* border = new MyBorder(this);
    border ->setBGColor(palette().color(backgroundRole()));

    videoList->setFrameShape(QFrame::NoFrame);
    delegate = new YTDelegate(videoList);
    videoList->setItemDelegate(delegate);    
    //videoList->setMouseTracking(true);
    //videoList->viewport()->installEventFilter(this);
    videoList->setContextMenuPolicy(Qt::CustomContextMenu);
    videoList->setAutoScroll(false);
    nextButton = new QPushButton(this);
    nextButton->setText("Next");
    nextButton->adjustSize();
    nextButton->setFixedWidth(nextButton->width());
    prevButton = new QPushButton(this);
    prevButton->setText("Previous");
    prevButton->adjustSize();
    prevButton->setFixedWidth(prevButton->width());
    searchBox = new SearchBox(this);
    connect(searchBox, SIGNAL(search(QString)), this, SLOT(setSearchTerm(QString)));    
    connect(nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(prevClicked()));
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(searchBox);
    hbox->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Preferred));
    hbox->addWidget(prevButton);        
    hbox->addWidget(nextButton);    
    hbox->setContentsMargins(6, 5, 8, 3);
    hbox->setSpacing(9);
    QVBoxLayout* vbox = new QVBoxLayout;    
    vbox->addWidget(tabBar);
    vbox->addWidget(videoList);
    vbox->addWidget(border );
    vbox->addLayout(hbox);
    setLayout(vbox);
    vbox->setSpacing(0);
    vbox->setContentsMargins( 0, 0, 0, 0);
    overlay->raise();
    setLoadingOverlay(false);    
    api = new YTDataAPI(this);
    connect(api, SIGNAL(finalResults(YTReply)), this, SLOT(gotAPIReply(YTReply)));
    connect(PixmapLoader::instance(), SIGNAL(pixmapResult(QPixmap,int)), this, SLOT(gotPixmap(QPixmap,int)));
    connect(videoList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(videoClicked(QListWidgetItem*)));
    connect(videoList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(videoDblClicked(QListWidgetItem*)));
    connect(videoList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

}

void YTDialog::setLoadingOverlay(bool enable)
{
    overlayVisible = enable;
    if(enable)
    {
        overlay->resize(size());
        overlay->setText(QString());
        overlay->show();
    }
    else
    {
        overlay->hide();
    }
}

void YTDialog::resizeEvent(QResizeEvent* r)
{
    if(overlayVisible)
    {
        overlay->resize(r->size());
    }
}

bool YTDialog::eventFilter(QObject* w, QEvent* e)
{
    if(w == videoList->viewport())
    {        
        if(e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* m = static_cast<QMouseEvent*>(e);
            QListWidgetItem* hoveredItem =  videoList->itemAt(videoList->viewport()->mapFromGlobal(m->globalPos()));
            for(int i=0; i < videoList->count(); ++i)
            {
                QListWidgetItem* wi = videoList->item(i);                
                if( wi == hoveredItem )
                {                    
                    wi->setData(Hovered, true);
                }
                else
                {                    
                    wi->setData(Hovered, false);
                }
            }
            if(hoveredItem)
                setCursor(QCursor(Qt::PointingHandCursor));
            else
                setCursor(QCursor(Qt::ArrowCursor));
        }
        if(e->type() == QEvent::Leave)
        {
            for(int i=0; i < videoList->count(); ++i)
            {
                QListWidgetItem* wi = videoList->item(i) ;                
                wi->setData(Hovered, false);
            }
            setCursor(QCursor(Qt::ArrowCursor));
        }
    }
    return false;
}

void YTDialog::setMode(Mode mode)
{
    disconnect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(gotCurrentTab(int)));
    reset();    
    if(mode == Button)
    {
        searchTerm = QString();
        searchBox->updateText(searchTerm);
        addTab(Popular);
        addTab(ViewedButton);
        addTab(Rated);        
    }
    else if(mode == Search)
    {
        searchBox->updateText(searchTerm);
        addTab(Relevant);
        addTab(Recent);
        addTab(Viewed);        
    }
    connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(gotCurrentTab(int)));
    tabBar->setCurrentIndex(0);
    gotCurrentTab(0);
}

void YTDialog::addTab(Tabs tab)
{
    int index = -1;    
    if(tab == Relevant)
    {
        index = tabBar->addTab("Most Relevant");
    }
    else if(tab == Recent)
    {
        index = tabBar->addTab("Most Recent");
    }
    else if(tab == Viewed)
    {
        index = tabBar->addTab("Most Viewed");
    }
    else if(tab == Popular)
    {
        index = tabBar->addTab("Most Popular");
    }
    else if(tab == ViewedButton)
    {
        index = tabBar->addTab("Most Viewed");
    }
    else if(tab == Rated)
    {
        index = tabBar->addTab("Most Rated");
    }    
    tabBar->setTabData(index, tab);
}

void YTDialog::gotCurrentTab(int index)
{
    Tabs tab = static_cast<Tabs>(tabBar->tabData(index).toInt());
    //Title text
    switch((int)tab)
    {
    case Popular: setWindowTitle(QString("Most Popular Videos on YouTube%1").arg(QChar(0x2122)));break;
    case Rated: setWindowTitle(QString("Most Rated Videos on YouTube%1").arg(QChar(0x2122)));break;
    case ViewedButton: setWindowTitle(QString("Most Viewed Videos on YouTube%1").arg(QChar(0x2122)));break;
    case Relevant :
    case Recent :
    case Viewed : setWindowTitle(QString("YouTube%2 results for \"%1\"").arg(searchTerm).arg(QChar(0x2122)));break;
    }

    if(entries.value(tab).populated)
    {
        updateNextPrevWidget();
    }
    else
    {
        setLoadingOverlay(true);
        switch((int)tab)
        {
        case Popular: resultForTab[api->getMost(Popular)] = Popular; break;
        case Rated: resultForTab[api->getMost(Rated)]= Rated; break;
        case ViewedButton: resultForTab[api->getMost(ViewedButton)]= ViewedButton; break;
        case Relevant : resultForTab[api->getMost(Relevant, searchTerm)]= Relevant; break;
        case Recent : resultForTab[api->getMost(Recent, searchTerm)]= Recent; break;
        case Viewed : resultForTab[api->getMost(Viewed, searchTerm)]= Viewed; break;
        }
    }
}

void YTDialog::nextClicked()
{    
    Tabs tab = static_cast<Tabs>(tabBar->tabData(tabBar->currentIndex()).toInt());
    TabData td = entries.value(tab);
    int lastPageNumber = lastPageNo(tab);
    if( lastPageNumber == td.pageNo)
    {
        if( !entries[tab].nextUrl.isEmpty() )
        {
            entries[tab].pageNo += 1;
            setLoadingOverlay(true);
            resultForTab[api->getNextResults(entries[tab].nextUrl)] = tab;
        }
    }
    else
    {
        entries[tab].pageNo += 1;
        updateNextPrevWidget();
    }

}

void YTDialog::prevClicked()
{
    Tabs tab = static_cast<Tabs>(tabBar->tabData(tabBar->currentIndex()).toInt());    
    entries[tab].pageNo = qMax(entries[tab].pageNo - 1, 1);
    updateNextPrevWidget();
}

void YTDialog::updateNextPrevWidget()
{
    videoList->clear();
    Tabs tab = static_cast<Tabs>(tabBar->tabData(tabBar->currentIndex()).toInt());
    QList<SingleVideoItem*> vList = entries.value(tab).data;
    //if(vList.empty())
        //return;
    QList<SingleVideoItem*>::iterator it = vList.begin();
    int thisPageNo = entries.value(tab).pageNo;
    it += (thisPageNo -1 ) * PAGE_RESULT_COUNT;
    for(int i = 0; i< PAGE_RESULT_COUNT; ++i)
    {
        if(it >= vList.end() )
            break;
        QListWidgetItem* wItem = new QListWidgetItem(videoList, QListWidgetItem::UserType + 1);
        SingleVideoItem* svi = *it;
        if(!svi->pixUrl.isEmpty())
        {
            int id = PixmapLoader::instance()->getPixmap(svi->pixUrl);
            pendingPixmapQueue[id] = svi;
        }
        wItem->setData(0, qVariantFromValue(*it));
        ++it;
    }        
    if(thisPageNo < lastPageNo(tab))
        nextButton->show();
    else if(!entries.value(tab).nextUrl.isEmpty())
        nextButton->show();
    else
        nextButton->hide();
    if(thisPageNo == 1 )
        prevButton->hide();
    else
        prevButton->show();
}

void YTDialog::gotAPIReply(const YTReply& formattedReply)
{    
    if(resultForTab.contains(formattedReply.replyPointer))
    {        
        Tabs tab = resultForTab.value(formattedReply.replyPointer);            
        for(int i=0; i < formattedReply.results.count(); ++i)
        {
            SingleVideoItem* svi = new SingleVideoItem;
            SingleResult sing = formattedReply.results.value(i);
            svi->date = sing.publishedDate;
            svi->desc = sing.desc.simplified();
            svi->header = sing.title;
            svi->totalTime = sing.duration;
            svi->views = sing.viewCount;
            svi->pixUrl = sing.thumbUrl;
            svi->videoid = sing.videoId;
            entries[tab].data.append(svi);
        }
        entries[tab].populated = true;
        entries[tab].nextUrl = formattedReply.nextUrl;
        resultForTab.remove(formattedReply.replyPointer);
        updateNextPrevWidget();
        if(!formattedReply.errorString.isEmpty())
        {
            overlay->setText("Error: Could not connect to Youtube Server.");
            return;
        }
        if(formattedReply.results.count() == 0)
        {
            if(searchTerm.isEmpty())
                overlay->setText("No videos found");
            else
                overlay->setText("No videos found for \"" + searchTerm + "\"");
            return;
        }

    }
    setLoadingOverlay(false);
}

void YTDialog::gotPixmap(QPixmap pix, int id)
{
    SingleVideoItem* item = pendingPixmapQueue.value(id);
    if(item)
    {
        item->pix = pix;
        item->pixUrl = QString();
        videoList->viewport()->update();
        pendingPixmapQueue.remove(id);
    }
}

void YTDialog::setSearchTerm(QString term)
{
    searchTerm = term;    
    setMode(Search);
}

void YTDialog::reset()
{
    PixmapLoader::instance()->reset();
    api->reset();
    pendingPixmapQueue.clear();
    resultForTab.clear();
    foreach(TabData d, entries.values())
    {
        qDeleteAll(d.data);
    }
    entries.clear();
    videoList->clear();
    while(tabBar->count() > 0)
    {
        tabBar->removeTab(0);
    }
}

int YTDialog::lastPageNo(Tabs tab)
{
    TabData td = entries[tab];
    return td.data.count()/PAGE_RESULT_COUNT + ((td.data.count() % PAGE_RESULT_COUNT)>0?1:0);
}

void YTDialog::videoClicked(QListWidgetItem *item)
{
    for(int i=0; i < videoList->count(); ++i)
    {
        QListWidgetItem* wi = videoList->item(i) ;
        wi->setData(Clicked, false);
    }
    item->setData(Clicked, true);
}

void YTDialog::videoDblClicked(QListWidgetItem *item)
{
    SingleVideoItem* svi = item->data(0).value<SingleVideoItem*>();
    RetrieveVideoUrl* rvu = new RetrieveVideoUrl(this);
    connect(rvu, SIGNAL(gotUrls(QMap<int,QString>, QString, QString)), this, SIGNAL(gotUrls(QMap<int,QString>, QString, QString)) );
    connect(rvu, SIGNAL(gotUrls(QMap<int,QString>, QString, QString)), rvu, SLOT(deleteLater()));
    rvu->fetchYTVideoPage(svi->videoid, svi->header );
}

void YTDialog::showContextMenu(QPoint point)
{ 
    QMenu menu;
    menu.addAction("Play video");
    menu.addAction("Record video");
    menu.addAction("Watch on YouTube");
    QAction* action = menu.exec(videoList->viewport()->mapToGlobal(point));
    if(!action) return;
    QListWidgetItem* item = videoList->itemAt(point);
    if(action->text().startsWith('P'))
    {
       videoDblClicked(item);
    }
    else if(action->text().startsWith('R'))
    {
       recordItem(item);
    }
    else if(action->text().startsWith('W'))
    {
        SingleVideoItem* svi = item->data(0).value<SingleVideoItem*>();
        QDesktopServices::openUrl(QString("http://www.youtube.com/watch?v=%1").arg(svi->videoid));
    }
}

void YTDialog::recordItem(QListWidgetItem *item)
{
    SingleVideoItem* svi = item->data(0).value<SingleVideoItem*>();
    RecordingDialog::downloadVideoId(svi->videoid, svi->header, 0);
}


YTDialog* YTDialog::instance()
{
    if(m_instance == 0)
    {
        m_instance = new YTDialog;
    }
    return m_instance;
}
