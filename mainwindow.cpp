#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QIcon>
#include <QDir>
#include <QFileDialog>
#include <QStringList>

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


void MainWindow::contentTrans(const QString& path)
{

    QFile readFile(path);
    if (!readFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr,tr("错误"),tr("无法打开文件"));
        return;
    }

    QTextStream readStream(&readFile);
    QTextCodec *readCodec = QTextCodec::codecForName("GBK");
    readStream.setCodec(readCodec);

    QFile writeFile(path + ".trans");
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
        QString newFilePath = lineTrans(path);
        QFile::rename(path + ".trans", newFilePath);
    }
    else
    {
        QFile::rename(path + ".trans", path);
    }
}


void MainWindow::readAllFiles(const QString& path)
{
    QString newPath = path;
    if(ui->fileNameTrans->isChecked())
    {
        newPath = lineTrans(path);
    }

    QFileInfo fileInfo(path);

    if (fileInfo.exists())
    {
        if (fileInfo.isFile())
        {
            MainWindow::contentTrans(path);
        }
        else if (fileInfo.isDir())
        {
            QDir dir;
            dir.mkpath(newPath);

            QDirIterator folderIt(path, QDir::Dirs | QDir::NoDotAndDotDot);
            while (folderIt.hasNext())
            {
                QString folderPath = folderIt.next();
                MainWindow::readAllFiles(folderPath);
            }

            QDirIterator it(path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
            while (it.hasNext())
            {
                QString filePath = it.next();
                MainWindow::contentTrans(filePath);
            }

            QDir(path).removeRecursively();
        }
    }
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
    MainWindow::readAllFiles(filePath);

}


void MainWindow::on_filePath_returnPressed()
{
    QString filePath = ui->filePath->text();

    QDir dir(filePath);
    if (!dir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("输入的路径不存在，将返回系统目录"));
    }

    QModelIndex fileIndex = model->index(filePath);
    ui->fileBrower->setRootIndex(fileIndex);
}

