/*
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期四 08/08/2019 10:04.
 * Last Revision: 星期四 08/08/2019 10:04.
 */                     

#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <termio.h>
#include <stdio.h>


struct ScreenShot{
    ScreenShot(uint x, uint y, uint width, uint height):
               x(x), y(y), width(width), height(height){

        display = XOpenDisplay(nullptr);
        int count_screens = ScreenCount(display);
        root = DefaultRootWindow(display);
        //cout<<"width: "<<ScreenOfDisplay(display, 0)->width<<endl;
        //cout<<"height: "<<ScreenOfDisplay(display, 0)->height<<endl;

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



#endif   /* End of SCREENSHOT.H */




