#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // First check XML
    QDomDocument document;
    QFile xml("ruet.xml");
    // Opening file
    if(!xml.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error!", "Could not find the file <b>ruet.xml</b>", QMessageBox::Ok);
        exit(0);
    }
    document.setContent(&xml);
    xml.close();
    // File closed

    // Setting students tree to NodeList
    studentList = document.firstChildElement().elementsByTagName("student");

    // Building UI
    ui->setupUi(this);
    ui->tableMarks->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableMarks->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableMarks->setHorizontalHeaderLabels(QStringList() << "Subject" << "Points" << "Grade");

    // Create the plot
    setupPlot();

    // Add student IDs to combo menu
    for(int i=0; i<studentList.count();i++)
        ui->comboStudentID->addItem(studentList.at(i).toElement().attribute("id"));

    connect(ui->buttonShow, SIGNAL(clicked()), this, SLOT(showAllAction()));
    connect(ui->buttonExit, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->buttonPrint, SIGNAL(clicked()), this, SLOT(printAction()));
    connect(ui->buttonSaveToText, SIGNAL(clicked()), this, SLOT(savetotextAction()));

    showMaximized();
    showAllAction();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPlot()
{
    plot = new QCustomPlot(ui->centralWidget);
    ui->gridPlot->addWidget(plot);

    abBar = new QCPBars(plot->xAxis, plot->yAxis);
    cqBar = new QCPBars(plot->xAxis, plot->yAxis);
    wpBar = new QCPBars(plot->xAxis, plot->yAxis);

    abBar->setWidth(0.8);
    cqBar->setWidth(0.8);
    wpBar->setWidth(0.8);

    plot->addPlottable(abBar);
    plot->addPlottable(cqBar);
    plot->addPlottable(wpBar);

    QPen pen;
    pen.setWidthF(1);

    abBar->setName("Attendance/Board Viva");
    pen.setColor(Qt::green);
    abBar->setPen(pen);
    abBar->setBrush(QColor(0, 255, 0, 75));

    cqBar->setName("Class Test/Quiz");
    pen.setColor(Qt::red);
    cqBar->setPen(pen);
    cqBar->setBrush(QColor(255, 0, 0, 75));

    wpBar->setName("Written/Performence");
    pen.setColor(Qt::blue);
    wpBar->setPen(pen);
    wpBar->setBrush(QColor(0, 0, 255, 75));

    abBar->moveAbove(wpBar);
    cqBar->moveAbove(wpBar);

    // prepare x axis
    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setAutoTickLabels(false);
    plot->xAxis->setTickLabelRotation(0);
    plot->xAxis->setSubTickCount(0);
    plot->xAxis->setGrid(true);
    plot->xAxis->setRange(0, 10);
    plot->xAxis->setLabel("Subjects");

    // prepare y axis:
    plot->yAxis->setRange(0, 100);
    plot->yAxis->setPadding(5);
    plot->yAxis->setLabel("Marks");
    plot->yAxis->setSubGrid(true);

    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    plot->yAxis->setGridPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    plot->yAxis->setSubGridPen(gridPen);

    // setup legend:
    plot->legend->setVisible(true);
    plot->legend->setPositionStyle(QCPLegend::psBottomRight);
    plot->legend->setBrush(QColor(255, 255, 255, 200));
    QPen legendPen;
    legendPen.setColor(QColor(130, 130, 130, 200));
    plot->legend->setBorderPen(legendPen);
    QFont legendFont = font();
    legendFont.setPointSize(8);
    plot->legend->setFont(legendFont);

    plot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    plot->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}

void MainWindow::showAllAction() // Show Trigger
{
    int roll = ui->comboStudentID->currentIndex();
    int semester = ui->comboSemesterID->currentIndex();

    ui->textInformations->setText("ID: <b>" + ui->comboStudentID->currentText() + "</b><br/>" +
                                  "Name: <b>" + studentList.at(roll).toElement().attribute("name") + "</b><br/>" +
                                  "Address: <b>" + studentList.at(roll).toElement().attribute("address") + "</b><br/>");

    ui->labelSGPA->setText("<b>" + studentList.at(roll).childNodes().at(semester).toElement().attribute("sgpa") + "</b>");

    QDomNodeList listSubjects = studentList.at(roll).childNodes().at(semester).childNodes();

    //Clear All Containers
    ticks.clear();
    Subjects.clear();
    attendance_boardviva.clear();
    classtest_quiz.clear();
    written_performence.clear();

    //Clear cpBar Data
    abBar->clearData();
    cqBar->clearData();
    wpBar->clearData();

    ui->tableMarks->setRowCount(listSubjects.count());

    for(int i=0; i<listSubjects.count();i++)
    {
        QDomElement subject = listSubjects.at(i).toElement();
        ticks << i+1;
        Subjects << subject.attribute("code");
        attendance_boardviva << subject.attribute("ab").toDouble();
        classtest_quiz << subject.attribute("cq").toDouble();
        written_performence << subject.attribute("wp").toDouble();

        int _point = attendance_boardviva.at(i)+ classtest_quiz.at(i)+ written_performence.at(i);
        QTableWidgetItem *name = new QTableWidgetItem(QString(Subjects.at(i)));
        QTableWidgetItem *point = new QTableWidgetItem(QString::number(_point));
        QTableWidgetItem *grade = new QTableWidgetItem();

        if(_point>=80)
        {
            name->setData(Qt::BackgroundRole, _colAplus);
            grade->setText("A+");
        }
        else if(_point>=75)
        {
            name->setData(Qt::BackgroundRole, _colA);
            grade->setText("A");
        }
        else if(_point>=70)
        {
            name->setData(Qt::BackgroundRole, _colAminus);
            grade->setText("A-");
        }
        else if(_point>=65)
        {
            name->setData(Qt::BackgroundRole, _colBplus);
            grade->setText("B+");
        }
        else if(_point>=60)
        {
            name->setData(Qt::BackgroundRole, _colB);
            grade->setText("B");
        }
        else if(_point>=55)
        {
            name->setData(Qt::BackgroundRole, _colBminus);
            grade->setText("B-");
        }
        else if(_point>=50)
        {
            name->setData(Qt::BackgroundRole, _colCplus);
            grade->setText("C+");
        }
        else if(_point>=45)
        {
            name->setData(Qt::BackgroundRole, _colC);
            grade->setText("C");
        }
        else if(_point>=40)
        {
            name->setData(Qt::BackgroundRole, _colD);
            grade->setText("D");
        }
        else if(_point<40)
        {
            name->setData(Qt::BackgroundRole, _colF);
            grade->setText("F");
        }

        ui->tableMarks->setItem(i,0,name);
        ui->tableMarks->setItem(i,1,point);
        ui->tableMarks->setItem(i,2,grade);

        name->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
        point->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
        grade->setData(Qt::TextAlignmentRole, Qt::AlignCenter);

    } // End of loop

    //Set Data
    abBar->setData(ticks, attendance_boardviva);
    cqBar->setData(ticks, classtest_quiz);
    wpBar->setData(ticks, written_performence);

    //Plot
    plot->xAxis->setTickVector(ticks);
    plot->xAxis->setTickVectorLabels(Subjects);
    plot->replot();
}

void MainWindow::printAction()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Where to save...", QDir::homePath(), tr("PDF File (*.pdf)"));
    if(fileName.isEmpty())
    {
        QMessageBox::critical(this, "Error!", "PDF save failed!!", QMessageBox::Ok);
        return;
    }
    else
        plot->savePdf(fileName);
    QMessageBox::information(this, "Success!", "PDF saved!", QMessageBox::Ok);
}

void MainWindow::savetotextAction()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Where to save...", QDir::homePath(), tr("Text File (*.txt)"));
    QFile exportFile(fileName);
    if(fileName.isEmpty())
    {
        QMessageBox::critical(this, "Error!", "Text save failed!!", QMessageBox::Ok);
        return;
    }
    else
        exportFile.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream textStream(&exportFile);

    textStream << "=== Result for " << ui->comboStudentID->currentText() << " ===\n";
    textStream << "\nName: " << studentList.at(ui->comboStudentID->currentIndex()).toElement().attribute("name");
    textStream << "\nAddress: " << studentList.at(ui->comboStudentID->currentIndex()).toElement().attribute("address");
    textStream << "\nSemester: " << ui->comboSemesterID->currentText();
    textStream << "\nSGPA: " << studentList.at(ui->comboStudentID->currentIndex()).childNodes().at(ui->comboSemesterID->currentIndex()).toElement().attribute("sgpa") << "\n\n";

    for(int i=0;i<ui->tableMarks->rowCount();i++)
        textStream << "\nSubject: " << ui->tableMarks->item(i,0)->text()
                   << "\tPoints: " << attendance_boardviva.at(i)+ classtest_quiz.at(i)+ written_performence.at(i);

    textStream.flush();
    exportFile.close();

    QMessageBox::information(this, "Success!", "Text saved!", QMessageBox::Ok);
}
