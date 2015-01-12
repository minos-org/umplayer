/********************************************************************************
** Form generated from reading UI file 'findsubtitleswindow.ui'
**
** Created: Mon 8. Feb 19:27:40 2010
**      by: Qt User Interface Compiler version 4.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDSUBTITLESWINDOW_H
#define UI_FINDSUBTITLESWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "filechooser.h"

QT_BEGIN_NAMESPACE

class Ui_FindSubtitlesWindow
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *subtitles_for_label;
    FileChooser *file_chooser;
    QTreeView *view;
    QHBoxLayout *hboxLayout1;
    QLabel *filter_language_label;
    QComboBox *language_filter;
    QSpacerItem *spacerItem;
    QPushButton *configure_button;
    QPushButton *refresh_button;
    QPushButton *download_button;
    QHBoxLayout *hboxLayout2;
    QLabel *status;
    QProgressBar *progress;
    QDialogButtonBox *buttonBox;

    void setupUi(QWidget *FindSubtitlesWindow)
    {
        if (FindSubtitlesWindow->objectName().isEmpty())
            FindSubtitlesWindow->setObjectName(QString::fromUtf8("FindSubtitlesWindow"));
        FindSubtitlesWindow->resize(649, 385);
        vboxLayout = new QVBoxLayout(FindSubtitlesWindow);
        vboxLayout->setSpacing(6);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        vboxLayout->setContentsMargins(9, 9, 9, 9);
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        subtitles_for_label = new QLabel(FindSubtitlesWindow);
        subtitles_for_label->setObjectName(QString::fromUtf8("subtitles_for_label"));

        hboxLayout->addWidget(subtitles_for_label);

        file_chooser = new FileChooser(FindSubtitlesWindow);
        file_chooser->setObjectName(QString::fromUtf8("file_chooser"));

        hboxLayout->addWidget(file_chooser);


        vboxLayout->addLayout(hboxLayout);

        view = new QTreeView(FindSubtitlesWindow);
        view->setObjectName(QString::fromUtf8("view"));

        vboxLayout->addWidget(view);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        filter_language_label = new QLabel(FindSubtitlesWindow);
        filter_language_label->setObjectName(QString::fromUtf8("filter_language_label"));

        hboxLayout1->addWidget(filter_language_label);

        language_filter = new QComboBox(FindSubtitlesWindow);
        language_filter->setObjectName(QString::fromUtf8("language_filter"));
        language_filter->setEditable(false);

        hboxLayout1->addWidget(language_filter);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem);

        configure_button = new QPushButton(FindSubtitlesWindow);
        configure_button->setObjectName(QString::fromUtf8("configure_button"));

        hboxLayout1->addWidget(configure_button);

        refresh_button = new QPushButton(FindSubtitlesWindow);
        refresh_button->setObjectName(QString::fromUtf8("refresh_button"));
        refresh_button->setEnabled(false);

        hboxLayout1->addWidget(refresh_button);

        download_button = new QPushButton(FindSubtitlesWindow);
        download_button->setObjectName(QString::fromUtf8("download_button"));
        download_button->setEnabled(false);

        hboxLayout1->addWidget(download_button);


        vboxLayout->addLayout(hboxLayout1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        hboxLayout2->setContentsMargins(0, 0, 0, 0);
        status = new QLabel(FindSubtitlesWindow);
        status->setObjectName(QString::fromUtf8("status"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(status->sizePolicy().hasHeightForWidth());
        status->setSizePolicy(sizePolicy);
        status->setFrameShape(QFrame::Panel);
        status->setFrameShadow(QFrame::Sunken);

        hboxLayout2->addWidget(status);

        progress = new QProgressBar(FindSubtitlesWindow);
        progress->setObjectName(QString::fromUtf8("progress"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(progress->sizePolicy().hasHeightForWidth());
        progress->setSizePolicy(sizePolicy1);
        progress->setValue(0);
        progress->setTextVisible(false);
        progress->setOrientation(Qt::Horizontal);

        hboxLayout2->addWidget(progress);


        vboxLayout->addLayout(hboxLayout2);

        buttonBox = new QDialogButtonBox(FindSubtitlesWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);

        vboxLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        subtitles_for_label->setBuddy(file_chooser);
        filter_language_label->setBuddy(language_filter);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(file_chooser, view);
        QWidget::setTabOrder(view, language_filter);
        QWidget::setTabOrder(language_filter, refresh_button);
        QWidget::setTabOrder(refresh_button, download_button);
        QWidget::setTabOrder(download_button, buttonBox);

        retranslateUi(FindSubtitlesWindow);
        QObject::connect(buttonBox, SIGNAL(rejected()), FindSubtitlesWindow, SLOT(close()));

        QMetaObject::connectSlotsByName(FindSubtitlesWindow);
    } // setupUi

    void retranslateUi(QWidget *FindSubtitlesWindow)
    {
        FindSubtitlesWindow->setWindowTitle(QApplication::translate("FindSubtitlesWindow", "Find Subtitles", 0, QApplication::UnicodeUTF8));
        subtitles_for_label->setText(QApplication::translate("FindSubtitlesWindow", "&Subtitles for", 0, QApplication::UnicodeUTF8));
        filter_language_label->setText(QApplication::translate("FindSubtitlesWindow", "&Language:", 0, QApplication::UnicodeUTF8));
        configure_button->setText(QApplication::translate("FindSubtitlesWindow", "&Options", 0, QApplication::UnicodeUTF8));
        refresh_button->setText(QApplication::translate("FindSubtitlesWindow", "&Refresh", 0, QApplication::UnicodeUTF8));
        download_button->setText(QApplication::translate("FindSubtitlesWindow", "&Download", 0, QApplication::UnicodeUTF8));
        status->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class FindSubtitlesWindow: public Ui_FindSubtitlesWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDSUBTITLESWINDOW_H
