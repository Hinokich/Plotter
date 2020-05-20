#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>
#include "porthandler.h"
#include "qcustomplot.h"
#include "datawriter.h"
#include <QDebug>
#include <QThread>
#include <QTimer>

QVector<int> values;
QVector<QString> colors = {"#0000ff","#ff0000","#6be33b","#fcba03","#3ec9c5","#9242b8","#eb57e3","#ffae00","#688a3e ","#6b6b6b"};

QThread* handlerThread = new QThread();
QThread* writerThread = new QThread();
Porthandler* handler = new Porthandler(&values);
Datawriter* dataWriter = new Datawriter();



QSerialPortInfo portInfo;
QTimer axisTimer;
QTime globalTime(QTime::currentTime());

QVector<int> portSpeeds = {115200, 38400, 9600};
QString portName;

int portSpeed;
bool connected = false;
int replotTime = 0;
int maxGraphs = 10;
int graphsCount = 1;
int yLower = 0;
int yUpper = 5000;
int xScale = 30;
bool logsEnabled = false;
bool headerEnabled = false;
bool isWriting = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for(int i=0; i<portInfo.availablePorts().size(); i++){
        ui->comboBoxPort->addItem(portInfo.availablePorts().value(i).portName());
    }
    for(int i=0; i<portSpeeds.size(); i++){
        ui->comboBoxSpeed->addItem(QString::number(portSpeeds.value(i)));
    }
    handler->moveToThread(handlerThread);
    dataWriter->moveToThread(writerThread);
    connect(this, SIGNAL (startListening()), handler, SLOT (process()));
    connect(handler, SIGNAL(plotDataGet(int)), this, SLOT(setupPlot(int)));
    connect(snapSave, SIGNAL (activated()), this, SLOT (snapshotSave()));
    connect(snapCopy, SIGNAL (activated()), this, SLOT (snapshotCopy()));

    handlerThread->start();
    writerThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plotterSetup(){
    ui->widget->clearPlottables();
    ui->widget->clearItems();
    ui->widget->clearGraphs();

    if(graphsCount>maxGraphs){
        graphsCount = maxGraphs;
    }
    for(int i=0; i<graphsCount; i++){
        ui->widget->addGraph();
        ui->widget->graph(i)->setPen(QPen(QColor(colors.value(i)), 1));
    }

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->widget->xAxis->setTicker(timeTicker);
    ui->widget->axisRect()->setupFullAxesBox();
    ui->widget->yAxis->setRange(yLower, yUpper);
    connect(&axisTimer, SIGNAL(timeout()), this, SLOT(axisReplot()));

    ui->label_3->setText(QString("Plots: " + QString::number(graphsCount)));

}

void MainWindow::axisReplot(){
    double key = globalTime.elapsed()/1000.0;
    static double lastPointKey = 0;
    if(key-lastPointKey > 0.002f){
    for(int i=0; i<graphsCount; i++){
        ui->widget->graph(i)->addData(key, values.value(i));
    }
    lastPointKey = key;
    }
    ui->widget->xAxis->setRange(key, xScale, Qt::AlignRight);
    ui->widget->replot();

    if(isWriting){
        dataWriter->writeDataToFile(&values);
    }

    QString format;
    for(int i=0; i<graphsCount; i++){
        format += QString("<font color=\"" + colors.value(i) + "\">███</font><b> " + QString::number(values.value(i)) + "</b>");
        format += "<br><br>";
    }
    ui->label_7->setText(format);
}

void MainWindow::setupPlot(int foundGraphsCount){
    graphsCount = foundGraphsCount;
    plotterSetup();
    axisTimer.start(replotTime);
}

void MainWindow::on_pushButton_clicked()
{
    if(connected){
        handler->close();
        ui->pushButton->setText("Connect");
        qDebug() << "Disconnected";
        ui->label_3->setText("Disconnected");
        axisTimer.stop();
        connected = false;
        dataWriter->stopWriting();
        isWriting = false;
    }else{
        portName = ui->comboBoxPort->currentText();
        portSpeed = portSpeeds.value(ui->comboBoxSpeed->currentIndex());
        handler->setPort(portName);
        handler->setSpeed(portSpeed);
        if(handler->open()){
            ui->pushButton->setText("Disconnect");
            qDebug() << "Connected";
            emit startListening();
            connected = true;
            if(logsEnabled){
                QString namef = "data/report_";
                QDateTime dateTime = QDateTime::currentDateTime();
                QDate date = dateTime.date();
                namef = namef + date.toString("ddMMyy") +"_" + dateTime.toUTC().toString("hhmmss") + ".txt";
                dataWriter->enableHeader(headerEnabled);
                dataWriter->openFile(namef);
                isWriting = true;
            }
        }else{
            ui->label_3->setText(QString("Error: unable to connect\nto port"+portName));
        }
    }
}

void MainWindow::on_spinBoxYTop_valueChanged(int arg1)
{
    yUpper = arg1;
    ui->widget->yAxis->setRange(yLower, yUpper);
}

void MainWindow::on_spinBoxYBottom_valueChanged(int arg1)
{
    yLower = arg1;
    ui->widget->yAxis->setRange(yLower, yUpper);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    xScale = arg1;
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    //duuuh
}

void MainWindow::on_pushButtonRefreh_clicked()
{
    ui->comboBoxPort->clear();
    for(int i=0; i<portInfo.availablePorts().size(); i++){
        ui->comboBoxPort->addItem(portInfo.availablePorts().value(i).portName());
    }

}

void MainWindow::on_checkBoxLogs_stateChanged(int arg1)
{
    if(arg1 == 0)
        logsEnabled = false;
    else
        logsEnabled = true;
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1 == 0)
        headerEnabled = false;
    else
        headerEnabled = true;
}

void MainWindow::on_pushButtonSnap_clicked()
{
    QString snap = "data/snapshot_";
    QDateTime dateTime = QDateTime::currentDateTime();
    QDate date = dateTime.date();
    snap = snap + date.toString("ddMMyy") +"_" + dateTime.toUTC().toString("hhmmss") + ".png";
    bool snapSaved = ui->widget->savePng(snap);
    qDebug() << "snapSaved:" << snapSaved;
}

void MainWindow::snapshotCopy(){
    qDebug() << "snapShotCopy triggered";
    QPixmap pixmap = ui->widget->toPixmap();
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setPixmap(pixmap);
}

void MainWindow::snapshotSave(){
    qDebug() << "snapShotSave triggered";
    QString snap = "data/snapshot_";
    QDateTime dateTime = QDateTime::currentDateTime();
    QDate date = dateTime.date();
    snap = snap + date.toString("ddMMyy") +"_" + dateTime.toUTC().toString("hhmmss") + ".png";
    bool snapSaved = ui->widget->savePng(snap);
    qDebug() << "snapSaved:" << snapSaved;
}
