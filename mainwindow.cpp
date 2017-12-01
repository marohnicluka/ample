#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QActionGroup *alignGroup = new QActionGroup(this);
    alignGroup->addAction(ui->actionAlignLeft);
    alignGroup->addAction(ui->actionAlignCenter);
    alignGroup->addAction(ui->actionAlignRight);
    alignGroup->addAction(ui->actionAlignJustify);
    alignGroup->setExclusive(true);
    connect(alignGroup, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));
}

void MainWindow::textAlign(QAction *a) {
    if (a == ui->actionAlignLeft)
        qDebug("Left");
    //textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == ui->actionAlignCenter)
        qDebug("Center");
    //textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == ui->actionAlignRight)
        qDebug("Right");
    //textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == ui->actionAlignJustify)
        qDebug("Justified");
    //textEdit->setAlignment(Qt::AlignJustify);
}

MainWindow::~MainWindow()
{
    delete ui;
}
