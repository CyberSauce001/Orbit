//Programmer: Alexander Nguyen
//Professor: Gordon Griesel
//Course: CMPS 4490 - Game Design
//Spring 2018


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/Xdbe.h>
#include <ctime>
#include <cstring>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <vector>
#include "fonts.h"
typedef float Flt;
typedef float Vec[3];
const int N_CIRCLE_SEGMENTS = 24;

//
#define PI 3.14159265358979
#define SWAP(x,y) (x)^=(y);(y)^=(x);(x)^=(y)
#define rnd() (Flt)rand() / (Flt)RAND_MAX
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
                        (c)[1]=(a)[1]-(b)[1]; \
                        (c)[2]=(a)[2]-(b)[2]
//
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent, fpsCurr, fpsStart;
struct timespec timePause;
double physicsCountdown = 0.0;
double timeSpan = 0.0;

class Circle {
public:
	Vec pos;
	Vec vel;
	Vec force;
	int index, orbit;
	Flt radius, radius2;
	Flt mass;
	float angle;
	Flt vert[40][2];
	int nverts;
	Circle() { 
        orbit = 0.0;
		angle = 0.0;
	}
	void init(int n, int rad, int x, int y, int ind) {
		float ang = 0.0;
		float inc = (PI*2.0)/(float)n;
		radius = rad;
		index = ind;
		pos[0] = x;
		pos[1] = y;
		nverts = 0;
		for (int i = 0; i < n; i++) {
			vert[nverts][0] = cos(ang);
			vert[nverts][1] = sin(ang);
			nverts++;
			ang += inc;
		}
		radius2 = radius * radius;
		vel[0] = vel[1] = 0.0f;
	}
	void setVel(Flt x, Flt y) {
		vel[0] = x;
		vel[1] = y;
	}
	void changeVel(Flt x, Flt y) {
		vel[0] += x;
		vel[1] += y;
	}	
	void move() {
		pos[0] += vel[0]; 
		pos[1] += vel[1];
	}
	void accel() {
		const float f[2] = {0.2f, 0};
		vel[0] += f[0]*physicsRate;
		vel[1] += f[1]*physicsRate;
		move();
	}
};

class Global {
public:
	bool toggle;
	int xres, yres;
	Circle planet, planet2, planet3;
	Circle blackhole;
	Circle satellite;
 	Circle orb1, orb2,orb3, bul5, bul6, bul7;
	Circle mark1,mark2,mark3;
	Circle porb;
	Circle troll;
   	 int pos, count, mod, size, length;
    	Flt trail[10][1000][2];
	char keys[65536];
	Global() {
	    	toggle = true;
		xres = 1400, yres = 800;
		memset(keys, 0, 65536);
		mod = 150; 
        	length = 10;
       		size = 1000/mod*length;
        	pos = size - 1;
        	count = 0;
        	for (int i= 0; i < 200; i++) {
            		trail[0][i][0] = 0.0;
            		trail[0][i][1] = 0.0;
            		trail[1][i][0] = 0.0;
           		trail[1][i][1] = 0.0;
           		trail[2][i][0] = 0.0;
           		trail[2][i][1] = 0.0;
			trail[3][i][0] = 0.0;
           		trail[3][i][1] = 0.0;
			trail[4][i][0] = 0.0;
           		trail[4][i][1] = 0.0;
			trail[5][i][0] = 0.0;
           		trail[5][i][1] = 0.0;
			trail[6][i][0] = 0.0;
           		trail[6][i][1] = 0.0;
			trail[7][i][0] = 0.0;
           		trail[7][i][1] = 0.0;
			trail[8][i][0] = 0.0;
           		trail[8][i][1] = 0.0;
			trail[9][i][0] = 0.0;
           		trail[9][i][1] = 0.0;


		}
	        planet.init(32, 80, 1000, 400, 0);//middle
        	orb1.init(10, 8, 1000, 200,1);
		orb1.setVel(0.105, 0.0);
	   	orb2.init(10, 6, 1000, 550,2);
		orb2.setVel(0.115, 0.0);
	   	orb3.init(10, 4, 900, 400,3);
		orb3.setVel(0.0, 0.17);

		planet2.init(40, 80, 200, 600, 0);//bottom
	
	
		planet3.init(20, 40, 600, 200, 0);//top
		porb.init(20,20,600,100,8);
		porb.setVel(0.03,0.01);
		srand(time(NULL));
		blackhole.init(4,4,(rand()%1000+1),(rand()%800+1),4); 	

	//-------------------------------------------------------------------
		satellite.init(10, 6, 200, 400, 4);
		satellite.setVel(0.2, 0.02);
	
		mark1.init(10, 10, 10, 780, 5);
		mark2.init(10, 10, 40, 780, 6);
		mark3.init(10, 10, 70, 780, 7);
		
		bul5.init(10, 6, satellite.pos[0], satellite.pos[1], 5);
		bul5.setVel(0.105, 0.0);
		bul6.init(10, 6, satellite.pos[0], satellite.pos[1], 6);
		bul6.setVel(0.02, 0.002);
		bul7.init(10, 6, satellite.pos[0], satellite.pos[1], 7);
		bul7.setVel(0.002, 0.002);
	//-------------------------------------------------------------------	
		}
}g;


class X11_wrapper {
private:
	Display *dpy;
	Window win;
	GLXContext glc;
	GC gc;
	XdbeBackBuffer backBuffer;
	XdbeSwapInfo swapInfo;
	int h, w;
public:
	~X11_wrapper() {
		//Deallocate back buffer
		if (!XdbeDeallocateBackBufferName(dpy, backBuffer)) {
			fprintf(stderr,"Error : unable to deallocate back buffer.\n");
		}
		XFreeGC(dpy, gc);
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}
	X11_wrapper() {
	    	int major, minor;
		XdbeBackBufferAttributes *backAttr;
		dpy = XOpenDisplay(NULL);
		GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

		Window root = DefaultRootWindow(dpy);
		XWindowAttributes getWinAttr;
    		XGetWindowAttributes(dpy, root, &getWinAttr);
 		XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
        //	int fullscreen = 0;
		setup_screen_res(g.xres,g.yres);
       	/*	if (!w && !h) {
                //Go to fullscreen.
                g.xres = getWinAttr.width;
                g.yres = getWinAttr.height;
                printf("getWinAttr: %i %i\n", w, h); fflush(stdout);
				//When window is fullscreen, there is no client window
                //so keystrokes are linked to the root window.
                XGrabKeyboard(dpy, root, False,
                        GrabModeAsync, GrabModeAsync, CurrentTime);
                fullscreen = 1;
        	}*/
		Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
        	XSetWindowAttributes swa;
        	swa.colormap = cmap;
                swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                StructureNotifyMask | SubstructureNotifyMask;
		unsigned int winops = CWBorderPixel|CWColormap|CWEventMask;
        /*	if (fullscreen) {
                	winops |= CWOverrideRedirect;
			swa.override_redirect = True;
        	}*/
		win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
                	vi->depth, InputOutput, vi->visual, winops, &swa);
        	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);

		//Create gc
		gc = XCreateGC(dpy, win, 0, NULL);
		//Get DBE version
		if (!XdbeQueryExtension(dpy, &major, &minor)) {
			fprintf(stderr, "Error : unable to fetch Xdbe Version.\n");
			XFreeGC(dpy, gc);
			XDestroyWindow(dpy, win);
			XCloseDisplay(dpy);
		}
		printf("Xdbe version %d.%d\n",major,minor);
		//Get back buffer and attributes (used for swapping)
		backBuffer = XdbeAllocateBackBufferName(dpy, win, XdbeUndefined);
		backAttr = XdbeGetBackBufferAttributes(dpy, backBuffer);
	    	swapInfo.swap_window = backAttr->window;
	    	swapInfo.swap_action = XdbeUndefined;
		XFree(backAttr);
		//Map and raise window
		set_window_title();
		XMapWindow(dpy, win);
		XRaiseWindow(dpy, win);
	}
	void set_window_title() {
		char ts[256];
		sprintf(ts, "Put the satellite in orbit.");
		XStoreName(dpy, win, ts);
	}
	void check_resize(XEvent *e) {
		//ConfigureNotify is sent when the window is resized.
		if (e->type != ConfigureNotify)
			return;
		XConfigureEvent xce = e->xconfigure;
	 	if (xce.width != g.xres || xce.height != g.yres) {
			reshape_window(xce.width, xce.height);
		}
	}
	void setup_screen_res(const int w, const int h)
	{
	 	g.xres = w;
        	g.yres = h;
        }
	void reshape_window(int width, int height)
	{
		setup_screen_res(width, height);
    		glViewport(0, 0, (GLint)width, (GLint)height);
    		glMatrixMode(GL_PROJECTION); glLoadIdentity();
    		glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    		glOrtho(0, g.xres, 0, g.yres, -1, 1);
		set_window_title();
	}
	void clear_screen()	{
		XSetForeground(dpy, gc, 0x00050505);
		XFillRectangle(dpy, backBuffer, gc, 0, 0, g.xres, g.yres);
	}
	void set_color_3i(int r, int g, int b) {
		unsigned long cref = 0L;
		cref += r;
		cref <<= 8;
		cref += g;
		cref <<= 8;
		cref += b;
		XSetForeground(dpy, gc, cref);
	}
	bool getXPending() {
		return XPending(dpy);
	}
	XEvent getXNextEvent() {
		XEvent e;
		XNextEvent(dpy, &e);

		return e;
	}
	void drawPoint(int x, int y) {
		XDrawPoint(dpy, backBuffer, gc, x, y);
	}
	void swapBuffers() { XdbeSwapBuffers(dpy, &swapInfo, 1); }
} x11;

//prototypes
void init();
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void physics();
void render();
void words();
double timeDiff(struct timespec *start, struct timespec *end);
void timeCopy(struct timespec *dest, struct timespec *source);
void normalize2d(Vec v);
void unitTest_normalize();
int main()
{
	init();
	srand((unsigned)time(NULL));
	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	int done = 0;
	while (!done) {
		//Handle all events in queue...
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			check_mouse(&e);
			done = check_keys(&e);
		}
		clock_gettime(CLOCK_REALTIME, &timeCurrent);
		timeSpan = timeDiff(&timeStart, &timeCurrent);
		timeCopy(&timeStart, &timeCurrent);
		physicsCountdown += timeSpan;
		while (physicsCountdown >= physicsRate) {
			physics();
			physicsCountdown -= physicsRate;
		}
		//Process physics and rendering every frame
		physics();
		render();
		x11.swapBuffers();
		//usleep(512);
	}
	cleanup_fonts();
	return 0;
}

void init()
{
	glViewport(0, 0, g.xres, g.yres);
    	//Initialize matrices
    	glMatrixMode(GL_PROJECTION); glLoadIdentity();
    	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    	//This sets 2D mode (no perspective)
    	glOrtho(0, g.xres, 0, g.yres, -1, 1);
    	//
    	glDisable(GL_LIGHTING);
    	glDisable(GL_DEPTH_TEST);
    	glDisable(GL_FOG);
    	glDisable(GL_CULL_FACE);
    	//
    	//Clear the screen to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
        //Do this to allow fonts 
        glEnable(GL_TEXTURE_2D); 
        initialize_fonts(); 
}

void normalize2d(Vec v)
{
    Flt len = v[0]*v[0] + v[1]*v[1];
    if (len == 0.0f) {
	v[0] = 1.0;
	v[1] = 0.0;
	return;
    }
    len = 1.0f / sqrt(len);
    v[0] *= len;
    v[1] *= len;
}

double timeDiff(struct timespec *start, struct timespec *end)
{
	//return the difference in two times.
	return (double)(end->tv_sec - start->tv_sec ) +
		(double)(end->tv_nsec - start->tv_nsec) * oobillion;
}

void timeCopy(struct timespec *dest, struct timespec *source)
{
	//copy one time structure to another.
	memcpy(dest, source, sizeof(struct timespec));
}

void check_mouse(XEvent *e)
{
	static int savex = 0;
	static int savey = 0;
	//
	if (e->type != ButtonRelease &&
			e->type != ButtonPress &&
			e->type != MotionNotify)
		return;
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//left button is down
		}
		if (e->xbutton.button==3) {
		}
	}
	if (e->type == MotionNotify) {
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			//mouse moved
			savex = e->xbutton.x;
			savey = e->xbutton.y;
		}
	}
}

int check_keys(XEvent *e)
{
	static int shift = 0;
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&e->xkey, 0);
	if (e->type == KeyRelease) {
		g.keys[key]=1;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			shift = 0;
			return 0;
		}
	}
	if (e->type == KeyPress) {
		g.keys[key]=1;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			shift = 1;
			return 0;
		}
	} else {
		return 0;
	}
	(void)shift;
	//a key was pressed
	switch (key) {
	    	#ifdef UNIT_TEST
	    	case XK_a:
		    void unitTest_normalize();
		    unitTest_normalize();
		    break;
		#endif
		case XK_g:
			g.bul5.init(30, 40, g.satellite.pos[0], g.satellite.pos[1], 5);
			break;
		case XK_Up:
		    	g.toggle = false;
			break;
		case XK_Down:
			g.toggle = true;
			break;
		case XK_Left:
			g.satellite.pos[1] -=10;
			break;
		case XK_Right:
			g.satellite.pos[1] +=10;
			break;
		case XK_Escape:
			//quitting the program
			return 1;
	}
	#ifdef ALEX_UNIT_TEST
	void unittest_keypress(int key);
       	unittest_keypress(key);
	#endif
	return 0;	
}


Flt VecNormalize(Vec vec)
{
    Flt len, tlen;
    Flt xlen = vec[0];
    Flt ylen = vec[1];
    Flt zlen = vec[2];
    len = xlen*xlen + ylen*ylen + zlen*zlen;
    if (len == 0.0) {
	MakeVector(0.0,0.0,1.0,vec);
	return 1.0;
    }
    len = sqrt(len);
    tlen = 1.0 / len;
    vec[0] = xlen * tlen;
    vec[1] = ylen * tlen;
    vec[2] = zlen * tlen;
    return(len);
}
#ifdef UNIT_TEST

Vec varr[] = {
0.62, 3280.8, 3.28,
0.39, 0.039, 0.0254,
2.54, 25.40, 0.30,
0.91, 0.00091, 1.61,
1.057, 0.264, 0.0042,
0.0338, 29.57, 236.6,
0.95, 3.785, 0.0011,
2.2046, 0.035, 0.002205,
0.000035, 28.35, 0.454 };

Flt retvarr[] = { 
3280.80, 
0.392, 
25.52,
1.849, 
1.089, 
238.44,
3.902, 
2.204, 
28.35 };

void unitTest_normalize()
{
	for (int i=0; i < 9; i++) {
		Vec tmp;
		tmp[0] = varr[i][0];
		tmp[1] = varr[i][1];
		tmp[2] = varr[i][2];
		Flt ret = VecNormalize(tmp);
		Flt tolerance = 0.05;
		ret = ret - retvarr[i];
		ret = fabs(ret);
		if (ret <= tolerance) {
			printf("unit test success.\n");
		//debug test to see where the ret and tolerance failed
		//printf("unit test failed. ret: %f retvarr[0]:%lf.\n", ret, retvarr[i]);
		} else {
			printf("unit test failed.ret: %lf",ret);
			printf(" varr[%d]:%lf %lf %lf.\n", i, varr[i][0],
				varr[i][1], varr[i][2]);
		}
	}
}
#endif //UNIT_TEST

void myBresenhamLine(int x0, int y0, int x1, int y1)
{
	//Bresenham line algorithm, integers only.
	int steep = (abs(y1-y0) > abs(x1-x0));
	if (steep) {
		SWAP(x0, y0);
		SWAP(x1, y1);
	}
	if (x0 > x1) {
		SWAP(x0, x1);
		SWAP(y0, y1);
	}
	int ystep = (y1>y0) ? 1 : -1;
	int yDiff = abs(y1-y0);
	int xDiff = x1 - x0;
	int err=xDiff>>1;
	int x, y=y0;
	for	(x=x0; x<=x1; x++) {
		if (steep)
			x11.drawPoint(y, x);
		else
			x11.drawPoint(x, y);
		err -= yDiff;
		if (err <= 0) {
			y += ystep;
			err += xDiff;
		}
	}
}

void show_anchor(int x, int y, int size)
{
	//draw a small rectangle...
	int i,j;
	for (i=-size; i<=size; i++) {
		for (j=-size; j<=size; j++) {
			x11.drawPoint(x+j, y+i);

		}
	}
}

void show_circle(Circle *c)
{
    srand(time(NULL));
    if(c->index == 0)
	    x11.set_color_3i((rand()%256),(rand()%256),(rand()%256));
    if(c->index == 1)
	    x11.set_color_3i(100,255,25);
    if(c->index == 2)
	    x11.set_color_3i(255,0,0);
    if(c->index == 3)
	    x11.set_color_3i(124,50,255);
    if(c->index == 4)
	    x11.set_color_3i(255,255,255);		
    if(c->index == 5)
	    x11.set_color_3i(0,204,204);		
    if(c->index == 6)
	    x11.set_color_3i(204,0,255);		
    if(c->index == 7)
	    x11.set_color_3i(153,38,0);		
    if(c->index == 8)
	    x11.set_color_3i(255,255,255);		

    for (int i = 0; i < c->nverts; i++) {
		int j = i+1;
		if (j >= c->nverts)
			j -= c->nverts;
		myBresenhamLine(
			c->pos[0] + c->vert[i][0] * c->radius,
			c->pos[1] + c->vert[i][1] * c->radius,
			c->pos[0] + c->vert[j][0] * c->radius,
			c->pos[1] + c->vert[j][1] * c->radius);
	}
}

void physics()
{
        Vec norm;
        Flt dist;
        Flt m1, m2;
        //Orb 1 - Red trail
        m1=(4/3)*PI*(g.orb1.radius * g.orb1.radius);
        m2=(4/3)*PI*(g.planet.radius * g.planet.radius);
        norm[0]=g.planet.pos[0]-g.orb1.pos[0];
        norm[1]=g.planet.pos[1]-g.orb1.pos[1];
        dist = sqrt((norm[0]*norm[0])+(norm[1]*norm[1]));
        norm[0]/=dist;
        norm[1]/=dist;
        g.orb1.vel[0] += (norm[0]*(m1*m2/(dist*dist))/10000)/m1;
        g.orb1.vel[1] += (norm[1]*(m1*m2/(dist*dist))/10000)/m1;
        g.planet.vel[0] -= (norm[0]*(m1*m2/(dist*dist))/10000)/m2;
        g.planet.vel[1] -= (norm[1]*(m1*m2/(dist*dist))/10000)/m2;
        
        //Orb 2 -Blue trail   
        m1=(4/3)*PI*(g.orb2.radius*g.orb2.radius);
        m2=(4/3)*PI*(g.planet.radius * g.planet.radius);
        norm[0]=g.planet.pos[0]-g.orb2.pos[0];
        norm[1]=g.planet.pos[1]-g.orb2.pos[1];
        dist = sqrt((norm[0]*norm[0])+(norm[1]*norm[1]));
        norm[0]/=dist;
        norm[1]/=dist;
        g.orb2.vel[0] += (norm[0]*(m1*m2/(dist*dist))/10000)/m1;
        g.orb2.vel[1] += (norm[1]*(m1*m2/(dist*dist))/10000)/m1;
        g.planet.vel[0] -= (norm[0]*(m1*m2/(dist*dist))/10000)/m2;
        g.planet.vel[1] -= (norm[1]*(m1*m2/(dist*dist))/10000)/m2;
        
        //Orb 3 - Pink trail
        m1=(4/3)*PI*(g.orb3.radius*g.orb3.radius);
        m2=(4/3)*PI*(g.planet.radius * g.planet.radius);
        norm[0]=g.planet.pos[0]-g.orb3.pos[0];
        norm[1]=g.planet.pos[1]-g.orb3.pos[1];
        dist = sqrt((norm[0]*norm[0])+(norm[1]*norm[1]));
        norm[0]/=dist;
        norm[1]/=dist;
        g.orb3.vel[0] += (norm[0]*(m1*m2/(dist*dist))/10000)/m1;
        g.orb3.vel[1] += (norm[1]*(m1*m2/(dist*dist))/10000)/m1;
        g.planet.vel[0] -= (norm[0]*(m1*m2/(dist*dist))/10000)/m2;
        g.planet.vel[1] -= (norm[1]*(m1*m2/(dist*dist))/10000)/m2;
 
	
	 
	//porb top planet - 
	m1=(4/3)*PI*(g.porb.radius*g.porb.radius);
        m2=(4/3)*PI*(g.planet3.radius * g.planet3.radius);
        norm[0]=g.planet3.pos[0]-g.porb.pos[0];
        norm[1]=g.planet3.pos[1]-g.porb.pos[1];
        dist = sqrt((norm[0]*norm[0])+(norm[1]*norm[1]));
        norm[0]/=dist;
        norm[1]/=dist;
        g.porb.vel[0] += (norm[0]*(m1*m2/(dist*dist))/50000)/m1;
        g.porb.vel[1] += (norm[1]*(m1*m2/(dist*dist))/50000)/m1;
        g.planet3.vel[0] -= (norm[0]*(m1*m2/(dist*dist))/10000000000)/m2;
        g.planet3.vel[1] -= (norm[1]*(m1*m2/(dist*dist))/10000000000)/m2;
  	
//--------------------------------------------------------------
	//Bullet 5 - teal 
        m1=(4/3)*PI*(g.bul5.radius*g.bul5.radius);
        m2=(4/3)*PI*(g.planet.radius * g.planet.radius);
        norm[0]=g.planet.pos[0]-g.bul5.pos[0];
        norm[1]=g.planet.pos[1]-g.bul5.pos[1];
        dist = sqrt((norm[0]*norm[0])+(norm[1]*norm[1]));
        norm[0]/=dist;
        norm[1]/=dist;
        g.bul5.vel[0] -= (norm[0]*(m1*m2/(dist*dist))/10000)/m1;
        g.bul5.vel[1] -= (norm[1]*(m1*m2/(dist*dist))/10000)/m1;
        g.planet.vel[0] -= (norm[0]*(m1*m2/(dist*dist))/3000)/m2;
        g.planet.vel[1] -= (norm[1]*(m1*m2/(dist*dist))/3000)/m2;
    
	//Bullet 6 - purple 
        m1=(4/3)*PI*(g.bul6.radius*g.bul6.radius);
        m2=(4/3)*PI*(g.planet.radius * g.planet.radius);
        norm[0]=g.planet.pos[0]-g.bul6.pos[0];
        norm[1]=g.planet.pos[1]-g.bul6.pos[1];
        dist = sqrt((norm[0]*norm[0])+(norm[1]*norm[1]));
        norm[0]/=dist;
        norm[1]/=dist;
        g.bul6.vel[0] -= sin((norm[0]*(m1*m2/(dist*dist))/10000))/m1;
        g.bul6.vel[1] -= sin((norm[1]*(m1*m2/(dist*dist))/10000))/m1;
        g.planet.vel[0] -= (norm[0]*(m1*m2/(dist*dist))/1000)/m2;
        g.planet.vel[1] -= (norm[1]*(m1*m2/(dist*dist))/1000)/m2;
    
	//Bullet 7 - red	
      	m1=(4/3)*PI*(g.bul7.radius*g.bul7.radius);
        m2=(4/3)*PI*(g.planet.radius * g.planet.radius);
        norm[0]=g.planet.pos[0]-g.bul7.pos[0];
        norm[1]=g.planet.pos[1]-g.bul7.pos[1];
        dist = sqrt((norm[0]*norm[0])+(norm[1]*norm[1]));
        norm[0]/=dist;
        norm[1]/=dist;
        g.bul7.vel[0] -= ((norm[0]*(m1*m2/(dist*dist))/10000))/m1;
        g.bul7.vel[1] += ((norm[1]*(m1*m2/(dist*dist))/10000))/m1;
        g.planet.vel[0] -= sin(norm[0]*(m1*m2/(dist*dist))/9000)/m2;
        g.planet.vel[1] += sin(norm[1]*(m1*m2/(dist*dist))/9000)/m2;
//----------------------------------------------------------------------    


        if(g.count % g.mod==0){
        g.trail[0][g.pos][0]=g.orb1.pos[0];
        g.trail[0][g.pos][1]=g.orb1.pos[1];
        g.trail[1][g.pos][0]=g.orb2.pos[0];
        g.trail[1][g.pos][1]=g.orb2.pos[1];
        g.trail[2][g.pos][0]=g.orb3.pos[0];
        g.trail[2][g.pos][1]=g.orb3.pos[1];
        g.trail[3][g.pos][0]=g.bul5.pos[0];
        g.trail[3][g.pos][1]=g.bul5.pos[1];
 	g.trail[4][g.pos][0]=g.bul6.pos[0];
        g.trail[4][g.pos][1]=g.bul6.pos[1];
  	g.trail[5][g.pos][0]=g.bul7.pos[0];
        g.trail[5][g.pos][1]=g.bul7.pos[1];
   	g.trail[6][g.pos][0]=g.porb.pos[0];
        g.trail[6][g.pos][1]=g.porb.pos[1];
   
	g.pos--;
            if(g.pos < 0) {
                g.pos=g.size-1;
            }
        }
        g.count++;
	bool run = true;
	if(!g.keys[XK_1] && run == true) {
		g.bul5.pos[0] = g.satellite.pos[0];
		g.bul5.pos[1] = g.satellite.pos[1];
	}
	if(!g.keys[XK_2] && run == true) {
	   	g.bul6.pos[0] = g.satellite.pos[0];
	   	g.bul6.pos[1] = g.satellite.pos[1];
	}
	if(!g.keys[XK_3] && run == true) {
	   	g.bul7.pos[0] = g.satellite.pos[0];
	   	g.bul7.pos[1] = g.satellite.pos[1];
	}
//*****************Collision Detection for Screen*************************
	//top of the screen
        if (g.satellite.pos[1] < 250.0) {
		g.satellite.pos[1] = 250.0;
	}
	//botton of screen
	else if (g.satellite.pos[1] > (float)g.yres-250) {
		g.satellite.pos[1] = (float)g.yres-250;
	}
//***************************************************************************
//***************************************************************************
	if (g.keys[XK_1]){
	    	run = false;
	    	g.bul5.vel[0] *= 0.95;
	    	g.bul5.accel();
	}
	if (g.keys[XK_2]){
		g.bul6.vel[0] *= (sin(75)+1.35);
		g.bul6.accel();
	}

        if (g.keys[XK_3]) {
	    	g.bul7.vel[0] *= 0.95;
		g.bul7.accel();
	
	}

//***************************************************************************
	  	norm[0]=g.planet.pos[0]-g.planet3.pos[0];
        	norm[1]=g.planet.pos[1]-g.planet3.pos[1];
        	dist = sqrt((norm[0]*norm[0])+(norm[1]*norm[1]));
		Vec col;
		Flt apart;
		col[0]= g.planet3.pos[0]-g.planet2.pos[0];
		col[1]= g.planet3.pos[1]-g.planet2.pos[1];
        	apart = sqrt((col[0]*col[0])+(col[1]*col[1]));
		Vec col2;
		Flt apt;
		col2[0]= g.planet.pos[0]-g.planet2.pos[0];
		col2[1]= g.planet.pos[1]-g.planet2.pos[1];
        	apt = sqrt((col2[0]*col2[0])+(col2[1]*col2[1]));

		if (apart <= 110) {
		    g.planet2.vel[0] = 0.05;
		    g.planet2.vel[1] = 0.05;
		}
		if (apt <= 156) {
		    g.planet2.vel[0] = 0.05;
		    g.planet2.vel[1] = 0.05;
		}
		if (dist <= 118) {
    			g.planet3.vel[0] -= 0.05;
    			g.planet3.vel[1] += 0.05;
		}
//***************************************************************************
		Vec o1,o2,o3;
		Flt c1,c2,c3;
		o1[0]=g.bul5.pos[0]-g.orb1.pos[0];
        	o1[1]=g.bul5.pos[1]-g.orb1.pos[1];
        	o2[0]=g.bul5.pos[0]-g.orb2.pos[0];
        	o2[1]=g.bul5.pos[1]-g.orb2.pos[1];
		o3[0]=g.bul5.pos[0]-g.orb3.pos[0];
        	o3[1]=g.bul5.pos[1]-g.orb3.pos[1];
		c1 = sqrt((o1[0]*o1[0])+(o1[1]*o1[1]));
		c2 = sqrt((o2[0]*o2[0])+(o2[1]*o2[1]));
		c3 = sqrt((o3[0]*o3[0])+(o3[1]*o3[1]));
		
		if (c1 < 6.5 && c1 > 5.4 ){
		   g.bul5.vel[0] *= -50; 
		   g.bul5.vel[1] *= 30; 
		}
		
		if(c2 < 11.6 && c2 > 11.1){
		   g.bul5.vel[0] *= 50; 
		   g.bul5.vel[1] *= -70; 
		}
		if(c3 < 13.9 && c3 > 13.3) {
		   g.bul5.vel[0] *= 90; 
		   g.bul5.vel[1] *= 90; 
		}
//**************************************************************************
		Vec o4,o5,o6;
		Flt c4,c5,c6;
		o4[0]=g.bul6.pos[0]-g.orb1.pos[0];
        	o4[1]=g.bul6.pos[1]-g.orb1.pos[1];
        	o5[0]=g.bul6.pos[0]-g.orb2.pos[0];
        	o5[1]=g.bul6.pos[1]-g.orb2.pos[1];
		o6[0]=g.bul6.pos[0]-g.orb3.pos[0];
        	o6[1]=g.bul6.pos[1]-g.orb3.pos[1];
		c4 = sqrt((o4[0]*o4[0])+(o4[1]*o4[1]));
		c5 = sqrt((o5[0]*o5[0])+(o5[1]*o5[1]));
		c6 = sqrt((o6[0]*o6[0])+(o6[1]*o6[1]));
		
		if (c4 < 10.5 && c4 > 10.1 ){
		   g.bul6.vel[0] *= -50; 
		   g.bul6.vel[1] *= 30; 
		}
		
		if(c5 < 13.6 && c5 > 13.1){
		   g.bul6.vel[0] *= 50; 
		   g.bul6.vel[1] *= -70; 
		}
		if(c6 < 14.4 && c6 > 13.8) {
		   g.bul6.vel[0] *= 90; 
		   g.bul6.vel[1] *= 90; 
		}

//*****************************************************************************
		Vec o7,o8,o9;
		Flt c7,c8,c9;
		o7[0]=g.bul7.pos[0]-g.orb1.pos[0];
        	o7[1]=g.bul7.pos[1]-g.orb1.pos[1];
        	o8[0]=g.bul7.pos[0]-g.orb2.pos[0];
        	o8[1]=g.bul7.pos[1]-g.orb2.pos[1];
		o9[0]=g.bul7.pos[0]-g.orb3.pos[0];
        	o9[1]=g.bul7.pos[1]-g.orb3.pos[1];
		c7 = sqrt((o7[0]*o7[0])+(o7[1]*o7[1]));
		c8 = sqrt((o8[0]*o8[0])+(o8[1]*o8[1]));
		c9 = sqrt((o9[0]*o9[0])+(o9[1]*o9[1]));
		
		if (c7 < 14.1 && c7 > 13.5 ){
		   g.bul7.vel[0] *= -5; 
		   g.bul7.vel[1] *= 3; 
		}
		
		if(c8 < 14.0 && c8 > 13.4){
		   g.bul7.vel[0] *= 50; 
		   g.bul7.vel[1] *= -70; 
		}
		if(c9 < 16.4 && c9 > 15.8) {
		   g.bul7.vel[0] *= 90; 
		   g.bul7.vel[1] *= 90; 
		}

	

		if(g.keys[XK_space]){

		   printf("\nc1: %lf:", c9);

	}
		Vec in;
		Flt disappear, mass1, mass2;
		mass1=(4/3)*PI*(g.bul5.radius*g.bul5.radius);
        	mass2=(4/3)*PI*(g.blackhole.radius * g.blackhole.radius);

		in[0]= g.blackhole.pos[0]-g.bul5.pos[0] 
		    = g.blackhole.pos[0]-g.bul6.pos[0] 
		    = g.blackhole.pos[0]-g.bul7.pos[0];
		in[1]= g.blackhole.pos[1]-g.bul5.pos[1] 
		    = g.blackhole.pos[1]-g.bul6.pos[1] 
		    = g.blackhole.pos[1]-g.bul7.pos[1];
        	disappear = sqrt((in[0]*in[0])+(in[1]*in[1]));
        	in[0]/=disappear;
        	in[1]/=disappear;
        	g.bul7.vel[0] -= ((in[0]*(mass1*mass2/(disappear*disappear))/10000))/m1;
        	g.bul7.vel[1] += ((in[1]*(mass1*mass2/(disappear*disappear))/10000))/m1;
        	g.planet.vel[0] -= sin(in[0]*(mass1*mass2/(disappear*disappear))/9000)/m2;
        	g.planet.vel[1] += sin(in[1]*(mass1*mass2/(disappear*disappear))/9000)/m2;

//**************************************************************************
            g.planet.move();
     	    g.orb1.move();
	    g.orb2.move();
	    g.orb3.move();
	    g.planet2.move();
            g.planet3.move();
	    g.porb.move();

}

void render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    if (g.toggle) {
    	x11.clear_screen();
    }
    Flt r1,gr1,b1,r2,gr2,b2,r3,gr3,b3,ic;
    Flt a1,a2,a3,ba1,ba2,ba3,c1,c2,c3, mod;
    Flt p1,p2,p3;
    mod= 800 /g.mod;
    int first = 0;
    int loop = g.pos + 1;
    if( loop > g.size - 1){
        loop = 0;
    }
    
    x11.set_color_3i(255,100,100);
    myBresenhamLine(g.orb1.pos[0],g.orb1.pos[1],g.trail[0][loop][0],g.trail[0][loop][1]);
    x11.set_color_3i(100,100,255);
    myBresenhamLine(g.orb2.pos[0],g.orb2.pos[1],g.trail[1][loop][0],g.trail[1][loop][1]);
    x11.set_color_3i(180,100,180);
    myBresenhamLine(g.orb3.pos[0],g.orb3.pos[1],g.trail[2][loop][0],g.trail[2][loop][1]);
   //----------------------------------------------------------------------  
    x11.set_color_3i(210,150,100);
    myBresenhamLine(g.bul5.pos[0],g.bul5.pos[1],g.trail[3][loop][0],g.trail[3][loop][1]);
    x11.set_color_3i(255,255,255);
    myBresenhamLine(g.bul6.pos[0],g.bul6.pos[1],g.trail[4][loop][0],g.trail[4][loop][1]);
    x11.set_color_3i(64,224,208);
    myBresenhamLine(g.bul7.pos[0],g.bul7.pos[1],g.trail[5][loop][0],g.trail[5][loop][1]);
    //----------------------------------------------------------------------
    x11.set_color_3i(255,255,250);
    myBresenhamLine(g.porb.pos[0],g.porb.pos[1],g.trail[6][loop][0],g.trail[6][loop][1]);
 
    for(int i = g.size-1; i > 60/g.mod; i--){
        int tempi = i+g.pos;
        int templ = (i+1)+g.pos;
        if(templ > g.size-1){
            templ = templ-g.size;
        }
        if(templ < 0){
            templ = templ + g.size;
        }
        if(tempi > g.size-1){
            tempi = tempi - g.size;
        }
        if(tempi < 0){
            tempi = tempi + g.size;
        }
        
        ic=(i + mod* g.length/5)/(mod * g.length/5);
        r1=255*(1/ic);
        gr1=100*(1/ic);
        b1=100*(1/ic);
        
        r2=100*(1/ic);
        gr2=100*(1/ic);
        b2=255*(1/ic);
        
        r3=180*(1/ic);
        gr3=100*(1/ic);
        b3=180*(1/ic);
	
	a1=210*(1/ic);
	a2=150*(1/ic);
	a3=100*(1/ic);
	
	ba1=255*(1/ic);
	ba2=255*(1/ic);
	ba3=255*(1/ic);
		
	c1=64*(1/ic);
	c2=224*(1/ic);
	c3=208*(1/ic);
	
	p1=255*(1/ic);
	p2=255*(1/ic);
	p3=250*(1/ic);
//------------------------------------------------------------------	
        x11.set_color_3i(r1,gr1,b1);
        x11.drawPoint(g.trail[0][tempi][0],g.trail[0][tempi][1]);
        x11.set_color_3i(r2,gr2,b2);
        x11.drawPoint(g.trail[1][tempi][0],g.trail[1][tempi][1]);
        x11.set_color_3i(r3,gr3,b3);
        x11.drawPoint(g.trail[2][tempi][0],g.trail[2][tempi][1]);
//-------------------------------------------------------------------       
	x11.set_color_3i(a1,a2,a3);
        x11.drawPoint(g.trail[3][tempi][0],g.trail[3][tempi][1]);
	x11.set_color_3i(ba1,ba2,ba3);
        x11.drawPoint(g.trail[4][tempi][0],g.trail[4][tempi][1]);
	x11.set_color_3i(c1,c2,c3);
        x11.drawPoint(g.trail[5][tempi][0],g.trail[5][tempi][1]);
//-------------------------------------------------------------------
	x11.set_color_3i(p1,p2,p3);
        x11.drawPoint(g.trail[6][tempi][0],g.trail[6][tempi][1]);


        
        if(first&&g.mod>10){
            if(g.trail[0][templ][0]!=0&&g.trail[0][templ][1]!=0){
                x11.set_color_3i(r1,gr1,b1);
                myBresenhamLine(g.trail[0][templ][0],g.trail[0][templ][1],g.trail[0][tempi][0],g.trail[0][tempi][1]);
                x11.set_color_3i(r2,gr2,b2);
                myBresenhamLine(g.trail[1][templ][0],g.trail[1][templ][1],g.trail[1][tempi][0],g.trail[1][tempi][1]);
                x11.set_color_3i(r3,gr3,b3);
                myBresenhamLine(g.trail[2][templ][0],g.trail[2][templ][1],g.trail[2][tempi][0],g.trail[2][tempi][1]);
//--------------------------------------------------------------------------
    	        x11.set_color_3i(p1,p2,p3);
                myBresenhamLine(g.trail[6][templ][0],g.trail[6][templ][1],g.trail[6][tempi][0],g.trail[6][tempi][1]);
//------
        
		}
    	}
    	if(first&&g.mod>150){
            if(g.trail[0][templ][0]!=0&&g.trail[0][templ][1]!=0){
		x11.set_color_3i(a1,a2,a3);
                myBresenhamLine(g.trail[3][templ][0],g.trail[3][templ][1],g.trail[3][tempi][0],g.trail[3][tempi][1]);
		x11.set_color_3i(ba1,ba2,ba3);
                myBresenhamLine(g.trail[4][templ][0],g.trail[4][templ][1],g.trail[4][tempi][0],g.trail[4][tempi][1]);
		x11.set_color_3i(c1,c2,c3);
                myBresenhamLine(g.trail[5][templ][0],g.trail[5][templ][1],g.trail[5][tempi][0],g.trail[5][tempi][1]);
	
	    }
    	}
    	first=1;
    }

//--------------------------------------------------------------------------
	show_circle(&g.blackhole);    
	show_circle(&g.planet2);
	show_circle(&g.satellite);
//--------------------------------------------------------------------------
	show_circle(&g.planet);
	show_circle(&g.orb1);
	show_circle(&g.orb2);
	show_circle(&g.orb3);
//--------------------------------------------------------------------------
	show_circle(&g.planet3);
	show_circle(&g.porb);
//------------------------------------------------------------------------
	show_circle(&g.bul5);
	if(g.bul5.pos[0] < (float)g.xres && g.bul5.pos[1] > 0.0)
		show_circle(&g.mark1);
	
	show_circle(&g.bul6);
	if(g.bul6.pos[0] < (float)g.xres && g.bul6.pos[1] < (float)g.yres)
		show_circle(&g.mark2);
	
	show_circle(&g.bul7);
	if((g.bul7.pos[0] < (float)g.xres && g.bul7.pos[0] > 0.0) && ((g.bul7.pos[1] < (float)g.yres && g.bul7.pos[1] > 0.0)))
		show_circle(&g.mark3);
}

#ifdef ALEX_UNIT_TEST
void unittest_keypress(int key)
{
	if (key) {
		printf("Unit test passed. Keys pressed: %c.\n", key);
	} else {
		printf("Unit test failed.\n");
	}
}
#endif //ALEX_UNIT_TEST

















