#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
           p + h > y;
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
    srand(clock());
    PipeGroup pipeGroups[2];
    int i;


    XFontStruct *font = XLoadQueryFont(display, "-*-fixed-*-*-*-*-20-*-*-*-*-*-*-*");
    if (!font)
        font = XLoadQueryFont(display, "fixed");

    Bool suports_detectable_repeat;
    XkbSetDetectableAutoRepeat(display, True, &suports_detectable_repeat);
    Window wi = XCreateSimpleWindow(display, parent, 0, 0, 300, 500, 0, 0, BLUE);
    XGetWindowAttributes(display, wi, &attrs);

    XRectangle ground = {
        .height = 50,
        .width = 300,
        .x = 0,
        .y = 450
    };

    int hrange = attrs.height - SIZE - 2*120;
    int gaps[2] = {
        60 + (rand() % hrange), 
        60 + (rand() % hrange)
    };
    int pipes[2] = {
        attrs.width,
        0
    };
    int p = 0;
    int vy = 0;

    pipeGroups[0].gap = (XRectangle){
        .width = 1,
        .height = 120,
        .x = 0,
        .y = 60 + (rand() % hrange)
    };

    pipeGroups[1].gap = (XRectangle){
        .width = 1,
        .height = 120,
        .x = 0,
        .y = 60 + (rand() % hrange)
    };


    pipeGroups[0].top = (XRectangle){
        .width = 50,
        .height = pipeGroups[0].gap.y,
        .y = 0,
        .x = 0
    };

    pipeGroups[0].bottom = (XRectangle){
        .width = 50,
        .height = attrs.height - (pipeGroups[0].gap.y + pipeGroups[0].gap.height) - ground.height,
        .y = pipeGroups[0].gap.y + pipeGroups[0].gap.height,
        .x = 0
    };


    pipeGroups[1].top = (XRectangle){
        .width = 50,
        .height = pipeGroups[1].gap.y,
        .y = 0,
        .x = 100
    };


    pipeGroups[1].bottom = (XRectangle){
        .width = 50,
        .height = attrs.height - (pipeGroups[0].gap.y + pipeGroups[0].gap.height) - ground.height,
        .y = pipeGroups[0].gap.y + pipeGroups[0].gap.height,
        .x = 100
    };

    XRectangle player = {
        .width = 50,
        .height = 50,
        .y = 120,
        .x = 120};
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

    pipeGroups[0].bottom.x = attrs.width;
    pipeGroups[0].top.x = attrs.width;
    pipeGroups[0].gap.x = attrs.width;

    pipeGroups[1].bottom.x = attrs.width + attrs.width / 2 + pipeGroups[1].bottom.width / 2;
    pipeGroups[1].top.x = attrs.width + attrs.width / 2 + pipeGroups[1].top.width / 2;
    pipeGroups[1].gap.x = attrs.width + attrs.width / 2 + pipeGroups[1].top.width / 2;

    for (i = 0; i < 2; i++)
    {
        pipeGroups[i].top.x -= 2;
        pipeGroups[i].bottom.x -= 2;
        pipeGroups[i].gap.x -= 2;

        if (pipeGroups[i].top.x < pipeGroups[i].bottom.width * -1)
        {
            pipeGroups[i].gap.y = 60 + (rand() % hrange);
            pipeGroups[i].gap.x = attrs.width;
            pipeGroups[i].bottom.x = attrs.width;
            pipeGroups[i].top.x = attrs.width;
        }


        pipeGroups[i].top.height = pipeGroups[i].gap.y;
        pipeGroups[i].bottom.y = pipeGroups[i].gap.y + pipeGroups[i].gap.height;
        pipeGroups[i].bottom.height = attrs.height - (pipeGroups[i].gap.y + pipeGroups[i].gap.height) - ground.height;

        XFillRectangle(display, wi, gc, pipeGroups[i].top.x, pipeGroups[i].top.y, pipeGroups[i].top.width, pipeGroups[i].top.height);
        XFillRectangle(display, wi, gc, pipeGroups[i].bottom.x, pipeGroups[i].bottom.y, pipeGroups[i].bottom.width, pipeGroups[i].bottom.height);
    }

    while (1)
    {
        for (i = 0; i < 2; i++)
        {
            if (
                intsectP(p, pipes[i], 0, gaps[i]) ||
                intsectP(p, pipes[i],  gaps[i]+GAP_SIZE, attrs.height-SIZE-gaps[i]-GAP_SIZE) ||
                intsectP(p, 120, attrs.height-SIZE,SIZE))
            {
                XSendEvent(display, wi, True, StructureNotifyMask, &closeWindow);
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
                intsectP(p, pipes[0], gaps[0], GAP_SIZE) &&
                intsectP(p, pipes[1], gaps[1], GAP_SIZE)
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
            pipeGroups[i].top.x -= 2;
            pipeGroups[i].bottom.x -= 2;
            pipeGroups[i].gap.x -= 2;

            if (pipeGroups[i].top.x < pipeGroups[i].bottom.width * -1)
            {
                pipeGroups[i].gap.y = 60 + (rand() % hrange);
                pipeGroups[i].gap.x = attrs.width;
                pipeGroups[i].bottom.x = attrs.width;
                pipeGroups[i].top.x = attrs.width;
            }

            pipeGroups[i].top.height = pipeGroups[i].gap.y;
            pipeGroups[i].bottom.y = pipeGroups[i].gap.y + pipeGroups[i].gap.height;
            pipeGroups[i].bottom.height = attrs.height - pipeGroups[i].bottom.y - ground.height;

            XFillRectangle(display, wi, gc, pipeGroups[i].top.x, pipeGroups[i].top.y, pipeGroups[i].top.width, pipeGroups[i].top.height);
            XFillRectangle(display, wi, gc, pipeGroups[i].bottom.x, pipeGroups[i].bottom.y, pipeGroups[i].bottom.width, pipeGroups[i].bottom.height);
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