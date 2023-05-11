#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPlainTextEdit>
#include <QMessageBox>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString inputString = ui->sourceCode->toPlainText();
    if(inputString==0)
    {
        QMessageBox::warning(nullptr,tr("?"),tr("空的"));
        return;
    }
    ui->solvedCode->clear();
    QTextCodec* fromcodec = QTextCodec::codecForName("GBK");
    QByteArray getstring = fromcodec->fromUnicode(inputString);

    QTextCodec* tocodec = QTextCodec::codecForName("Shift-JIS");

    QString outputString = tocodec->toUnicode(getstring);
    ui->solvedCode->insertPlainText(outputString);
}

