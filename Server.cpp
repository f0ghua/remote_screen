/*
 * File Type:     C/C++
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期四 08/08/2019 11:04.
 * Last Revision: 星期五 09/08/2019 12:06.
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
int default_fps = 10;


int main(){
    int WIDTH = 1920;
    int HEIGHT = 1080;
    ScreenShot screen(0, 0, 1920, 1080);
    cv::Mat *cur = new cv::Mat(1080, 1920, CV_8UC4);
    cv::Mat cur_gray;

    Compresser compresser(HEIGHT, WIDTH, PARTS_MOTION_JPEG);
    uchar* data = new uchar[HEIGHT*WIDTH*4];
    int size;

    TCPManager sender;
    sender.ConnectToServer();
    if(! sender.isConnected()) return 0;

    for(uint i=0;i>=0; ++i){
        double start = cv::getTickCount();

        screen(cur);
        cv::cvtColor(*cur, cur_gray, CV_BGRA2BGR);
        cv::resize(cur_gray, cur_gray, cv::Size(WIDTH, HEIGHT));
        compresser.compress(data, &size, cur_gray.data);
        sender.SendBuffer(data, size);

        int sz = size;
        for(int i=0;i<sz;i+=10000){
            cout<<"0";
        }
        double net_rate = sz*default_fps*1.0/1024/1024;
        cout<<"\t\t\t"<<net_rate<<"  MB/S"<<endl;
        if(net_rate < 0.3) default_fps += 5;
        else if(net_rate > 1.5) default_fps -= 5;
        default_fps = max(10, min(30, default_fps));
        cout<<default_fps<<endl;

        start = cv::getTickCount() - start;
        start = start / cv::getTickFrequency();
        double iter_time = start;
        if(iter_time < 1000./default_fps)
            usleep((1000./default_fps - iter_time)*1000);
        //printf("fps %4.f  spf %.4f ms\n", 1./start, start*1000);
    }

    return 0;
}





/* EOF */







/* EOF */

