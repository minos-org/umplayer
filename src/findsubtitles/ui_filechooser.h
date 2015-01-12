/********************************************************************************
** Form generated from reading UI file 'filechooser.ui'
**
** Created: Mon 8. Feb 19:27:40 2010
**      by: Qt User Interface Compiler version 4.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILECHOOSER_H
#define UI_FILECHOOSER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileChooser
{
public:
    QHBoxLayout *hboxLayout;
    QLineEdit *line_edit;
    QToolButton *button;

    void setupUi(QWidget *FileChooser)
    {
        if (FileChooser->objectName().isEmpty())
            FileChooser->setObjectName(QString::fromUtf8("FileChooser"));
        FileChooser->resize(407, 27);
        hboxLayout = new QHBoxLayout(FileChooser);
        hboxLayout->setSpacing(2);
        hboxLayout->setContentsMargins(2, 2, 2, 2);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        line_edit = new QLineEdit(FileChooser);
        line_edit->setObjectName(QString::fromUtf8("line_edit"));

        hboxLayout->addWidget(line_edit);

        button = new QToolButton(FileChooser);
        button->setObjectName(QString::fromUtf8("button"));

        hboxLayout->addWidget(button);


        retranslateUi(FileChooser);

        QMetaObject::connectSlotsByName(FileChooser);
    } // setupUi

    void retranslateUi(QWidget *FileChooser)
    {
        Q_UNUSED(FileChooser);
    } // retranslateUi

};

namespace Ui {
    class FileChooser: public Ui_FileChooser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILECHOOSER_H
