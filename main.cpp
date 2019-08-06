/*
 * File Type:     C/C++
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期四 25/07/2019 18:39.
 * Last Revision: 星期二 06/08/2019 14:46.
 */
#include <iostream>
#include <opencv2/opencv.hpp>  // This includes most headers!
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <termio.h>
#include <stdio.h>
#include <vector>

#include <time.h>
#define FPS(start) (CLOCKS_PER_SEC / (clock()-start))


using std::cout;
using std::endl;
const int channels = 3;

struct ScreenShot{
    ScreenShot(uint x, uint y, uint width, uint height):
               x(x), y(y), width(width), height(height){

        display = XOpenDisplay(nullptr);
        root = DefaultRootWindow(display);

        XGetWindowAttributes(display, root, &window_attributes);
        screen = window_attributes.screen;
        ximg = XShmCreateImage(display, DefaultVisualOfScreen(screen), DefaultDepthOfScreen(screen), ZPixmap, NULL, &shminfo, width, height);

        shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT|0777);
        shminfo.shmaddr = ximg->data = (char*)shmat(shminfo.shmid, 0, 0);
        shminfo.readOnly = False;
        if(shminfo.shmid < 0)
            puts("Fatal shminfo error!");;
        Status s1 = XShmAttach(display, &shminfo);
        printf("XShmAttach() %s\n", s1 ? "success!" : "failure!");

        init = true;
    }

    void operator() (cv::Mat* cv_img){
        if(init)
            init = false;

        XShmGetImage(display, root, ximg, 0, 0, 0x00ffffff);
        memcpy(cv_img->data, ximg->data, height*width*4);
        //cv_img = cv::Mat(height, width, CV_8UC4, ximg->data);
    }

    ~ScreenShot(){
        if(!init)
            XDestroyImage(ximg);

        XShmDetach(display, &shminfo);
        shmdt(shminfo.shmaddr);
        XCloseDisplay(display);
    }

    Display* display;
    Window root;
    XWindowAttributes window_attributes;
    Screen* screen;
    XImage* ximg;
    XShmSegmentInfo shminfo;

    int x, y, width, height;

    bool init;
};

long calc_dif_pix(cv::Mat* last, cv::Mat* cur){
    int width = last->cols;
    int height = last->rows;
    long ret = 0;
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++){
            bool same = true;
            for(int c=0;c<3;c++){
                if(last->data[i*width*4+j*4+c] != cur->data[i*width*4+j*4+c]){
                    same = false;
                    break;
                }
            }
            if(!same) ret++;
        }

    return ret;
}

void calc_dif_mat(cv::Mat* diff, cv::Mat* last, cv::Mat* cur){
    int width = last->cols;
    int height = last->rows;
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++){
            for(int c=0;c<3;c++){
                diff->data[i*width*4+j*4+c] = cur->data[i*width*4+j*4+c] - last->data[i*width*4+j*4+c];
            }
        }

}

int main(){
    int WIDTH = 1920;
    int HEIGHT = 1080;
    ScreenShot screen(0, 0, WIDTH, HEIGHT);
    cv::Mat *last = new cv::Mat(HEIGHT, WIDTH, CV_8UC4);
    cv::Mat *cur = new cv::Mat(HEIGHT, WIDTH, CV_8UC4);
    cv::Mat *diff = new cv::Mat(HEIGHT, WIDTH, CV_8UC4);
    screen(last);

    for(uint i=0;i<1000000; ++i){
        double start = cv::getTickCount();

        screen(cur);
        //printf("%ld\n", calc_dif_pix(last, cur));
        calc_dif_mat(diff, last, cur);
        swap(cur, last);

        std::vector<uchar> data;
        std::vector<int> params;
        params.push_back(cv::IMWRITE_JPEG_QUALITY);
        params.push_back(80);
        cv::imencode(".jpg", *diff, data, params);
        cout<<data.size()<<endl;

        start = cv::getTickCount() - start;
        start = start / cv::getTickFrequency();
        printf("fps %4.f  spf %.4f ms\n", 1./start, start*1000);
        cv::Mat gray;
        cv::cvtColor(*last, gray, CV_BGRA2GRAY);
        cv::imshow("img", gray);
        char q = cv::waitKey(1);
        if(q == 'q') break;
    }

    return 0;
}





/* EOF */

