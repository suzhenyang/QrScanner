#include "AgvCommandProcess.h"
//#include <stdio.h>
#include <time.h>
//#include <sys/time.h>
//#include <stdlib.h>
#include <signal.h>
#include <errno.h>

using namespace cv;

#define SRC_IMG         "src_command.jpg"
#define SRC_QR_IMG      "src_qr_command.jpg"
#define SRC_THRESHOLD_IMG "src_threshold_command.jpg"
#define SRC_CLOSE_IMG "src_close_command.jpg"
#define SRC_GRAY_IMG "src_gray_command.jpg"
#define SRC_ERODE_IMG "src_erode_command.jpg"
#define SRC_BLUR_IMG "src_blur_command.jpg"
#define SRC_DILATE_IMG "src_dilate_command.jpg"
#define SRC_WISE_IMG "src_wise_command.jpg"
#define SRC_ROI_IMG "src_roi_command.jpg"

static int command_process_thread_running = 0;
static int heart_beat_thread_running = 0;
static int connect_status = 0;//0-no connect   1-connect

CAgvCommandProcess::CAgvCommandProcess()
{
    agvSocket = new CAgvSocketClient("192.168.1.12",7003);
    if(NULL == agvSocket)
    {
        perror("Invoke CAgvSocketClient failed");
    }
    else
    {
        signal(SIGPIPE, SIG_IGN);
    }
}

CAgvCommandProcess::~CAgvCommandProcess()
{
    if(command_process_thread_running)
    {
        command_process_thread_running = 0;
        pthread_join(command_process_thread,NULL);
    }
    if(heart_beat_thread_running)
    {
        heart_beat_thread_running = 0;
        pthread_join(heart_beat_thread,NULL);
    }
    pthread_mutex_destroy(&socket_mutex);

    if(NULL != agvSocket)
    {
        agvSocket->SocketClose();
        delete agvSocket;
    }
}

void CAgvCommandProcess::SetCvCapture(CvCapture *capture, pthread_mutex_t *capture_mutex)
{
    this->capture = capture;
    this->capture_mutex = capture_mutex;
}

void CAgvCommandProcess::SetAgvCode(CAgvCode *agvCode)
{
    this->agvCode = agvCode;
}

void CAgvCommandProcess::SetAgvLogs(CAgvLogs *agvLogs)
{
    this->agvLogs = agvLogs;
}

void CAgvCommandProcess::Start(void)
{
    if(NULL != agvSocket)
    {
        pthread_attr_t  thread_attr;

        pthread_mutex_init(&socket_mutex, NULL);

        pthread_attr_init (&thread_attr);
        pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED);
        command_process_thread_running = 1;
        if (pthread_create (&command_process_thread, &thread_attr, CommandProcessThread, this) != 0)
        {
            command_process_thread_running = 0;
            perror ("pthread_create failed");
        }
        heart_beat_thread_running = 1;
        if (pthread_create (&heart_beat_thread, &thread_attr, HeartBeatThread, this) != 0)
        {
            heart_beat_thread_running = 0;
            perror ("pthread_create failed");
        }
        pthread_attr_destroy (&thread_attr);
    }
}

bool compareContours(vector<cv::Point> one,vector<cv::Point> two);

string int2string(int number)
{
    char buf[16];
    snprintf(buf,16,"%d",number);
    string str(buf);
    return str;
}

void SetImageRectColor(Mat image)
{
    Point points[1][20];
    points[0][0] = Point( 0, image.rows*19/20 );
    points[0][1] = Point( image.cols-1, image.rows*19/20 );
    points[0][2] = Point( image.cols-1, image.rows-1 );
    points[0][3] = Point( 0, image.rows-1 );

    const Point* pt[1] = { points[0] };
    int npt[1] = {4};

    fillPoly( image, pt, npt, 1, Scalar(255,255,255), 8);


}

int CommandGetDegree(unsigned short *degree, short *distance_x, short *distance_y, CvCapture *capture, pthread_mutex_t *capture_mutex, CAgvCode *agvCode, CAgvLogs *agvLogs)
{
    IplImage* frame;
    int ret = -1;
    string code;
    Mat qr_img;
    int qr_success = 1;

    *degree = 0;
    *distance_x = 0;
    *distance_y = 0;

    if(capture == NULL){
        agvLogs->SaveError ("CommandGetDegree, capture == NULL\n");
        sleep(1);
        return ret;
    }

    pthread_mutex_lock(capture_mutex);
    frame = cvQueryFrame(capture);
    pthread_mutex_unlock(capture_mutex);

    if(frame != NULL)//Figure:very important
    {
        //QDateTime dt = QDateTime::currentDateTime();
        //QString strCurrentDateTime = dt.toString("yyyy-MM-dd-hh-mm-ss-zzz");
        //strCurrentDateTime += ".jpg";
        //cvSaveImage(strCurrentDateTime.toStdString().c_str(),frame);

        //cvFlip(frame,frame,1);
        cvSaveImage(SRC_IMG,frame);

        //image = QImage((const uchar*)frame->imageData, frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();
        //ui->lbPic->setPixmap(QPixmap::fromImage(image));

        #if 0
            Mat src_img_roi = imread(SRC_IMG,CV_LOAD_IMAGE_COLOR);
            Mat src_img = src_img_roi(Rect(src_img_roi.cols/10,src_img_roi.rows/10,src_img_roi.cols*8/10,src_img_roi.rows*8/10));
            imwrite(SRC_ROI_IMG,src_img);
        #else
            Mat src_img = imread(SRC_IMG,CV_LOAD_IMAGE_COLOR);
        #endif
        if (NULL == src_img.data)
        {
            agvLogs->SaveError("Invoke imread failed\n");
            sleep(1);
            return ret;
        }
        cvtColor(src_img,src_img,CV_RGB2GRAY);

        blur(src_img,src_img,Size(3,3));
        erode(src_img, src_img, Mat());
        erode(src_img, src_img, Mat());
        dilate(src_img, src_img, Mat());
        dilate(src_img, src_img, Mat());
        adaptiveThreshold(src_img,src_img,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,199,10);
        SetImageRectColor(src_img);
        cv::resize(src_img,qr_img,Size(320,240));
        imwrite(SRC_QR_IMG,qr_img);
        vector<cv::Point> points;
        if(agvCode->ReadCode(SRC_QR_IMG,&code,points) != 0)
        {
            agvLogs->SaveError("ReadCode failed");
            //if(agvCode->CheckQrCode(SRC_IMG, agvLogs) == 0){
            //    agvLogs->SaveError("CheckQrCode failed");
                qr_success = 0;
            //}
        }else{
			agvLogs->SaveInfo(code);
		}
 #if 1
        RotatedRect rect;
        if(qr_success){
            rect = minAreaRect(points);
            *degree = (unsigned short)(fabs(rect.angle)*60);
            *distance_x = (short)((rect.center.x*2-src_img.cols/2)*82)/480;
            *distance_y = (short)((rect.center.y*2-src_img.rows/2)*82)/480;
            if(code.compare("1000-6000") == 0){
                *distance_y -= 60;
            }else if(code.compare("1002-6002") == 0){
                *distance_y += 60;
            }
            ret = 0;
        }else{
            *degree = 0;
            *distance_x = 0;
            *distance_y = 0;
        }
        agvLogs->SaveInfo("distance_x of contour:"+int2string((int)(*distance_x)));
        agvLogs->SaveInfo("distance_y of contour:"+int2string((int)(*distance_y)));
#else
        //Figure:gravity and angle
        bitwise_not(src_img,src_img);

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

            agvLogs->SaveInfo("size of contours:"+int2string((int)contours.size()));

            if(contours.size() > 0)
            {
                agvLogs->SaveInfo("finded it");

                std::sort(contours.begin(),contours.end(),compareContours);

                agvLogs->SaveInfo("area of contour:"+int2string((int)cv::contourArea(contours[0])));
                rect = minAreaRect(contours[0]);
                *degree = (unsigned short)(fabs(rect.angle)*60);
                *distance_x = (short)((rect.center.x-src_img.cols/2)*82)/480;
                *distance_y = (short)((rect.center.y-src_img.rows/2)*82)/480;
                if(qr_success){
                    if(code.compare("1000-6000") == 0){
                        *distance_y -= 60;
                    }else if(code.compare("1002-6002") == 0){
                        *distance_y += 60;
                    }
                    ret = 0;
                }else{
					*distance_x = 0;
					*distance_y = 0;
				}
                agvLogs->SaveInfo("distance_x of contour:"+int2string((int)(*distance_x)));
                agvLogs->SaveInfo("distance_y of contour:"+int2string((int)(*distance_y)));				
                break;
            }


        }
#endif
    }
    else
    {
        agvLogs->SaveError("frame eq NUL\n");
        sleep(1);
        return ret;
    }

    return ret;
}

void SendCommand(CAgvSocketClient *agvSocket, unsigned char *buf, int buf_len)
{
    int ret = 0;

    if(connect_status == 1){
        ret = agvSocket->SocketWrite(buf,buf_len);//send
        if(ret == 0){
            agvSocket->SocketClose();
            connect_status = 0;
        }else if(ret < 0){
            if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){
                ;
            }else{
               agvSocket->SocketClose();
               connect_status = 0;
            }
        }
    }
}

void CommandRequestDegree(CAgvSocketClient *agvSocket, int ret, unsigned short degree, short distance_x, short distance_y, pthread_mutex_t* socket_mutex, CAgvLogs *agvLogs)
{
    unsigned char send_buf[32];
    //unsigned short degree = 0;
    //short distance_x = 0;
    //short distance_y = 0;
    unsigned short crc = 0;
    //int ret = -1;
    /*int try_count = 1;
    time_t time_start = time(NULL);
    time_t time_end = time_start;

    while(((time_end-time_start)<15) && ret != 0){
        agvLogs->SaveInfo("get degree, try_count :"+int2string(try_count));
        ret = CommandGetDegree(&degree, &distance, capture, capture_mutex, agvCode, agvLogs);
        try_count++;
        time_end = time(NULL);
    }*/

    //ret = CommandGetDegree(&degree, &distance_x, &distance_y, capture, capture_mutex, agvCode, agvLogs);
    /*send_buf[0] = 0xFE;
    send_buf[1] = 0xFE;
    send_buf[2] = 0x02;//len
    send_buf[3] = 0xE0;
    if(ret == 0)
        send_buf[4] = 0x01;//valid
    else
        send_buf[4] = 0x02;//invalid
    send_buf[5] = 0xEF;
    send_buf[6] = 0xEF;
    agvSocket->SocketWrite(send_buf,7);//ack*/

        send_buf[0] = 0xFE;
        send_buf[1] = 0xFE;
        send_buf[2] = 0x0a;//len
        send_buf[3] = 0xE0;
        if(ret == 0){
            if(degree >= 2700 && degree <= 5400 ){
                send_buf[4] = 0x01;//顺时针
                degree = 5400-degree;
                agvLogs->SaveInfo("get degree sucess, degree is "+int2string(degree));
            }else{
                send_buf[4] = 0x02;//逆时针
                agvLogs->SaveInfo("get degree sucess, degree is -"+int2string(degree));
            }
        }else{
            send_buf[4] = 0x01;
            degree = 0;
            agvLogs->SaveInfo("get degree fail");
        }
        send_buf[5] = (degree & 0xff00)>>8;
        send_buf[6] = (degree & 0x00ff);
        if(distance_x < 0){
            send_buf[7] = 0x01;
            distance_x = -distance_x;
        }else{
            send_buf[7] = 0x02;
        }
        send_buf[8] = (distance_x & 0xff00)>>8;
        send_buf[9] = (distance_x & 0x00ff);
        if(distance_y < 0){
            send_buf[10] = 0x01;
            distance_y = -distance_y;
        }else{
            send_buf[10] = 0x02;
        }
        send_buf[11] = (distance_y & 0xff00)>>8;
        send_buf[12] = (distance_y & 0x00ff);
        send_buf[13] = (crc & 0xff00)>>8;
        send_buf[14] = (crc & 0x00ff);
        send_buf[15] = 0xEF;
        send_buf[16] = 0xEF;
        agvLogs->SaveInfo("response degree");
        pthread_mutex_lock(socket_mutex);
        SendCommand(agvSocket, send_buf, 17);
        pthread_mutex_unlock(socket_mutex);

}

void* HeartBeatThread(void *param)
{
    CAgvSocketClient *agvSocket = ((CAgvCommandProcess*)param)->agvSocket;
    pthread_mutex_t *socket_mutex = &(((CAgvCommandProcess*)param)->socket_mutex);

    while(heart_beat_thread_running){
        unsigned char send_buf[2];
        send_buf[0] = 0x27;
        send_buf[1] = 0x0F;
        pthread_mutex_lock(socket_mutex);
        SendCommand(agvSocket, send_buf, 2);
        pthread_mutex_unlock(socket_mutex);
        sleep(2);//2s
    }

    return NULL;
}
/*
void set_timer()
{
    struct itimerval itv, oldtv;

    itv.it_interval.tv_sec = 5;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 5;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, &oldtv);
}

void sigalrm_handler(int sig)
{
    //count++;
    //printf("timer signal..\n");

}
*/
void* CommandProcessThread(void* param)
{
    CAgvSocketClient *agvSocket = ((CAgvCommandProcess*)param)->agvSocket;
    pthread_mutex_t *socket_mutex = &(((CAgvCommandProcess*)param)->socket_mutex);
    CvCapture *capture = ((CAgvCommandProcess*)param)->capture;
    pthread_mutex_t *capture_mutex = ((CAgvCommandProcess*)param)->capture_mutex;
    CAgvCode *agvCode = ((CAgvCommandProcess*)param)->agvCode;
    CAgvLogs *agvLogs = ((CAgvCommandProcess*)param)->agvLogs;

    int ret = 0;
    unsigned char recv_buf[16];
    unsigned char data_len = 0;
    unsigned char phase = 0;
    unsigned char offset = 0;
    unsigned char com_pro_status = COM_PRO_IDLE;
    int get_degree_ret = 0;
    unsigned short degree = 0;
    short distance_x = 0;
    short distance_y = 0;
    time_t time_heartbeat;

    //signal(SIGALRM, sigalrm_handler);
    //set_timer();
    while(command_process_thread_running)
    {
        if(agvSocket)
        {
            pthread_mutex_lock(socket_mutex);
            if(connect_status == 0){
                if(0 != agvSocket->SocketInit())
                {
                    agvLogs->SaveError("Invoke SocketInit failed");
                    sleep(1);
                }
                else
                {
                    agvSocket->set_send_timeout(3,0);
                    agvSocket->set_recv_timeout(3,0);
                    connect_status = 1;
                    phase = 0;
                    agvLogs->SaveInfo("Invoke SocketInit success");
                }
            }
            if(connect_status == 1){
                if(phase == 0){
                    ret = agvSocket->SocketRead(recv_buf,1);
                    if(ret == 1){
                        if(recv_buf[0] == 0xFE){
                            phase = 1;
                        }else if(recv_buf[0] == 0x27){
                            phase = 5;
                        }
                    }
                }
                if(phase == 1){
                    ret = agvSocket->SocketRead(recv_buf,1);
                    if(ret == 1){
                        if(recv_buf[0] == 0xFE){
                            phase = 2;
                        }else{
                            phase = 0;
                        }
                    }
                }
                if(phase == 2){
                    ret = agvSocket->SocketRead(recv_buf,1);
                    if(ret == 1){
                        data_len = recv_buf[0];
                        if(data_len < 16){
                            phase = 3;
                            offset = 0;
                        }else{
                            phase = 0;
                        }
                    }
                }
                if(phase == 3){
                    ret = agvSocket->SocketRead(recv_buf+offset,data_len-offset);
                    if(ret > 0){
                        offset += ret;
                    }
                    if(offset == data_len){
                        phase = 4;
                    }
                }
                if(phase == 4){
                    if(data_len == 2 && recv_buf[0] == 0xE0 && recv_buf[1] == 0x00){
                        agvLogs->SaveInfo("request degree");
                        //request
                        com_pro_status = COM_PRO_REQUEST;
                    }else if(data_len == 4 && recv_buf[0] == 0xE2 && (recv_buf[1] == 0x01 || recv_buf[1] == 0x02)){
                        //ack

                    }
                    phase = 0;
                }
                if(phase == 5){
                    ret = agvSocket->SocketRead(recv_buf,1);
                    if(ret == 1){
                        if(recv_buf[0] == 0x0F){
                            //heart beat
                            time_heartbeat = time(NULL);
                            agvLogs->SaveInfo("recevie heart beat");
                        }
                        phase = 0;
                    }
                }
            }
            pthread_mutex_unlock(socket_mutex);

            if(com_pro_status == COM_PRO_REQUEST){
                get_degree_ret = CommandGetDegree(&degree, &distance_x, &distance_y, capture, capture_mutex, agvCode, agvLogs);
                com_pro_status = COM_PRO_SEND;
            }
            if(com_pro_status == COM_PRO_SEND){
                CommandRequestDegree(agvSocket, get_degree_ret, degree, distance_x, distance_y, socket_mutex, agvLogs);
                com_pro_status = COM_PRO_IDLE;
            }

            /*if((time(NULL) - time_heartbeat) > 15){
				agvLogs->SaveInfo("heart beat time out");
                pthread_mutex_lock(socket_mutex);
                agvSocket->SocketClose();
                connect_status = 0;
                pthread_mutex_unlock(socket_mutex);
            }*/

            usleep(10000);//10ms
        }
        else
        {
            sleep(3);
        }
    }
    return NULL;
}
