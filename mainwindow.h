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

bool isUTF8(const char *data, int length)
{
    int i = 0;
    while (i < length)
    {
        if ((data[i] & 0x80) == 0)
        {i++;}
        else if ((data[i] & 0xE0) == 0xC0)
        {
            if (i + 1 >= length || (data[i + 1] & 0xC0) != 0x80)
            {return false;}
            i += 2;
        }
        else if ((data[i] & 0xF0) == 0xE0)
        {
            if (i + 2 >= length || (data[i + 1] & 0xC0) != 0x80 || (data[i + 2] & 0xC0) != 0x80)
            {return false;}
            i += 3;
        }
        else if ((data[i] & 0xF8) == 0xF0)
        {
            if (i + 3 >= length || (data[i + 1] & 0xC0) != 0x80 || (data[i + 2] & 0xC0) != 0x80 || (data[i + 3] & 0xC0) != 0x80)
            {return false;}
            i += 4;
        }
        else
        {return false;}
    }
    return true;
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
    QString newPath = lineTrans(path);
    QFileInfo fileInfo(path);

    if (fileInfo.exists())
    {
        if (fileInfo.isDir())
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
                QString newFilePath = lineTrans(filePath);
                QFile::rename(filePath, newFilePath);
            }

            QDir(path).removeRecursively();
        }
    }
}

void MainWindow::on_codeTrans_clicked()
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
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists())
    {
        if (fileInfo.isFile())
        {
            ui->fileNameTrans->setEnabled(true);
            ui->fileNameTrans->setChecked(true);
            ui->fileTrans->setEnabled(true);
            ui->folderFix->setDisabled(true);
            ui->attention->setDisabled(true);
        }
        else if (fileInfo.isDir())
        {
            ui->fileNameTrans->setDisabled(true);
            ui->fileNameTrans->setChecked(false);
            ui->fileTrans->setDisabled(true);
            ui->folderFix->setEnabled(true);
            ui->attention->setEnabled(true);
        }
    }
}

void MainWindow::on_fileTrans_clicked()
{
    QString filePath = ui->filePath->text();
    MainWindow::contentTrans(filePath);
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


void MainWindow::on_folderFix_clicked()
{
    QString folderPath = ui->filePath->text();
    MainWindow::readAllFiles(folderPath);
}

