#ifndef SCDATAAPI_H
#define SCDATAAPI_H

#include <QObject>
#include <QVariant>

class QNetworkAccessManager;
class QNetworkReply;

class SingleSCResult
{
public:
    SingleSCResult() {}
    ~SingleSCResult() {}
    QString name;
    QString format;
    QString id;
    QString bitrate;
    QString genre;
    QString song;
    QString listeners;
};

Q_DECLARE_METATYPE(SingleSCResult*);

class SCReply
{
public:
    int totalResultCount;
    QString queryString;
    QList<SingleSCResult> results;
    QString errorString;
};


class SCDataAPI : public QObject
{
Q_OBJECT
public:
    explicit SCDataAPI(QObject *parent = 0);
    void getTop500();
    void getSearchResult(QString searchTerm);
    void reset();


private:
    QNetworkAccessManager* manager;
    void parseXMLReply(QString xml, SCReply& formattedReply);
    QString unescape(QString html);


signals:    
    void finalResult(SCReply& formattedReply);

public slots:
    void gotReply(QNetworkReply* reply);

};

#endif // SCDATAAPI_H
