#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void plotterSetup();

public slots:
    void axisReplot();
    void setupPlot(int foundGraphsCount);

signals:
    void startListening();

private slots:
    void on_pushButton_clicked();

    void on_spinBoxYTop_valueChanged(int arg1);

    void on_spinBoxYBottom_valueChanged(int arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_pushButtonRefreh_clicked();

    void on_checkBoxLogs_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
