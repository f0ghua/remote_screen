/*
 * File Type:     C/C++
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期四 08/08/2019 11:04.
 * Last Revision: 星期五 09/08/2019 12:06.
 */

#include <iostream>
#include <opencv2/opencv.hpp>  // This includes most headers!
#include <vector>
#include <thread>
#include <Compresser.hpp>
#include <ScreenShot.hpp>
#include <TCPManager.hpp>
#include <InputSimulator.hpp>

#include <time.h>
#define FPS(start) (CLOCKS_PER_SEC / (clock()-start))


using std::cout;
using std::endl;
int default_fps = 10;

int WIDTH = 1920;
int HEIGHT = 1080;

Compresser compresser(HEIGHT, WIDTH, PARTS_MOTION_JPEG);
TCPManager tcp;
InputSimulator simulator;

void recv_input_fun(){
    while(1){
        long long code = tcp.Recv_Input();
        unsigned char* p = (unsigned char*)&code;
        unsigned char t;
        t = p[0];
        if((t>>7)&1){
            // KeyBoard Input
            if((t>>6)&1){
                //Release
                simulator.ReleaseKey(p[1]);
            }
            else {
                //Press
                simulator.PressKey(p[1]);
            }
        }
        else{
            // Mouse Input
            if((t>>6)&1){
                //Mouse Move
                unsigned short x, y;
                x = *(unsigned short*)(p+1);
                y = *(unsigned short*)(p+3);
                simulator.MouseMove(x, y);
            }
            else{
                // Mouse Button
                t = t%8;
                switch(t){
                    case 0:simulator.PressLeft();break;
                    case 1:simulator.ReleaseLeft();break;
                    case 2:simulator.PressRight();break;
                    case 3:simulator.ReleaseRight();break;
                    case 4:simulator.PressMid();break;
                    case 5:simulator.ReleaseMid();break;
                    case 6:simulator.WheelUp();break;
                    case 7:simulator.WheelDown();break;
                }
            }
        }
    }
}

int main(){
    ScreenShot screen(0, 0, 1920, 1080);
    cv::Mat *cur = new cv::Mat(1080, 1920, CV_8UC4);
    cv::Mat cur_gray;

    uchar* data = new uchar[HEIGHT*WIDTH*4];
    int size;

    tcp.ConnectToServer();
    if(! tcp.isConnected()) {
        cout<<"Connecting to Server error."<<endl;
        return 0;
    }

    std::thread* th = new std::thread(recv_input_fun);
    th->detach();

    for(uint i=0;i>=0; ++i){
        double start = cv::getTickCount();

        screen(cur);
        cv::cvtColor(*cur, cur_gray, CV_BGRA2BGR);
        cv::resize(cur_gray, cur_gray, cv::Size(WIDTH, HEIGHT));
        compresser.compress(data, &size, cur_gray.data);
        tcp.SendBuffer(data, size);

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
    }

    delete th;

    return 0;
}





/* EOF */







/* EOF */

