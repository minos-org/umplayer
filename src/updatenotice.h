#ifndef UPDATENOTICE_H
#define UPDATENOTICE_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QBoxLayout>

class QPushButton;

class UpdateNotice : public QDialog
{
Q_OBJECT    
public:
    enum UpdateType
    {
        Optional = 1,
        OptionalCritical = 2,
        Critical = 3,
        OptionalBinary = 11,
        OptionalCriticalBinary = 12,
        CriticalBinary = 13
    };

    explicit UpdateNotice(QWidget *parent = 0);
    void setUpdateType(int type);

signals:

public slots:

private:
    QLabel* headerLabel;
    QLabel* detailsLabel;
    QLabel* exclamationLabel;
    QLabel* borderLabel;
    QDialogButtonBox* buttonBox;
    QPushButton* laterButton;

};

#endif // UPDATENOTICE_H
