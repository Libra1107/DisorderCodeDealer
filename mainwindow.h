#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>

#include <QPlainTextEdit>
#include <QTextCodec>
#include <QTextStream>
#include <QMessageBox>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void readAllFiles(const QString& path);

    void contentTrans(const QString& path);

private slots:
    void on_codeTrans_clicked();

    void on_fileBrower_clicked(const QModelIndex &index);

    void on_fileTrans_clicked();

    void on_filePath_returnPressed();

    void on_folderFix_clicked();

    void on_folderBrower_clicked(const QModelIndex &index);

    void on_folderPath_returnPressed();

    void filePretrans(const QString& filepath);

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
};

#endif // MAINWINDOW_H
