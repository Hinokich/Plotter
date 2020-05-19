#include "datawriter.h"
#include <QDateTime>
#include <QDebug>

Datawriter::Datawriter()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QDate date = dateTime.date();
    header = "Simple Serial Plotter v0.1\n";
    header = header + date.toString("dddd, d MMMM yyyy") + ", " + dateTime.toUTC().toString("hh:mm:ss") +"\n\n";
}

void Datawriter::openFile(QString filename){
    name = filename;
    file.setFileName(name);
    qDebug() << "attempt to open:" << name;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "!!!A BIG SHIT HAPENT!!!";
        qDebug() << file.errorString();
        return;
    }
    QTextStream out(&file);
    if(printHeader){
        out << header;
    }
    isWriting = true;
    qDebug() << "isWriting = true, filename = " << filename;
    qDebug() << "header=" << printHeader << header;
}

void Datawriter::writeDataToFile(QVector<int>* values_ptr){
    if(isWriting){
        QTextStream out(&file);
        for(int i=0; i<values_ptr->size(); i++){
            out << values_ptr->value(i) << " ";
        }
        out << endl;
    }
}

void Datawriter::stopWriting(){
    file.close();
    isWriting = false;
}

void Datawriter::enableHeader(bool state){
    printHeader = state;
    qDebug() << "HEADER IS NOW " << printHeader;
}
