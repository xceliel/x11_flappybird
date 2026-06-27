#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BROWN 0x895129
#define BLUE 0x87ceeb
#define GREEN 0x00ee00
#define YELLOW 0xeeee00



typedef struct
{
    XRectangle bottom, top;
} PipeGroup;

int intersectRect(XRectangle a, XRectangle b){
    return a.x + a.width > b.x && 
        a.x < b.x + b.width &&
        a.y < b.y + b.height &&
        a.y + a.height > b.y;
}

int main()
{
    Display *display = XOpenDisplay(NULL);
    Window parent = RootWindow(display, 0);
    double vw, vh;
    XWindowAttributes attrs;
    int score = 0;
    char *scoreBuffer = calloc(50, 1);

    PipeGroup pipeGroups[2];

    XRectangle gap = {
        .width = 50,
        .height = 120,
        .x = 0,
        .y = 100};

    XRectangle ground = {
        .height = 50,
        .width = 300,
        .x = 0,
        .y = 450};

    pipeGroups[0].top = (XRectangle){
        .width = 50,
        .height = 0,
        .y = 0,
        .x = 0};

    pipeGroups[0].bottom = (XRectangle){
        .width = 50,
        .height = 0,
        .y = 0,
        .x = 0};

    pipeGroups[1].top = (XRectangle){
        .width = 50,
        .height = 0,
        .y = 0,
        .x = 100};

    pipeGroups[1].bottom = (XRectangle){
        .width = 50,
        .height = 0,
        .y = 0,
        .x = 100};

    XRectangle player = {
        .width = 50,
        .height = 50,
        .y = 120,
        .x = 120};
    Bool pressed = False;
    Bool suports_detectable_repeat;
    XkbSetDetectableAutoRepeat(display, True, &suports_detectable_repeat);
    Window wi = XCreateSimpleWindow(display, parent, 0, 0, 300, 500, 0, 0, BLUE);
    XGetWindowAttributes(display, wi, &attrs);

    Atom wm_close = XInternAtom(display, "WM_DELETE_WINDOW", False);
    Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS_WINDOW", False);

    GC gc = XCreateGC(display, wi, 0, NULL);

    XStoreName(display, wi, "Flappy Bird");

    XSelectInput(display, wi, ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask);

    XSetWMProtocols(display, wi, &wm_close, 1);
    XMapWindow(display, wi);
    XFlush(display);

    XEvent event;
    XKeyEvent kevent;
    int vy = 0;

    XEvent closeWindow;
    memset(&closeWindow, 0, sizeof(closeWindow));
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

    pipeGroups[1].bottom.x = attrs.width + attrs.width / 2 + pipeGroups[1].bottom.width/2;
    pipeGroups[1].top.x = attrs.width + attrs.width / 2 + pipeGroups[1].top.width/2;
    while (1)
    {
        int i;
        for (i = 0; i < 2; i++)
        {
            if(
                intersectRect(player, pipeGroups[i].top) || 
                intersectRect(player, pipeGroups[i].bottom) || 
                intersectRect(player, ground)
            ){
                XSendEvent(display, wi, True, StructureNotifyMask, &closeWindow);
                XFlush(display);
            }
        }

        if (XPending(display) > 0)
        {
            XNextEvent(display, &event);

            if (event.type == ClientMessage && event.xclient.data.l[0] == wm_close)
            {
                break;
            }

            // if(event.type == ButtonPress){
            //     printf("Jump\n");
            //     KeySym key = XKeycodeToKeysym(display, event.xkey.keycode, 0);
            //     if(key == XK_space || key == XK_Up || key == XK_w || key == XK_W){
            //         printf("Jump\n");
            //         pressed = True;
            //     }
            // }

            char keys[32];

            XQueryKeymap(display, keys);

            int index = event.xkey.keycode / 8;
            int mask = 1 << (event.xkey.keycode % 8);

            if (event.type == KeyPress)
            {


                KeySym key = XKeycodeToKeysym(display, event.xkey.keycode, 0);

                if ((key == XK_space || key == XK_Up || key == XK_w || key == XK_W))
                {
                    if(!pressed){
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
                    if(!(keys[index] & mask))
                        pressed = False;
                }
            }
        }


        // if (event.type == Expose || event.type == Button1)
        // {
     
        XClearWindow(display, wi);

        // pipes
        XSetForeground(display, gc, GREEN);

        for (i = 0; i < 2; i++)
        {
            pipeGroups[i].top.x -= 1;
            pipeGroups[i].bottom.x -= 1;

            if (pipeGroups[i].top.x < pipeGroups[i].bottom.width * -1)
            {
                pipeGroups[i].bottom.x = attrs.width;
                pipeGroups[i].top.x = attrs.width;
            }

            pipeGroups[i].top.height = gap.y;
            pipeGroups[i].bottom.y = gap.y + gap.height;
            pipeGroups[i].bottom.height = attrs.height - pipeGroups[i].bottom.y - ground.height;

            XFillRectangle(display, wi, gc, pipeGroups[i].top.x, pipeGroups[i].top.y, pipeGroups[i].top.width, pipeGroups[i].top.height);
            XFillRectangle(display, wi, gc, pipeGroups[i].bottom.x, pipeGroups[i].bottom.y, pipeGroups[i].bottom.width, pipeGroups[i].bottom.height);
        }
        XFlush(display);

        // ground
        XSetForeground(display, gc, BROWN);
        XFillRectangle(display, wi, gc, ground.x, ground.y, ground.width, ground.height);
        XFlush(display);

        // player
        XSetForeground(display, gc, YELLOW);
        XFillRectangle(display, wi, gc, player.x, player.y, player.width, player.height);
        XFlush(display);

        //score
        XFontStruct *font = XLoadQueryFont(display, "-*-fixed-*-*-*-*-20-*-*-*-*-*-*-*");
        if (!font) font = XLoadQueryFont(display, "fixed");
        sprintf(scoreBuffer, "%d", score);
        int width_text = XTextWidth(font, scoreBuffer, strlen(scoreBuffer));

        XSetForeground(display, gc, 0x000000);
        XSetFont(display, gc, font->fid);
        XDrawString(display, wi, gc, attrs.width/2 - width_text/2, 50, scoreBuffer, strlen(scoreBuffer));
        XFlush(display);


        vy += 1;
        player.y+=vy;
        // }

        usleep(30000);
    }

    XUnmapWindow(display, wi);
    XDestroyWindow(display, wi);
    XCloseDisplay(display);

    return 0;
}