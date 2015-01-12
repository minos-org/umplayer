/********************************************************************************
** Form generated from reading UI file 'subchooserdialog.ui'
**
** Created: Mon 8. Feb 19:27:40 2010
**      by: Qt User Interface Compiler version 4.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SUBCHOOSERDIALOG_H
#define UI_SUBCHOOSERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SubChooserDialog
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *textLabel;
    QListWidget *listWidget;
    QHBoxLayout *hboxLayout;
    QPushButton *selectAll;
    QPushButton *selectNone;
    QSpacerItem *spacerItem;
    QFrame *line;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SubChooserDialog)
    {
        if (SubChooserDialog->objectName().isEmpty())
            SubChooserDialog->setObjectName(QString::fromUtf8("SubChooserDialog"));
        SubChooserDialog->resize(370, 329);
        vboxLayout = new QVBoxLayout(SubChooserDialog);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        textLabel = new QLabel(SubChooserDialog);
        textLabel->setObjectName(QString::fromUtf8("textLabel"));
        textLabel->setAlignment(Qt::AlignVCenter);
        textLabel->setWordWrap(true);

        vboxLayout->addWidget(textLabel);

        listWidget = new QListWidget(SubChooserDialog);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::NoEditTriggers);
        listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
        listWidget->setViewMode(QListView::ListMode);
        listWidget->setUniformItemSizes(true);
        listWidget->setSortingEnabled(true);

        vboxLayout->addWidget(listWidget);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        selectAll = new QPushButton(SubChooserDialog);
        selectAll->setObjectName(QString::fromUtf8("selectAll"));

        hboxLayout->addWidget(selectAll);

        selectNone = new QPushButton(SubChooserDialog);
        selectNone->setObjectName(QString::fromUtf8("selectNone"));

        hboxLayout->addWidget(selectNone);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);


        vboxLayout->addLayout(hboxLayout);

        line = new QFrame(SubChooserDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        vboxLayout->addWidget(line);

        buttonBox = new QDialogButtonBox(SubChooserDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(SubChooserDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SubChooserDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SubChooserDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SubChooserDialog);
    } // setupUi

    void retranslateUi(QDialog *SubChooserDialog)
    {
        SubChooserDialog->setWindowTitle(QApplication::translate("SubChooserDialog", "Subtitle selection", 0, QApplication::UnicodeUTF8));
        textLabel->setText(QApplication::translate("SubChooserDialog", "This archive contains more than one subtitle file. Please choose the ones you want to extract.", 0, QApplication::UnicodeUTF8));
        selectAll->setText(QApplication::translate("SubChooserDialog", "Select All", 0, QApplication::UnicodeUTF8));
        selectNone->setText(QApplication::translate("SubChooserDialog", "Select None", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SubChooserDialog: public Ui_SubChooserDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUBCHOOSERDIALOG_H
