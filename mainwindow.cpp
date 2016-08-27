#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "AgvCode.h"

#include <QDateTime>
#include <sys/time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define SRC_IMG    "src.jpg"
#define SRC_QR_IMG "srcqr.jpg"
#define SRC_THRESHOLD_IMG "srcthreshold.jpg"
#define SRC_CLOSE_IMG "srcclose.jpg"
#define SRC_GRAY_IMG "srcgray.jpg"
#define SRC_ERODE_IMG "srcerode.jpg"
#define SRC_BLUR_IMG "srcblur.jpg"
#define SRC_DILATE_IMG "srcdilate.jpg"
#define SRC_WISE_IMG "srcwise.jpg"
#define SRC_ROI_IMG "srcroi.jpg"

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    agvLogs.SaveInfo("MainWindow start");
    ui->setupUi(this);

    capture = cvCaptureFromCAM(getVideoDevice());
    pthread_mutex_init(&capture_mutex, NULL);
    if(NULL == capture)
    {
        qDebug("Invoke cvCaptureFromCAM failed");
    }
    else
    {
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH , 640);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);

        timer = new QTimer(this);
        timer->start(1);

        //Figu:thread maybe it is more better
        connect(timer,SIGNAL(timeout()),this,SLOT(cameraCapture()));

    }

    agvCommandProcess.SetAgvCode(&agvCode);
    agvCommandProcess.SetAgvLogs(&agvLogs);
    agvCommandProcess.SetCvCapture(capture, &capture_mutex);
    agvCommandProcess.Start();
}

MainWindow::~MainWindow()
{
    timer->stop();
    cvReleaseCapture(&capture);
    pthread_mutex_destroy(&capture_mutex);
    delete ui;
}

void MainWindow::cameraCapture()
{
    //return;
    pthread_mutex_lock(&capture_mutex);
    frame = cvQueryFrame(capture);
    pthread_mutex_unlock(&capture_mutex);

    if(frame != NULL)//Figure:very important
    {

        image = QImage((const uchar*)frame->imageData, frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();
        ui->lbPic->setPixmap(QPixmap::fromImage(image));

    }
    else
    {
        qDebug("frame eq null");
    }


}


void MainWindow::on_btnTakePic_clicked()
{
    ui->txdAngle->setText("");
    ui->txdGravityX->setText("");
    ui->txdGravityY->setText("");
    ui->txdCode->setText("");
    ui->txdCostTime->setText("");


    struct timeval t_start,t_end;
    long cost_time = 0;
    gettimeofday(&t_start, NULL);
    long start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;

    pthread_mutex_lock(&capture_mutex);
    frame = cvQueryFrame(capture);
    pthread_mutex_unlock(&capture_mutex);

    if(frame != NULL)//Figure:very important
    {
        //QDateTime dt = QDateTime::currentDateTime();
        //QString strCurrentDateTime = dt.toString("yyyy-MM-dd-hh-mm-ss-zzz");
        //strCurrentDateTime += ".jpg";
        //cvSaveImage(strCurrentDateTime.toStdString().c_str(),frame);


        //cvFlip(frame,frame,1);
        cvSaveImage(SRC_IMG,frame);

        image = QImage((const uchar*)frame->imageData, frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();
        ui->lbPic->setPixmap(QPixmap::fromImage(image));

        imgOpencvProcess(SRC_IMG);


    }
    else
    {
        qDebug("frame eq NUL");
    }

    gettimeofday(&t_end, NULL);
    long end = ((long)t_end.tv_sec)*1000+(long)t_end.tv_usec/1000;

    cost_time = end - start;
    ui->txdCostTime->setText(QString::number(cost_time,10));

}

bool compareContours(vector<cv::Point> one,vector<cv::Point> two)
{
    return (cv::contourArea(one) > cv::contourArea(two));
}
void SetImageRectColor(Mat image);
void MainWindow::imgOpencvProcess(const char *filename)
{
#if 0
    Mat src_img_roi = imread(filename,CV_LOAD_IMAGE_COLOR);
    Mat src_img = src_img_roi(Rect(src_img_roi.cols/10,src_img_roi.rows/10,src_img_roi.cols*8/10,src_img_roi.rows*8/10));
    imwrite(SRC_ROI_IMG,src_img);
#else
    Mat src_img = imread(filename,CV_LOAD_IMAGE_COLOR);
#endif
    Mat qr_img;

    if (NULL == src_img.data)
    {
        qDebug("Invoke imread failed");
        return;
    }

    cvtColor(src_img,src_img,CV_RGB2GRAY);
    //imwrite(SRC_GRAY_IMG,src_img);
    blur(src_img,src_img,Size(3,3));
    //imwrite(SRC_BLUR_IMG,src_img);
    erode(src_img, src_img, Mat());
    erode(src_img, src_img, Mat());
    //imwrite(SRC_ERODE_IMG,src_img);
    dilate(src_img, src_img, Mat());
    dilate(src_img, src_img, Mat());
    //imwrite(SRC_DILATE_IMG,src_img);
    adaptiveThreshold(src_img,src_img,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,199,10);
    //imwrite(SRC_THRESHOLD_IMG,src_img);
	SetImageRectColor(src_img);
    cv::resize(src_img,qr_img,Size(320,240));

    if(!imwrite(SRC_QR_IMG,qr_img))
    {
        qDebug("Invoke imwrite failed");
        return;
    }

    string code;
    vector<cv::Point> points;
    if(agvCode.ReadCode(SRC_QR_IMG,&code,points) != 0)
    {
         ui->txdCode->setText("No Data");
         agvLogs.SaveError("We can't read QR code");
         //if(agvCode.CheckQrCode(SRC_IMG, &agvLogs) == 0){
         //   agvLogs.SaveError("We can't check QR code");
            return;
         //}
    }
    else
    {
        ui->txdCode->setText(QString::fromStdString(code));
        agvLogs.SaveInfo(code);
        RotatedRect rect;
        rect = minAreaRect(points);
        ui->txdAngle->setText(QString::number(rect.angle));
        ui->txdGravityX->setText(QString::number((int)rect.center.x*2,10));
        ui->txdGravityY->setText(QString::number((int)rect.center.y*2,10));
    }
#if 0
    //Figure:gravity and angle
    bitwise_not(src_img,src_img);
    //imwrite(SRC_WISE_IMG,src_img);
    vector<vector<cv::Point> > contours;
    vector<Vec4i> hierarchy;
    Mat contours_img;
    RotatedRect rect;
    Mat kernel_img;

    for(int i=35; i<41; i++)
    {
        kernel_img =  getStructuringElement(MORPH_RECT,Size(i,i));//Figure
        morphologyEx(src_img,contours_img,MORPH_CLOSE,kernel_img);
        imwrite(SRC_CLOSE_IMG,contours_img);
        findContours( contours_img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

        qDebug("size of contours:%d",(int)contours.size());

        if(contours.size() > 0)
        {
            qDebug("finded it");

            std::sort(contours.begin(),contours.end(),compareContours);

            rect = minAreaRect(contours[0]);
            ui->txdAngle->setText(QString::number(rect.angle));
            ui->txdGravityX->setText(QString::number((int)rect.center.x,10));
            ui->txdGravityY->setText(QString::number((int)rect.center.y,10));

            break;
        }


    }
#endif




}


int MainWindow::getVideoDevice(void)
{
    int videoDevice = 0;
    char* videoFile = "videoFile.txt";
    FILE *fp = NULL;
    char buf[32] = {0};

    system("ls /dev/video* > videoFile.txt");
    if(access(videoFile, 0) == 0){
        fp = fopen(videoFile, "r");
        if(fp){
            fgets(buf,32,fp);
            if(strlen(buf) > strlen("/dev/video")){
                sscanf(buf,"/dev/video %d",&videoDevice);
            }
            fclose(fp);
        }
    }

    return videoDevice;
}
