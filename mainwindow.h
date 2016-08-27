#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AgvCommandProcess.h"
#include "AgvCode.h"
#include "AgvLogs.h"
#include <QMainWindow>
#include <QTimer>

#include <opencv2/opencv.hpp>


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
    QTimer *timer;
    IplImage* frame;
    CvCapture* capture;
    QImage image;

    CAgvCode agvCode;
    CAgvLogs agvLogs;
    CAgvCommandProcess agvCommandProcess;
    pthread_mutex_t capture_mutex;
private slots:
    void cameraCapture();
    void on_btnTakePic_clicked();

private:
    void imgOpencvProcess(const char *filename);
    int getVideoDevice(void);

};

#endif // MAINWINDOW_H
