#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <iostream>
// The key code to be sent.
// A full list of available codes can be found in /usr/include/X11/keysymdef.h
/* g++ -o XFakeKey tst.c -L/usr/X11R6/lib -lX11 -lXtst */
void SendClick(int button, Bool down)
{
    Display *display = XOpenDisplay(NULL);
    XTestFakeButtonEvent(display, button, down, CurrentTime);
    XFlush(display);
    XCloseDisplay(display);
}
char *text = "2";
int main()
{
    // Obtain the X11 display.
    Display *display = XOpenDisplay(0);
    if(display == NULL)
    return -1;

    Window focusWindow;
    int revert;
    XGetInputFocus(display, &focusWindow, &revert);
    KeyCode code = XKeysymToKeycode(display, XStringToKeysym(text));

    XTestFakeKeyEvent(display, code, True, CurrentTime);
    XTestFakeKeyEvent(display, code, False, CurrentTime);

    XTestFakeMotionEvent (display, 0, 250, 250, CurrentTime);
    XCloseDisplay(display);
    SendClick(1, true);
    SendClick(1, false);
    SendClick(5, true);   // press lmb
    SendClick(5, false);  // release lmb
  return 0;
}
