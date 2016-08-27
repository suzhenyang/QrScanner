#include "AgvCode.h"

#include <time.h>
#include <sys/time.h>
#include <Magick++.h>
#include <zbar.h>

#define STR(s) #s

using namespace zbar;
using namespace cv;

CAgvCode::CAgvCode()
{

}

CAgvCode::~CAgvCode()
{

}


int CAgvCode::ReadCode(const char *filename, string *code, vector<cv::Point> &points)
{

    // create a reader
    ImageScanner scanner;

    // configure the reader
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

    // obtain image data
    Magick::Image magick(filename);  // read an image file
    int width = magick.columns();    // extract dimensions
    int height = magick.rows();
    Magick::Blob blob;               // extract the raw data
    magick.modifyImage();
    magick.write(&blob, "GRAY", 8);
    const void *raw = blob.data();

    // wrap image data
    Image image(width, height, "Y800", raw, width * height);

    // scan the image for barcodes
    if(scanner.scan(image) != 1)
    {
        image.set_data(NULL, 0);
        return -1;
    }


    // extract results
    for(Image::SymbolIterator symbol = image.symbol_begin();
        symbol != image.symbol_end();
        ++symbol)
    {
       //do something useful with results
       cout << "decoded:" << symbol->get_type_name() << endl;
       cout << "symbol:" << symbol->get_data() << endl;

       *code  = symbol->get_data();

       for (int i = 0; i < 4; i++)
       {
           Point midPoint;

           midPoint.x = symbol->get_location_x(i);
           midPoint.y = symbol->get_location_y(i);

           points.push_back(midPoint);
       }

    }

    // clean up
    image.set_data(NULL, 0);

    return 0;
}

string int2string(int number);
int CAgvCode::CheckQrCode(const char *filename, CAgvLogs *agvLogs)
{
    int found = 0;

    Mat img=imread(filename, CV_LOAD_IMAGE_COLOR);

    if (NULL == img.data)
    {
        return found;
    }

    Mat img_gray;
    cvtColor(img,img_gray,CV_RGB2GRAY);

    Mat canny_img;
    Canny(img_gray,canny_img,100,200);

    vector<vector<cv::Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(canny_img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    int i;
    int c;
    int k;
    for(i = 0; i < (int)contours.size(); i++){
        k = i;
        c = 0;
        while(hierarchy[k][2] != -1){
            k = hierarchy[k][2];
            c++;
        }

        if(c>=5){
            found++;
        }
    }

    agvLogs->SaveInfo("CheckQrCode, found:"+int2string(found));
    return found>0?1:0;
}
