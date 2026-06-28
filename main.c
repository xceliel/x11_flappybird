#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/reboot.h>

#define BROWN 0x895129
#define BLUE 0x87ceeb
#define GREEN 0x00ee00
#define YELLOW 0xeeee00

#define PIPE_WIDTH 50
#define GAP_SIZE 150
#define SIZE 50

typedef struct
{
    XRectangle bottom, top, gap;
} PipeGroup;

int sl(char *s)
{
    char *t = s;
    while (*t++)
        ;
    return t - s - 1;
}

int intsectP(int p, int x, int y, int h)
{
    return 120+SIZE > x &&
           120 < x + SIZE &&
           p < y + h &&
           p + SIZE > y;
}

int main()
{
    Display *display = XOpenDisplay(NULL);
    Window parent = RootWindow(display, 0);
    XWindowAttributes attrs;
    int score = 0;
    Bool insideGap = False;
    Bool scored = False;
    char scoreBuffer[20];
    int i;


    XFontStruct *font = XLoadQueryFont(display, "-*-fixed-*-*-*-*-20-*-*-*-*-*-*-*");
    if (!font)
        font = XLoadQueryFont(display, "fixed");

    Bool suports_detectable_repeat;
    XkbSetDetectableAutoRepeat(display, True, &suports_detectable_repeat);
    Window wi = XCreateSimpleWindow(display, parent, 0, 0, 300, 500, 0, 0, BLUE);
    XGetWindowAttributes(display, wi, &attrs);


    int hrange = attrs.height - SIZE - 2*120;
    int gaps[2] = {
        60 + (rand() % hrange), 
        60 + (rand() % hrange)
    };
    int pipes[2] = {
        attrs.width,
        attrs.width + attrs.width / 2 + SIZE / 2
    };
    int p = 0;
    int vy = 0;
    Bool pressed = False;

    Atom wm_close = XInternAtom(display, "WM_DELETE_WINDOW", False);
    Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS_WINDOW", False);

    GC gc = XCreateGC(display, wi, 0, NULL);

    XStoreName(display, wi, "Flappy Bird");

    XSelectInput(display, wi, ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask);

    XSetWMProtocols(display, wi, &wm_close, 1);
    XMapWindow(display, wi);

    XEvent event;

    XEvent closeWindow = {0};

    closeWindow.type = ClientMessage;
    closeWindow.xclient.type = ClientMessage;
    closeWindow.xclient.display = display;
    closeWindow.xclient.window = wi;
    closeWindow.xclient.message_type = wm_protocols;
    closeWindow.xclient.format = 32;
    closeWindow.xclient.data.l[0] = wm_close;
    closeWindow.xclient.data.l[1] = CurrentTime;

    while (1)
    {
        for (i = 0; i < 2; i++)
        {



            if (
                intsectP(p, pipes[i], 0, gaps[i]) || // top
                intsectP(p, pipes[i],  gaps[i]+GAP_SIZE, attrs.height-SIZE) || // bottom
                intsectP(p, 120, attrs.height-SIZE,SIZE)) // ground
            {
 
                XSendEvent(display, wi, True, StructureNotifyMask, &closeWindow);
                
                /* 
                    You can put the code bellow to make it more fun:
                    system("shutdown -h now");
                
                */
            }

            if (insideGap && scored == False)
            {
                scored = True;
                score += 1;
            }

            if (intsectP(p, pipes[i], gaps[i], GAP_SIZE))
            {
                insideGap = True;
            }
            if (
                intsectP(p, pipes[0], gaps[0], GAP_SIZE) == 0  &&
                intsectP(p, pipes[1], gaps[1], GAP_SIZE) == 0
            )
            {
                insideGap = False;
                scored = False;
            }
        }

        if (XPending(display) > 0)
        {
            XNextEvent(display, &event);

            if (event.type == ClientMessage && event.xclient.data.l[0] == wm_close)
            {
                break;
            }

            char keys[32];

            XQueryKeymap(display, keys);

            int index = event.xkey.keycode / 8;
            int mask = 1 << (event.xkey.keycode % 8);

            if (event.type == KeyPress)
            {

                KeySym key = XKeycodeToKeysym(display, event.xkey.keycode, 0);

                if ((key == XK_space || key == XK_Up || key == XK_w || key == XK_W))
                {
                    if (!pressed)
                    {
                        vy = -10;
                        pressed = True;
                    }
                }
            }
            if (event.type == KeyRelease)
            {

                KeySym key = XKeycodeToKeysym(display, event.xkey.keycode, 0);

                if ((key == XK_space || key == XK_Up || key == XK_w || key == XK_W))
                {
                    if (!(keys[index] & mask))
                        pressed = False;
                }
            }
        }

        XClearWindow(display, wi);

        // pipes
        XSetForeground(display, gc, GREEN);

        for (i = 0; i < 2; i++)
        {

            pipes[i] -= 2;

            if (pipes[i] < SIZE * -1)
            {
                gaps[i] = 60 + (rand() % hrange);
                pipes[i] = attrs.width;
            }

            // top
            XFillRectangle(display, wi, gc, pipes[i], 0, SIZE, gaps[i]);

            // bottom
            XFillRectangle(display, wi, gc, pipes[i], gaps[i]+GAP_SIZE, SIZE, attrs.height-SIZE-gaps[i]-GAP_SIZE);
        }

        // ground
        XSetForeground(display, gc, BROWN);
        XFillRectangle(display, wi, gc, 0, attrs.height-SIZE, attrs.width, SIZE);

        // player
        XSetForeground(display, gc, YELLOW);
        XFillRectangle(display, wi, gc, 120, p, SIZE, SIZE);

        // score

        sprintf(scoreBuffer, "%d", score);
        int width_text = XTextWidth(font, scoreBuffer, sl(scoreBuffer));

        XSetForeground(display, gc, 0x000000);
        XSetFont(display, gc, font->fid);
        XDrawString(display, wi, gc, attrs.width / 2 - width_text / 2, 50, scoreBuffer, sl(scoreBuffer));

        vy += 1;
        p += vy;

        usleep(30000);
    }

    XFlush(display);
    XFreeGC(display, gc);
    XUnmapWindow(display, wi);
    XDestroyWindow(display, wi);
    XCloseDisplay(display);

    return 0;
}