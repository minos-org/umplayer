#ifndef FILEASSOCQUESTION_H
#define FILEASSOCQUESTION_H

#include <QDialog>

namespace Ui {
    class FileAssocQuestion;
}

class FileAssocQuestion : public QDialog
{
    Q_OBJECT

public:
    static FileAssocQuestion* mInstance;
    static FileAssocQuestion* instance();
    void showQuestion(QString ext);



private:
    Ui::FileAssocQuestion *ui;
    explicit FileAssocQuestion(QWidget *parent = 0);
    ~FileAssocQuestion();
    QString mExtension;
    QString messageText;


private slots:
    void defaultCheck(int result);
};

#endif // FILEASSOCQUESTION_H
