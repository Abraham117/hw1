//cs3350 Spring 2017 Lab-1
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
//extern "C" {
//	#include "fonts.h"
//}

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 1000  //45000
#define GRAVITY 0.2
#define rnd() (float)rand() / (float)RAND_MAX

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
	float x, y, z;
};

struct Shape {
	float width, height;
	float radius;
	Vec center;
};

struct Particle {
	Shape s;
	Vec velocity;
};

struct Game {
	Shape box;
	Shape ball;
	Particle particle[MAX_PARTICLES];
	int n;
	int bubbler;
	int mouse[2];
	Game() { n=0; bubbler=0;  }
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game, Game *game1, Game *game2, Game *game3, Game *game4, Game *game5);
void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius);
void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius);
void render(Game *game, Game *game1, Game *game2, Game *game3, Game *game4);


int main(void)
{
	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();

//-----------------------------------------------------------

	//declare game object
	Game game;
	game.n=0;

	//declare a box shape
	game.box.width = 90;
	game.box.height = 20;
	game.box.center.x = 150;     //original 120 + 5*65
	game.box.center.y = 480;   //original 500 - 5*60

//-----------------------------------------------------------

	//declare game object1
	Game game1;
	game1.n=0;

	//declare a box shape
	game1.box.width = 90;
	game1.box.height = 20;
	game1.box.center.x = 230;
	game1.box.center.y = 420;

//-----------------------------------------------------------

	//declare game object2
	Game game2;
	game2.n=0;

	//declare a box shape
	game2.box.width = 90;
	game2.box.height = 20;
	game2.box.center.x = 310;
	game2.box.center.y = 360;

//-----------------------------------------------------------

	//declare game object3
	Game game3;
	game3.n=0;

	//declare a box shape
	game3.box.width = 90;
	game3.box.height = 20;
	game3.box.center.x = 390;
	game3.box.center.y = 300;

//-----------------------------------------------------------

	//declare game object4
	Game game4;
	game4.n=0;

	//declare a box shape
	game4.box.width = 90;
	game4.box.height = 20;
	game4.box.center.x = 470;
	game4.box.center.y = 240;

//-----------------------------------------------------------

	//declare circle
	Game game5;
	game5.n=0;

	//declare circle shape
	game5.ball.radius = 170;
	game5.ball.center.x = 750;
	game5.ball.center.y = 0;

//-----------------------------------------------------------

	//start animation
	while (!done) {
		while (XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_mouse(&e, &game);
			done = check_keys(&e, &game);
		}
		movement(&game, &game1, &game2, &game3, &game4, &game5);
		render(&game, &game1, &game2, &game3, &game4);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	return 0;
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "3350 Lab1   LMB for particle");
}

void cleanupXWindows(void)
{
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void)
{
	//do not change
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server\n" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask | PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
		InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);   //original 0.1,0.1,0.1,1.0
	//Do this to allow fonts
//	glEnable(GL_TEXTURE_2D);
//	initialize_fonts();
}

void makeParticle(Game *game, int x, int y)
{
	if (game->n >= MAX_PARTICLES)
		return;
	std::cout << "makeParticle() " << x << " " << y << std::endl;
	//position of particle
	Particle *p = &game->particle[game->n];
	p->s.center.x = x;
	p->s.center.y = y;
	p->velocity.y =  rnd() * 1.0 - 0.25;
	p->velocity.x =  rnd() * 1.0 - 0.25;
	game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
	static int savex = 0;
	static int savey = 0;
//	static int n = 0;

	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed
			int y = WINDOW_HEIGHT - e->xbutton.y;
			for (int i = 0; i <50;i++)
			makeParticle(game, e->xbutton.x, y);
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed
			return;
		}
	}
	//Did the mouse move?
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		int y = WINDOW_HEIGHT - e->xbutton.y;
		if (game->bubbler == 0) {
		    game->mouse[0] = savex;
		    game->mouse[1] = y;
		}
		for (int i = 0; i <5;i++)
		makeParticle(game, e->xbutton.x, y);
	}

//		if (++n < 10)
//			return;
//	}
}

int check_keys(XEvent *e, Game *game)
{
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
		int key = XLookupKeysym(&e->xkey, 0);
		if (key == XK_b) {
		    game->bubbler ^= 1;
			
		}
		if (key == XK_Escape) {
			return 1;
		}
		//You may check other keys here.
	}
	return 0;
}

void movement(Game *game, Game *game1, Game *game2, Game *game3, Game *game4, Game *game5)
{
	Particle *p;

	if (game->n <= 0)
		return;

	if (game->bubbler != 0) {
	    //bubbler is on
	    // makeParticle(game,WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
	    makeParticle(game,game->mouse[0],game->mouse[1]);
	}

	for (int i=0; i <game->n; i++) {
	    	p = &game->particle[i];
		p->velocity.y -= GRAVITY;
		p->s.center.x += p->velocity.x;
		p->s.center.y += p->velocity.y;
	
//-----------------------------------------------------------

		//check for collision with shape
		Shape *s;
		s = &game->box;
		if (p->s.center.y < s->center.y + s->height &&
			p->s.center.x >= s->center.x - s->width &&
			p->s.center.x <= s->center.x + s->width)
       	{
			p->s.center.y = s->center.y + s->height;
			p->velocity.y = -p->velocity.y * 0.8f;
			p->velocity.x += 0.008f;
		};
	
//-----------------------------------------------------------

		//check for collision with shape1
		Shape *s1;
		s1 = &game1->box;
		if (p->s.center.y < s1->center.y + s1->height &&
			p->s.center.x >= s1->center.x - s1->width &&
			p->s.center.x <= s1->center.x + s1->width)
       	{
			p->s.center.y = s1->center.y + s1->height;
			p->velocity.y = -p->velocity.y * 0.5f;
			p->velocity.x += 0.008f;
		};

//-----------------------------------------------------------

		//check for collision with shape2
		Shape *s2;
		s2 = &game2->box;
		if (p->s.center.y < s2->center.y + s2->height &&
			p->s.center.x >= s2->center.x - s2->width &&
			p->s.center.x <= s2->center.x + s2->width)
       	{
			p->s.center.y = s2->center.y + s2->height;
			p->velocity.y = -p->velocity.y * 0.4f;
			p->velocity.x += 0.008f;
		};

//-----------------------------------------------------------

		//check for collision with shape3
		Shape *s3;
		s3 = &game3->box;
		if (p->s.center.y < s3->center.y + s3->height &&
			p->s.center.x >= s3->center.x - s3->width &&
			p->s.center.x <= s3->center.x + s3->width)
       	{
			p->s.center.y = s3->center.y + s3->height;
			p->velocity.y = -p->velocity.y * 0.4f;
			p->velocity.x += 0.008f;
		};

//-----------------------------------------------------------

		//check for collision with shape4
		Shape *s4;
		s4 = &game4->box;
		if (p->s.center.y < s4->center.y + s4->height &&
			p->s.center.x >= s4->center.x - s4->width &&
			p->s.center.x <= s4->center.x + s4->width)
       	{
			p->s.center.y = s4->center.y + s4->height;
			p->velocity.y = -p->velocity.y * 0.4f;
			p->velocity.x += 0.4f;
		};

//-----------------------------------------------------------		

		//check for collision with circle
		Shape *s5;
		s5 = &game5->ball;


		GLfloat radius = 0.8f; //radius
		float x = 750;
		float y = 0;
		GLfloat twicePi = 2.0f * M_PI;
		
//		if () {
			for(int j = 0; j <= 100;j++) { 
				x = s5->center.x + (radius * cos(j * twicePi / 100)); 
				y = s5->center.y + (radius * sin(j * twicePi / 100));
			}

//-----------------------------------------------------------

		//check for off-screen
		if (p->s.center.y < 0.0) {
			std::cout << "off screen" << std::endl;
			game->particle[i] = game->particle[--game->n];
		}
	}
}

void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius){
	//code used from https://gist.github.com/strife25/803118

	int triangleAmount = 100; //# of triangles used to draw circle
	
	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * M_PI;

	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(int i = 0; i <= triangleAmount;i++) { 
			glVertex2f(
		        x + (radius * cos(i * twicePi / triangleAmount)), 
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();
}

void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius){
	//code used from https://gist.github.com/strife25/803118

	int lineAmount = 100; //# of triangles used to draw circle
	
	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * M_PI;
	
	glBegin(GL_LINE_LOOP);

		for(int i = 0; i <= lineAmount;i++) { 
			glVertex2f(
			    x + (radius * cos(i * twicePi / lineAmount)), 
			    y + (radius * sin(i * twicePi / lineAmount))
			);
		}
	glEnd();
}

void render(Game *game, Game *game1, Game *game2, Game *game3, Game *game4)
{
	float w, h;
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw shapes...

//-----------------------------------------------------------

	//draw box
	Shape *s;
	glColor3ub(61,56,55);		//original 90,140,90
	s = &game->box;
	glPushMatrix();
	glTranslatef(s->center.x, s->center.y, s->center.z);
	w = s->width;
	h = s->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();

//-----------------------------------------------------------

	//draw box1
	Shape *s1;
	glColor3ub(71,66,65);		//original 90,140,90
	s1 = &game1->box;
	glPushMatrix();
	glTranslatef(s1->center.x, s1->center.y, s1->center.z);
	w = s1->width;
	h = s1->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();

//-----------------------------------------------------------

	//draw box2
	Shape *s2;
	glColor3ub(81,76,75);		//original 90,140,90
	s2 = &game2->box;
	glPushMatrix();
	glTranslatef(s2->center.x, s2->center.y, s2->center.z);
	w = s2->width;
	h = s2->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();

//-----------------------------------------------------------

	//draw box3
	Shape *s3;
	glColor3ub(91,86,85);		//original 90,140,90
	s3 = &game3->box;
	glPushMatrix();
	glTranslatef(s3->center.x, s3->center.y, s3->center.z);
	w = s3->width;
	h = s3->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();

//-----------------------------------------------------------

	//draw box4
	Shape *s4;
	glColor3ub(101,96,95);		//original 90,140,90
	s4 = &game4->box;
	glPushMatrix();
	glTranslatef(s4->center.x, s4->center.y, s4->center.z);
	w = s4->width;
	h = s4->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();

//-----------------------------------------------------------

	//draw circle
	glColor3ub(111,106,105);
	drawFilledCircle(750,0,170);
	glColor3ub(0,0,0);
	drawHollowCircle(750,0,170);
	drawHollowCircle(750,0,169.5);
	drawHollowCircle(750,0,169);
	drawHollowCircle(750,0,168.5);

//-----------------------------------------------------------	

	//draw all particles here
	int a,d,b,g;
	for (int i = 0; i <game->n; i++) {
		
		a = rand()%5;
			if(a == 1) {
				d = 73;
				b = 113;
				g = 156;
			}
			else {	
				d = 57;
				b = 88;
				g = 121;
			}

		glPushMatrix();
		glColor3ub(d,b,g);    //original 150,160,220
		Vec *c = &game->particle[i].s.center;
		w = 5;
		h = 5;
		glBegin(GL_QUADS);
			glVertex2i(c->x-w, c->y-h);
			glVertex2i(c->x-w, c->y+h);
			glVertex2i(c->x+w, c->y+h);
			glVertex2i(c->x+w, c->y-h);
		glEnd();
		glPopMatrix();
	}

	//text is here
	//I couldn't get this code to work
//	int yres=480;
	//
//	Rect r;
//	glClear(GL_COLOR_BUFFER_BIT);
	//
//	r.bot = yres - 20;
//	r.left = 10;
//	r.center = 0;
//	ggprint8b(&r, 16, 0x00ff00, "Waterfall");
}








