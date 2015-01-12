#include <QLocale>
#include <QDebug>
#include "languageselection.h"
#include "ui_languageselection.h"
#include "helper.h"
#include "languages.h"
#include "preferences.h"
#include "global.h"


using namespace Global;

LanguageSelection::LanguageSelection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LanguageSelection)
{
    ui->setupUi(this);
    fillCombos();
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(languageAccepted()));
    setWindowTitle("Language Preference");
}

LanguageSelection::~LanguageSelection()
{
    delete ui;
}


void LanguageSelection::fillCombos()
{
    QMap<QString, QString> l = Helper::interfaceLanguages();
    QLocale locale = QLocale::system();
    foreach(QString key, l.keys())
    {
        ui->interfaceCombo->addItem(key, l.value(key));        
        qDebug() <<  l.value(key) << locale.name();
    }

    ui->interfaceCombo->setCurrentIndex(ui->interfaceCombo->findData(locale.name().left(2), Qt::UserRole, Qt::MatchStartsWith));
    l = Languages::encodings();
    QMapIterator<QString, QString> i(l);
    while (i.hasNext()) {
            i.next();
            ui->subtitleCombo->addItem( i.value() + " (" + i.key() + ")", i.key() );
    }
    ui->subtitleCombo->setCurrentIndex(ui->subtitleCombo->findData("UTF-8"));
}

void LanguageSelection::languageAccepted()
{
    pref->language_is_set = true;
    pref->subcp = ui->subtitleCombo->itemData(ui->subtitleCombo->currentIndex()).toString();
    pref->language =  ui->interfaceCombo->itemData(ui->interfaceCombo->currentIndex() ).toString();
    pref->save();
    accept();
    deleteLater();
}
