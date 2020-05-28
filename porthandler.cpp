#include "porthandler.h"
#include <stdlib.h>
#include <QDebug>

Porthandler::Porthandler(QVector<int>* values_ptr)
{
    values = values_ptr;
}

void Porthandler::process(){
    int maxParams=1;
    bool maxParamsFound = false;
    bool firstLineSkipped = false;
    char buf[1024];
    while(doRead){
        if(port.bytesAvailable()>0){
            if(port.canReadLine()){
                int lineLength = port.readLine(buf, 1024);
                //qDebug() << "Read Line <" << buf << "> with size"<<lineLength;
                QByteArray input(buf);
                QList<QByteArray> sep = input.split(' ');
                if(firstLineSkipped){
                if(!maxParamsFound){
                    qDebug() << "Looking for maxsize";
                    if(sep.size()>0){
                        qDebug() << "Sep.size is " << sep.size();
                        maxParamsFound = true;
                        maxParams = sep.size();
                        values->resize(maxParams);
                        qDebug()<< "Maximum values:" << maxParams;
                        emit plotDataGet(maxParams);
                        qDebug()<< "emit plotDataGet(maxParams);" << maxParams;
                    }
                }else{
                    for(int i=0; i<maxParams; i++){
                        values->replace(i, sep.value(i).toInt());
                    }
                }
                }else{
                    firstLineSkipped=true;
                }
            }
        }
    }
}

void Porthandler::setPort(QString name){
    portName = name;
}

void Porthandler::setSpeed(int speed){
    portSpeed = speed;
}

bool Porthandler::open(){
    port.setBaudRate(portSpeed, QSerialPort::AllDirections);
    port.setDataBits(QSerialPort::Data8);
    port.setFlowControl(QSerialPort::NoFlowControl);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    port.setPortName(portName);
    qDebug() << "Port name:" << portName << "speed:" << portSpeed;
    bool portState = port.open(QIODevice::ReadWrite);
    doRead = portState;
    return portState;
}

bool Porthandler::close(){
    port.close();
    doRead = false;
    qDebug() << "Port closed";
    return true;
}

int Porthandler::write(QByteArray data){

}
