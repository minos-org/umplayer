#ifndef LANGUAGESELECTION_H
#define LANGUAGESELECTION_H

#include <QDialog>

namespace Ui {
    class LanguageSelection;
}

class LanguageSelection : public QDialog
{
    Q_OBJECT

public:
    explicit LanguageSelection(QWidget *parent = 0);
    ~LanguageSelection();

private:
    Ui::LanguageSelection *ui;
    void fillCombos();
private slots:
    void languageAccepted();
};

#endif // LANGUAGESELECTION_H
