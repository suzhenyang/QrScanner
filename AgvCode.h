#ifndef AGVCODE_H
#define AGVCODE_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include "AgvLogs.h"
using namespace std;

class CAgvCode
{
public:
    CAgvCode();
    ~CAgvCode();

public:
    int ReadCode(const char *filename, string *code, vector<cv::Point> &points);
    int CheckQrCode(const char *filename, CAgvLogs *agvLogs);

private:
    string m_code;
};

#endif // AGVCODE_H
