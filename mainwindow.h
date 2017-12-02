#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>
#include <qtspell/QtSpell.hpp>

namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void textAlign(QAction *a);

};

#endif // MAINWINDOW_H
