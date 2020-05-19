#ifndef PORTHANDLER_H
#define PORTHANDLER_H

#include <QObject>
#include <QThread>
#include <QSerialPort>

class Porthandler : public QObject
{
    Q_OBJECT
public:
    Porthandler(QVector<int>* values_ptr);
    void setPort(QString name);
    void setSpeed(int speed);
    bool open();
    bool close();
    int write(QByteArray data);

public slots:
    void process();

signals:
    void dataArrived(QByteArray* data);
    void plotDataGet(int count);

private:
    QString portName;
    int portSpeed;
    bool doRead = false;
    QSerialPort port;
    QByteArray bufferInternal;
    QVector<int>* values;
    int* maxGraphs;
};

#endif // PORTHANDLER_H
