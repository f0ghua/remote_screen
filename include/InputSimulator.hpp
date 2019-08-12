/*
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期日 11/08/2019 12:01.
<<<<<<< HEAD
 * Last Revision: 星期一 12/08/2019 13:04.
=======
 * Last Revision: 星期一 12/08/2019 13:04.
>>>>>>> 24d41a0251b9d9268638dd515316ad5a7ad3a347
 */                     

#ifndef INPUTSIMULATOR_H
#define INPUTSIMULATOR_H

#include <ScreenShot.hpp>
#include <X11/extensions/XTest.h>


class InputSimulator
{
private:
    Display *display;
    Window *focusWindow;

public:
    InputSimulator(){
        display = XOpenDisplay(0);
    }
    void SendClick(int button, Bool down){
        XTestFakeButtonEvent(display, button, down, CurrentTime);
        XFlush(display);
    }

    void MouseMove(int x, int y){
        XTestFakeMotionEvent(display, 0, x, y, CurrentTime);
        XFlush(display);
    }
    void PressLeft(){
        SendClick(1, true);
    }
    void ReleaseLeft(){
        SendClick(1, false);
    }
    void PressRight(){
        SendClick(3, true);
    }
    void ReleaseRight(){
        SendClick(3, false);
    }
    void PressMid(){
        SendClick(2, true);
    }
    void ReleaseMid(){
        SendClick(2, false);
    }
    void WheelUp(){
        SendClick(4, true);
        SendClick(4, false);
    }
    void WheelDown(){
        SendClick(5, true);
        SendClick(5, false);
    }

    void PressKey(int scancode){
        XTestFakeKeyEvent(display, scancode, true, CurrentTime);
        XFlush(display);
    }
    void ReleaseKey(int scancode){
        XTestFakeKeyEvent(display, scancode, false, CurrentTime);
        XFlush(display);
    }
};



#endif   /* End of INPUTSIMULATOR.H */




