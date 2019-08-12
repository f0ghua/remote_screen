/*
 * File Type:     C/C++
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期四 08/08/2019 11:28.
 * Last Revision: 星期六 10/08/2019 17:43.
 */
#include <GUIManager.hpp>
#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>  // This includes most headers!
#include <vector>
#include <mutex>
#include <queue>
#include <Compresser.hpp>
#include <ScreenShot.hpp>
#include <TCPManager.hpp>

#include <time.h>
#define FPS(start) (CLOCKS_PER_SEC / (clock()-start))


using std::cout;
using std::endl;

const int WIDTH = 1920;
const int HEIGHT = 1080;

GUIManager gui;
extern TCPManager tcp;
uchar* buffer;
bool need_update = false;

void recv_image_fun(){
    Decompresser decompresser(HEIGHT, WIDTH, PARTS_MOTION_JPEG);
    uchar* data = new uchar[HEIGHT*WIDTH*3];
    unsigned int size;
    while(1){
        tcp.RecvBuffer(data, &size);
        decompresser.decompress(data, data);
        while(need_update) usleep(1000);
        memcpy(buffer, data, HEIGHT*WIDTH*3);
        need_update = true;
    }
}

int main(){
    buffer = new uchar[HEIGHT*WIDTH*3];
    tcp.ConnectToServer();
    if(!tcp.isConnected()){
        cout<<"Connecting to server error."<<endl;
        return -1;
    }

    if(!gui.CreateWindow(HEIGHT, WIDTH, "Client")){
        cout<<"Create Window error."<<endl;
        return -1;
    }

    std::thread *th = new std::thread(recv_image_fun);
    th->detach();

    while(!gui.shouldclose()){
        if(need_update){
            gui.updateFrameData(buffer, HEIGHT, WIDTH);
            need_update = false;
        }
        gui.Draw();
    }

    delete th;
    delete buffer;
    return 0;
}







/* EOF */

