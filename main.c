#include <X11/Xlib.h>
#include <X11/Xutil.h>

void drawFloor(Display *display, Window wi, GC gc, double vw, double vh)
{
   
    XSetForeground(display, gc, 0x895129);
    XFillRectangle(display, wi, gc, 0, vh * 90, vw * 100, vh * 11);
    XFlush(display);

    XSetForeground(display, gc, 0x864826);
    XFillRectangle(display, wi, gc, 0, vh * 90, vw * 100, vh * 1);
    XFlush(display);
}

void drawBottomPipe(Display *display, Window wi, GC gc, int x, int y,  double vw, double vh)
{



    XSetForeground(display, gc, 0x00cc00);
    XFillRectangle(display, wi, gc, x+2*vw, y+vh*8, vw*16, vh*60);
    XFlush(display);

    XSetForeground(display, gc, 0x00ee00);
    XFillRectangle(display, wi, gc, x+3*vw, y+vh*8, vw*14, vh*60);
    XFlush(display);

    
    // top part

    XSetForeground(display, gc, 0x00cc00);
    XFillRectangle(display, wi, gc, x, y, vw*20, vh*8);
    XFlush(display);

    XSetForeground(display, gc, 0x00ee00);
    XFillRectangle(display, wi, gc, x+1*vw, y+0.5*vh, vw*20 - 2*vw, vh*7);
    XFlush(display);
}

void drawTopPipe(Display *display, Window wi, GC gc, int x, int y,  double vw, double vh)
{



    XSetForeground(display, gc, 0x00cc00);
    XFillRectangle(display, wi, gc, x+2*vw, y+vh*8, vw*16, vh*60);
    XFlush(display);

    XSetForeground(display, gc, 0x00ee00);
    XFillRectangle(display, wi, gc, x+3*vw, y+vh*8, vw*14, vh*60);
    XFlush(display);

    
    // top part

    XSetForeground(display, gc, 0x00cc00);
    XFillRectangle(display, wi, gc, x, y, vw*20, vh*8);
    XFlush(display);

    XSetForeground(display, gc, 0x00ee00);
    XFillRectangle(display, wi, gc, x+1*vw, y+0.5*vh, vw*20 - 2*vw, vh*7);
    XFlush(display);
}

int main()
{
    Display *display = XOpenDisplay(NULL);
    Window parent = RootWindow(display, 0);
    double vw, vh;
    XWindowAttributes attrs;
    Window wi = XCreateSimpleWindow(display, parent, 0, 0, 300, 500, 0, 0, 0x87ceeb);

    Atom wm_close = XInternAtom(display, "WM_DELETE_WINDOW", False);

    GC gc = XCreateGC(display, wi, 0, NULL);
    XStoreName(display, wi, "Flappy Bird");

    XSelectInput(display, wi, ExposureMask | StructureNotifyMask  );
    XSetWMProtocols(display, wi, &wm_close, 1);
    XMapWindow(display, wi);
    XFlush(display);

    XEvent event;

    while (1)
    {
        XNextEvent(display, &event);
        if (event.type == ClientMessage && event.xclient.data.l[0] == wm_close)
        {
            break;
        }

        if (event.type == Expose || event.type == ConfigureNotify)
        {
            XGetWindowAttributes(display, wi, &attrs);
            vw = (double)attrs.width / 100;
            vh = (double)attrs.height / 100;
            
            drawBottomPipe(display, wi, gc, 0, 50*vh, vw, vh);

            drawFloor(display, wi, gc, vw, vh);
        }
    }

    XUnmapWindow(display, wi);
    XDestroyWindow(display, wi);
    XCloseDisplay(display);

    return 0;
}