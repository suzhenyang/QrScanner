#ifndef AGVCOMMANDPROCESS_H
#define AGVCOMMANDPROCESS_H

#include "AgvSocketClient.h"
#include "AgvCode.h"
#include "AgvLogs.h"
#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/core/core_c.h>
//#include <opencv2/highgui/highgui_c.h>
//#include <opencv2/imgproc/imgproc_c.h>
enum{
    COM_PRO_IDLE,
    COM_PRO_REQUEST,
    COM_PRO_SEND
};
class CAgvCommandProcess
{
public:
    CAgvCommandProcess();
    ~CAgvCommandProcess();

public:
    void SetCvCapture(CvCapture* capture, pthread_mutex_t *capture_mutex);
    void SetAgvCode(CAgvCode *agvCode);
    void SetAgvLogs(CAgvLogs *agvLogs);
    void Start(void);

public:
   CAgvSocketClient *agvSocket;
   CvCapture* capture;
   pthread_t  command_process_thread;
   pthread_t  heart_beat_thread;
   pthread_mutex_t socket_mutex;
   pthread_mutex_t *capture_mutex;
   CAgvCode *agvCode;
   CAgvLogs *agvLogs;
};

void* CommandProcessThread(void* param);
void* HeartBeatThread(void *param);

#endif // AGVCOMMANDPROCESS_H
