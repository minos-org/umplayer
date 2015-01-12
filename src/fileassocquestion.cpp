#include "fileassocquestion.h"
#include "ui_fileassocquestion.h"
#include "winfileassoc.h"
#include "preferences.h"
#include "global.h"

using namespace Global;

FileAssocQuestion* FileAssocQuestion::mInstance = 0;

FileAssocQuestion::FileAssocQuestion(QWidget *parent):
    QDialog(parent),
    ui(new Ui::FileAssocQuestion)
{
    ui->setupUi(this);    
    connect(this, SIGNAL(finished(int)), this, SLOT(defaultCheck(int)));
    messageText = ui->messageLabel->text();
}

FileAssocQuestion::~FileAssocQuestion()
{
    delete ui;
}

void FileAssocQuestion::defaultCheck(int result)
{
    pref->checkDefault = ui->performCheck->isChecked();
    if(result == QDialog::Accepted)
    {
        int count = WinFileAssoc().CreateFileAssociations(QStringList() << mExtension);
        if(count != 1)
        {
            pref->exceptExtensions.append(mExtension);
        }
    }
}

FileAssocQuestion* FileAssocQuestion::instance()
{
    if(mInstance == 0)
    {
        mInstance = new FileAssocQuestion(0);
    }
    return mInstance;
}

void FileAssocQuestion::showQuestion(QString ext)
{
    if(!pref->checkDefault || pref->exceptExtensions.contains(ext))
    {
        return;
    }
    mExtension = ext;
    QStringList regExt;
    WinFileAssoc().GetRegisteredExtensions(QStringList() << mExtension , regExt );
    if(regExt.contains(ext))
        return;
    else
    {
        ui->messageLabel->setText(messageText.arg(ext.toUpper()));
        exec();
    }
}
