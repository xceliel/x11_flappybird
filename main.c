#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(){
    Display *display = XOpenDisplay(NULL);
    Window parent = RootWindow(display, 0);

    Window wi = XCreateSimpleWindow(display, parent, 0, 0, 300, 500, 0, 0, 0x87ceeb);

    Atom wm_close = XInternAtom(display, "WM_DELETE_WINDOW", False);

    GC gc = XCreateGC(display, wi, 0, NULL);
    XStoreName(display, wi, "Flappy Bird");

    XSelectInput(display, wi, ExposureMask);
    XSetWMProtocols(display, wi, &wm_close, 1);
    XMapWindow(display, wi);
    XFlush(display);

    XEvent event;

    while(1){
        XNextEvent(display, &event);
        if(event.type == ClientMessage && event.xclient.data.l[0] == wm_close){
            break;
        }

        if(event.type == Expose) {


            


            // pipe middle shadow
            XSetForeground(display, gc, 0x00cc00);
            XFillRectangle(display, wi, gc, 150-40, 200, 80, 300);
            XFlush(display);

            // pipe top shadow
            XSetForeground(display, gc, 0x00cc00);
            XFillRectangle(display, wi, gc, 150-55, 145, 110, 60);
            XFlush(display);

            // pipe top
            XSetForeground(display, gc, 0x00ee00);
            XFillRectangle(display, wi, gc, 150-50, 150, 100, 50);
            XFlush(display);


            // pipe middle
            XSetForeground(display, gc, 0x00ee00);
            XFillRectangle(display, wi, gc, 150-35, 205, 70, 295);
            XFlush(display);
            


            // floor

            XSetForeground(display, gc, 0x864826);
            XFillRectangle(display, wi, gc, 0, 500-52, 300, 2);
            XFlush(display);

            XSetForeground(display, gc, 0x895129);
            XFillRectangle(display, wi, gc, 0, 500-50, 300, 50);
            XFlush(display);
        }
    }

    XUnmapWindow(display, wi);
    XDestroyWindow(display, wi);
    XCloseDisplay(display);

    return 0;
}