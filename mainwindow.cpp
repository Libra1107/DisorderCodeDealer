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

    ui->folderBrower->setModel(model);
    ui->folderBrower->setColumnWidth(0, 320);

}

MainWindow::~MainWindow()
{
    delete ui;
}

QString lineTrans(const QString& codeLine, const QString& fromCode , const QString& toCode)
{
    QTextCodec* fromcodec = QTextCodec::codecForName(fromCode.toUtf8());
    QByteArray getstring = fromcodec->fromUnicode(codeLine);

    QTextCodec* tocodec = QTextCodec::codecForName(toCode.toUtf8());
    QString outputString = tocodec->toUnicode(getstring);
    return outputString;
}

void MainWindow::expandFolder(const QString& filepath)
{

    QModelIndex fileIndex = model->index(filepath);

    ui->folderExpand->setModel(model);
    ui->folderExpand->setRootIndex(fileIndex);
    ui->folderExpand->setColumnWidth(0,270);

    ui->folderExpand->expandToDepth(2);
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
        QString newFilePath = lineTrans(path, "GBK", "Shift-Jis");
        QFile::rename(path + ".trans", newFilePath);
    }
    else
    {
        QFile::rename(path + ".trans", path);
    }
}

void MainWindow::filePretrans(const QString& filepath)
{
    ui->filePreview->clear();
    ui->transPreview->clear();
    QFile readFile(filepath);

    if (!readFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        ui->filePreview->insertPlainText("无法读取");
        return;
    }

    QFileInfo fileInfo(filepath);
    qint64 fileSize = fileInfo.size();
    if (fileSize > 1024 * 1024) {
        QString message = "这个文件较大，会导致运行减慢";
        message += "。是否依然要预览";
        message += "？您也可以关闭始终预览选项";

        QMessageBox::StandardButton reply = QMessageBox::question(this, "文件过大", message);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    QTextStream readStream(&readFile);
    QTextCodec *readCodec = QTextCodec::codecForName("GBK");
    readStream.setCodec(readCodec);
    QString content = readStream.readAll();
    ui->filePreview->insertPlainText(content);

    QByteArray getContent = readCodec->fromUnicode(content);

    QTextCodec* tocodec = QTextCodec::codecForName("Shift-JIS");
    QString transContent = tocodec->toUnicode(getContent);

    ui->transPreview->insertPlainText(transContent);
}

void MainWindow::readAllFiles(const QString& path)
{
    QString newPath = lineTrans(path, "GBK", "Shift-Jis");
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
                QString newFilePath = lineTrans(filePath, "GBK", "Shift-Jis");
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
        QMessageBox::warning(nullptr,tr("提示"),tr("还请输入内容"));
        return;
    }
    ui->solvedCode->clear();
    QString outputString = lineTrans(inputString,ui->currentCodeBox->currentText(),ui->originCodeBox->currentText());
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
            ui->fileTrans->setEnabled(true);
            if(ui->previewAbled->isChecked())
            {
                MainWindow::filePretrans(filePath);
            }
        }
        else
        {
            ui->fileNameTrans->setDisabled(true);
            ui->fileTrans->setDisabled(true);
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
    QString folderPath = ui->folderPath->text();
    MainWindow::readAllFiles(folderPath);
}


void MainWindow::on_folderBrower_clicked(const QModelIndex &index)
{
    QString folderPath = model->filePath(index);
    ui->folderPath->setText(folderPath);
    QFileInfo folderInfo(folderPath);
    if (folderInfo.exists())
    {
        if (folderInfo.isDir())
        {
            ui->folderFix->setEnabled(true);
            ui->attention->setEnabled(true);
            MainWindow::expandFolder(folderPath);

        }
        else
        {
            ui->folderFix->setDisabled(true);
            ui->attention->setDisabled(true);
        }
    }

}


void MainWindow::on_folderPath_returnPressed()
{
    QString folderPath = ui->folderPath->text();

    QDir dir(folderPath);
    if (!dir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("输入的路径不存在，将返回系统目录"));
    }

    QModelIndex folderIndex = model->index(folderPath);
    ui->folderBrower->setRootIndex(folderIndex);

}

