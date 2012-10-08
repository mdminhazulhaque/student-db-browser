#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QFileDialog>
#include <QtXml>
#include <QDebug>
#include <QMessageBox>
#include <QMainWindow>
#include "defines.h"
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setupPlot();
    
private slots:
    void showAllAction();
    void printAction();
    void savetotextAction();

private:
    Ui::MainWindow *ui;
    QDomNodeList studentList;
    QCustomPlot *plot;
    QVector<double> attendance_boardviva, classtest_quiz, written_performence, ticks;
    QVector<QString>Subjects;
    QCPBars *abBar, *cqBar, *wpBar;
};

#endif // MAINWINDOW_H
