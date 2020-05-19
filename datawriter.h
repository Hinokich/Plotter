#ifndef DATAWRITER_H
#define DATAWRITER_H

#include <QObject>
#include <QFile>

class Datawriter : public QObject
{
    Q_OBJECT
public:
    explicit Datawriter();
    void openFile(QString filename);
    void writeDataToFile(QVector<int>* values_ptr);
    void stopWriting();
    void enableHeader(bool state);

signals:

public slots:

private:
    QString header;
    QString name;
    QFile file;
    bool printHeader = false;
    bool isWriting = false;
    bool isStopWriting = false;
};

#endif // DATAWRITER_H
