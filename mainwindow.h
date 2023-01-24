#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QProcess>
#include <QLabel>
#include <deque>
#include <QSplineSeries>

class Line
{
public:
    int x;
    int y;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString formatTime(int s);
    std::deque<Line> Musage;
    std::deque<Line> Susage;
    std::deque<Line> Cpu0usage;
    std::deque<Line> Cpu1usage;
    std::deque<Line> Cpu2usage;
    std::deque<Line> Cpu3usage;
    QStringList string_list; // to store dir name
    QtCharts::QSplineSeries *splineSeries_m,
                            *splineSeries_s,
                            *splineSeries_cpu0,
                            *splineSeries_cpu1,
                            *splineSeries_cpu2,
                            *splineSeries_cpu3;
    int processNum;
private:
    Ui::MainWindow *ui;
private slots:
    void getMemAvailable();
    void getRuntime();
    void showNowTime();
    void on_ShutdownButton_clicked();
    void on_RebootButton_clicked();
    void getSwapFree();
    void showMemline();
    void showCpuline();
    void getCpu0();
    void getCpu1();
    void getCpu2();
    void getCpu3();
    void showprocess();
    void showmodule();
    void on_KillButton_clicked();
    void on_SearchButton_clicked();
    void on_DetailButton_clicked();
    void on_AboutQTButton_clicked();
    void on_AboutMonitorButton_clicked();
    void on_ExitButton_clicked();
};

#endif // MAINWINDOW_H
