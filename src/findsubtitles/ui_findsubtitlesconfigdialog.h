/********************************************************************************
** Form generated from reading UI file 'findsubtitlesconfigdialog.ui'
**
** Created: Mon 8. Feb 19:27:40 2010
**      by: Qt User Interface Compiler version 4.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDSUBTITLESCONFIGDIALOG_H
#define UI_FINDSUBTITLESCONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FindSubtitlesConfigDialog
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox;
    QVBoxLayout *vboxLayout1;
    QCheckBox *use_proxy_check;
    QWidget *proxy_container;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLineEdit *proxy_hostname_edit;
    QLabel *label_3;
    QSpinBox *proxy_port_spin;
    QLabel *label_4;
    QLineEdit *proxy_username_edit;
    QLabel *label_5;
    QLineEdit *proxy_password_edit;
    QLabel *label_6;
    QComboBox *proxy_type_combo;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *FindSubtitlesConfigDialog)
    {
        if (FindSubtitlesConfigDialog->objectName().isEmpty())
            FindSubtitlesConfigDialog->setObjectName(QString::fromUtf8("FindSubtitlesConfigDialog"));
        FindSubtitlesConfigDialog->resize(489, 271);
        vboxLayout = new QVBoxLayout(FindSubtitlesConfigDialog);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        groupBox = new QGroupBox(FindSubtitlesConfigDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        vboxLayout1 = new QVBoxLayout(groupBox);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        use_proxy_check = new QCheckBox(groupBox);
        use_proxy_check->setObjectName(QString::fromUtf8("use_proxy_check"));

        vboxLayout1->addWidget(use_proxy_check);

        proxy_container = new QWidget(groupBox);
        proxy_container->setObjectName(QString::fromUtf8("proxy_container"));
        proxy_container->setEnabled(false);
        gridLayout = new QGridLayout(proxy_container);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(proxy_container);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        proxy_hostname_edit = new QLineEdit(proxy_container);
        proxy_hostname_edit->setObjectName(QString::fromUtf8("proxy_hostname_edit"));

        gridLayout->addWidget(proxy_hostname_edit, 0, 1, 1, 2);

        label_3 = new QLabel(proxy_container);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 0, 3, 1, 1);

        proxy_port_spin = new QSpinBox(proxy_container);
        proxy_port_spin->setObjectName(QString::fromUtf8("proxy_port_spin"));
        proxy_port_spin->setMaximum(65535);

        gridLayout->addWidget(proxy_port_spin, 0, 4, 1, 1);

        label_4 = new QLabel(proxy_container);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        proxy_username_edit = new QLineEdit(proxy_container);
        proxy_username_edit->setObjectName(QString::fromUtf8("proxy_username_edit"));

        gridLayout->addWidget(proxy_username_edit, 1, 1, 1, 4);

        label_5 = new QLabel(proxy_container);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        proxy_password_edit = new QLineEdit(proxy_container);
        proxy_password_edit->setObjectName(QString::fromUtf8("proxy_password_edit"));
        proxy_password_edit->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(proxy_password_edit, 2, 1, 1, 4);

        label_6 = new QLabel(proxy_container);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 3, 0, 1, 1);

        proxy_type_combo = new QComboBox(proxy_container);
        proxy_type_combo->setObjectName(QString::fromUtf8("proxy_type_combo"));

        gridLayout->addWidget(proxy_type_combo, 3, 1, 1, 1);

        spacerItem = new QSpacerItem(261, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 3, 2, 1, 3);


        vboxLayout1->addWidget(proxy_container);


        vboxLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(FindSubtitlesConfigDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(proxy_hostname_edit);
        label_3->setBuddy(proxy_port_spin);
        label_4->setBuddy(proxy_username_edit);
        label_5->setBuddy(proxy_password_edit);
        label_6->setBuddy(proxy_type_combo);
#endif // QT_NO_SHORTCUT

        retranslateUi(FindSubtitlesConfigDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), FindSubtitlesConfigDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FindSubtitlesConfigDialog, SLOT(reject()));
        QObject::connect(use_proxy_check, SIGNAL(toggled(bool)), proxy_container, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(FindSubtitlesConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *FindSubtitlesConfigDialog)
    {
        FindSubtitlesConfigDialog->setWindowTitle(QApplication::translate("FindSubtitlesConfigDialog", "Advanced options", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("FindSubtitlesConfigDialog", "Proxy", 0, QApplication::UnicodeUTF8));
        use_proxy_check->setText(QApplication::translate("FindSubtitlesConfigDialog", "&Enable proxy", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("FindSubtitlesConfigDialog", "&Host:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("FindSubtitlesConfigDialog", "&Port:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("FindSubtitlesConfigDialog", "&Username:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("FindSubtitlesConfigDialog", "Pa&ssword:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("FindSubtitlesConfigDialog", "&Type:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FindSubtitlesConfigDialog: public Ui_FindSubtitlesConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDSUBTITLESCONFIGDIALOG_H
