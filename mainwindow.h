#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QProcess>
#include <QString>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_CPU_clicked();

    void on_RAM_clicked();

    void on_GPU_clicked();

    void on_MONITOR_clicked();

    void on_BASEBOARD_clicked();

    void split(QString &result);

private:
    Ui::MainWindow *ui;
    QProcess process;
    QStringList arguments;
    QString currentLine;
    QString first_column;
    QString second_column;
    size_t found;
    QString gpu_first_column;
    QString gpu_second_column;


};




#endif // MAINWINDOW_H
