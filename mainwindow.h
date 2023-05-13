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
    void on_pushButton_clicked();

    void on_fileBrower_clicked(const QModelIndex &index);

    void on_pushButton_2_clicked();

    void on_filePath_returnPressed();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
};

#endif // MAINWINDOW_H
