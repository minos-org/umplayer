#include "updatenotice.h"
#include <QGridLayout>
#include <QPushButton>
#include <QPixmap>

UpdateNotice::UpdateNotice(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint |
                   Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(470, 110);
    setWindowTitle("Update Available");
    headerLabel = new QLabel(this);
    detailsLabel = new QLabel(this);
    exclamationLabel = new QLabel(this);
    borderLabel = new QLabel(this);    
    buttonBox = new QDialogButtonBox(this);
    QWidget* upperBox = new QWidget(this);
    buttonBox->addButton(QDialogButtonBox::Yes);
    buttonBox->addButton(QDialogButtonBox::Ok);
    laterButton = buttonBox->addButton("Later", QDialogButtonBox::RejectRole);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QPalette pal = upperBox->palette();
    pal.setColor(upperBox->backgroundRole(), Qt::white);
    upperBox->setPalette(pal);
    upperBox->setAutoFillBackground(true);

    QFont font = headerLabel->font();
    font.setPixelSize(12);
    detailsLabel->setFont(font);
    detailsLabel->setWordWrap(true);
    detailsLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    detailsLabel->setIndent(0);
    detailsLabel->setSizePolicy(QSizePolicy::Expanding, detailsLabel->sizePolicy().verticalPolicy());


    font.setBold(true);
    headerLabel->setFont(font);
    headerLabel->setAlignment(Qt::AlignLeft);    
    headerLabel->setIndent(0);

    exclamationLabel->setPixmap(QPixmap(":/Control/alert.png"));
    exclamationLabel->setFixedSize(exclamationLabel->pixmap()->size());


    borderLabel->setPixmap(QPixmap(":/Control/alert-border.png"));
    borderLabel->setScaledContents(true);
    borderLabel->setFixedHeight(borderLabel->pixmap()->height());

    QGridLayout* grid = new QGridLayout;
    grid->setHorizontalSpacing(0);
    grid->setVerticalSpacing(0);
    grid->addItem(new QSpacerItem(17, 10, QSizePolicy::Fixed, QSizePolicy::Preferred), 0, 0, 2, 1);
    grid->addWidget(headerLabel, 0, 1);
    grid->addWidget(detailsLabel, 1, 1);
    grid->addWidget(exclamationLabel, 0, 2, 2, 1, Qt::AlignCenter);
    grid->addWidget(borderLabel, 3, 0, 1, 4);
    grid->addItem(new QSpacerItem(5, 10, QSizePolicy::Preferred, QSizePolicy::Expanding), 2, 0, 1, 3);
    grid->addItem(new QSpacerItem(5, 10, QSizePolicy::Fixed, QSizePolicy::Preferred), 0, 3, 3, 1);

    grid->setContentsMargins(0,10,0,0);    
    upperBox->setLayout(grid);    
    QGridLayout* outerGrid = new QGridLayout;
    outerGrid->addWidget(upperBox, 0, 0, 1, 2);
    outerGrid->addWidget(buttonBox, 1, 0);
    outerGrid->addItem(new QSpacerItem(5, 10, QSizePolicy::Fixed, QSizePolicy::Preferred), 1, 1, 1, 1);
    outerGrid->setContentsMargins(0,0,0,7);
    outerGrid->setVerticalSpacing(10);
    setLayout(outerGrid);
}

void UpdateNotice::setUpdateType(int type)
{
    switch(type)
    {
    case Optional:
    case OptionalBinary:
        headerLabel->setText("Application Update");
        detailsLabel->setText("A new version of UMPlayer is available. Do you want to download it now?");
        buttonBox->button(QDialogButtonBox::Yes)->show();
        buttonBox->button(QDialogButtonBox::Ok)->hide();
        laterButton->show();
        break;
    case OptionalCritical:
    case OptionalCriticalBinary:
        headerLabel->setText("Critical Application Update");
        detailsLabel->setText("A new critical update of UMPlayer is available. We strongly \
recommend that you install this update now. Do you want to download it now?");
        buttonBox->button(QDialogButtonBox::Yes)->show();
        buttonBox->button(QDialogButtonBox::Ok)->hide();
        laterButton->show();
        break;
    case Critical:
    case CriticalBinary:
        headerLabel->setText("Critical Application Update");
        detailsLabel->setText("A new critical update of UMPlayer is available. \
You must install this update now in order for the application to continue to work");
        buttonBox->button(QDialogButtonBox::Yes)->hide();
        buttonBox->button(QDialogButtonBox::Ok)->show();
        laterButton->hide();
        break;
    default:
        break;
    }
}
