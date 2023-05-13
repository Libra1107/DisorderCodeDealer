#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QFileSystemModel(this);
    model->setRootPath(QDir::currentPath());
    ui->fileBrower->setModel(model);
    ui->fileBrower->setColumnWidth(0, 320);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString lineTrans(const QString& codeLine)
{
    QTextCodec* fromcodec = QTextCodec::codecForName("GBK");
    QByteArray getstring = fromcodec->fromUnicode(codeLine);

    QTextCodec* tocodec = QTextCodec::codecForName("Shift-JIS");\
    QString outputString = tocodec->toUnicode(getstring);
    return outputString;
}

void MainWindow::on_pushButton_clicked()
{
    QString inputString = ui->sourceCode->toPlainText();
    if(inputString==0)
    {
        QMessageBox::warning(nullptr,tr("提示"),tr("输入内容"));
        return;
    }
    ui->solvedCode->clear();
    QString outputString = lineTrans(inputString);
    ui->solvedCode->insertPlainText(outputString);
}


void MainWindow::on_fileBrower_clicked(const QModelIndex &index)
{
    QString filePath = model->filePath(index);
    ui->filePath->setText(filePath);
}


void MainWindow::on_pushButton_2_clicked()
{
    QString filePath = ui->filePath->text();
    if (!filePath.endsWith(".txt", Qt::CaseInsensitive)) {
        QMessageBox::warning(nullptr,tr("抱歉"),tr("暂不支持非文本类文件"));
        return;
    }

    QFile readFile(filePath);
    if (!readFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr,tr("错误"),tr("无法打开文件"));
        return;
    }

    QTextStream readStream(&readFile);
    QTextCodec *readCodec = QTextCodec::codecForName("GBK");
    readStream.setCodec(readCodec);

    QFile writeFile(filePath + ".trans");
    if (!writeFile.open(QIODevice::Text | QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr,tr("错误"),tr("无法写入"));
        return;
    }

    QTextStream writeStream(&writeFile);
    QTextCodec *transCodec = QTextCodec::codecForName("Shift-Jis");
    QTextCodec *writeCodec = QTextCodec::codecForName("UTF-8");
    writeStream.setCodec(writeCodec);

    while (!readStream.atEnd()) {
        QString line = readStream.readLine();
        QByteArray getLine = readCodec->fromUnicode(line);
        QString outputString = transCodec->toUnicode(getLine);
        writeStream<<outputString<<'\n';
    }

    readFile.close();
    readFile.remove();
    writeFile.close();

    if(ui->fileNameTrans->isChecked())
    {
        QFile::rename(filePath + ".trans", lineTrans(filePath));
    }
    else
    {
        QFile::rename(filePath + ".trans", filePath);
    }
}

