/*
 * File Type:     C/C++
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期四 25/07/2019 18:39.
 * Last Revision: 星期四 08/08/2019 12:08.
 */
#include <iostream>
#include <opencv2/opencv.hpp>  // This includes most headers!
#include <vector>
#include <Compresser.hpp>
#include <ScreenShot.hpp>
#include <TCPManager.hpp>

#include <time.h>
#define FPS(start) (CLOCKS_PER_SEC / (clock()-start))


using std::cout;
using std::endl;
const int channels = 3;
const int default_fps = 10;


int main(){
    int WIDTH = 1280;
    int HEIGHT = 720;
    ScreenShot screen(0, 0, 1920, 1080);
    cv::Mat *cur = new cv::Mat(1080, 1920, CV_8UC4);
    cv::Mat cur_gray;

    Compresser compresser(HEIGHT, WIDTH, PARTS_MOTION_JPEG);
    Decompresser decompresser(HEIGHT, WIDTH, PARTS_MOTION_JPEG);
    uchar* data = new uchar[HEIGHT*WIDTH*4];
    int size;

    TCPManager sender;
    sender.ConnectToServer();

    for(uint i=0;i>=0; ++i){
        double start = cv::getTickCount();

        screen(cur);
        cv::cvtColor(*cur, cur_gray, CV_BGRA2BGR);
        cv::resize(cur_gray, cur_gray, cv::Size(WIDTH, HEIGHT));
        compresser.compress(data, &size, cur_gray.data);
        decompresser.decompress(data, data);
        memcpy(cur_gray.data, data, WIDTH*HEIGHT*3);

        int sz = size;
        for(int i=0;i<sz;i+=10000){
            cout<<"0";
        }
        cout<<"\t\t\t"<<sz*default_fps*1.0/1024/1024<<"  MB/S"<<endl;

        start = cv::getTickCount() - start;
        start = start / cv::getTickFrequency();
        //printf("fps %4.f  spf %.4f ms\n", 1./start, start*1000);
        cv::resize(cur_gray, cur_gray, cv::Size(1920, 1080));
        cv::imshow("img", cur_gray);
        char q = cv::waitKey(1000/default_fps);
        if(q == 27) break;
    }

    return 0;
}





/* EOF */

