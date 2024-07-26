#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Matt Hearne

// title of these windows:

const char *WINDOWTITLE = "OpenGL / Starship Moon Landing -- Matt Hearne";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// used for animation
const int MSEC = 40000;   // 10000 = 10 seconds so this is 40 seconds

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE     = GL_LINEAR;
const GLfloat FOGDENSITY  = 0.30f;
const GLfloat FOGSTART    = 1.5f;
const GLfloat FOGEND      = 4.f;

// for lighting:

const float	WHITE[ ] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 10000;		// 10000 milliseconds = 10 seconds


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
GLuint	BoxList;				// object display list
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		// ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
GLuint  Starship;				// starship obj
GLuint  Booster;                // booster obj
GLuint  StarshipTex;            // Starship Texture
GLuint  EarthDL;
GLuint  EarthTex;
GLuint  SphereDL;
GLuint  MoonDL;
GLuint  MoonTex;
GLuint  LaunchPad;
GLuint  MoonSurface;
GLuint  Explosion; 
GLuint  ExplosionTex;
GLuint  Space;
GLuint  SpaceTex;
GLuint  RocketTex;

unsigned char* SpaceTexture;
unsigned char* EarthTexture;
unsigned char* MoonTexture;
unsigned char* ExplosionTexture;
char* FaceFiles[6] = {
	"nvposx.bmp",
	"nvnegx.bmp",
	"nvposy.bmp",
	"nvnegy.bmp",
	"nvposz.bmp",
	"nvnegz.bmp"
};

// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );
void			HsvRgb( float[3], float [3] );
void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);
float			Unit(float [3]);


// utility to create an array from 3 separate values:

float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3( float factor, float array0[ ] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float *
MulArray3(float factor, float a, float b, float c )
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
//#include "osucone.cpp"
//#include "osutorus.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
#include "glslprogram.cpp"

GLSLProgram RocketProgram;
GLSLProgram BoosterS;
GLSLProgram SpaceProgram;
GLSLProgram EarthProgram;
GLSLProgram MoonProgram;
GLSLProgram ExplosionProgram;
GLSLProgram FloorProgram;


Keytimes Ypos1;      // used for Starship1
Keytimes Ypos2;      // Booster1
Keytimes ScaleEx;   // Explosion1
Keytimes Ypos3;      // Explosion1
Keytimes Yview;      // x position view
Keytimes ScaleR;      // scale factor for rocket
Keytimes ScaleB;       // scale factor for booster
Keytimes ThetaY;     // degree change for rocket
Keytimes Zpos1;       // moves the rocket in space 
Keytimes Iposx;      // Eye look at position x
Keytimes Iposy;      // Eye look at position y
Keytimes Iposz;      // Eye look at position z
Keytimes Iatx;        // Eye look at X
Keytimes Iaty;       // Eye look at y
Keytimes Ypos4;      // used to land Starship on the moon

// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display lists that **will not change**:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow( MainWindow );

	// erase the background:
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
#ifdef DEMO_DEPTH_BUFFER
	if( DepthBufferOn == 0 )
		glDisable( GL_DEPTH_TEST );
#endif

	// turn # msec into the cycle ( 0 - MSEC-1 ):
	int msec = glutGet(GLUT_ELAPSED_TIME) % MSEC;

	// turn that into a time in seconds:
	float nowTime = (float)msec / 1000.;

	// specify shading to be flat:

	glShadeModel( GL_FLAT );

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( NowProjection == ORTHO )
		glOrtho( -2.f, 2.f,     -2.f, 2.f,     0.1f, 1000.f );
	else
		gluPerspective( 70.f, 1.f,	0.1f, 1000.f );

	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector:
	if (nowTime <= 10)
		gluLookAt(1.f, -1.f, 5.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f);
	else if (nowTime >= 20.01 && nowTime < 30.01)
		gluLookAt(1.f, -1.f, 5.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
	else if (nowTime >= 30.01)
		gluLookAt(0.f, 100.f, 3.f, 0.f, 100.f, 0.f, 0.f, 1.f, 0.f);
	else
		gluLookAt(-4.0f, -100.f, 3.0f, 6.f, -100.f, 0.f, 0.f, 1.f, 0.f);
	//gluLookAt(Iposx.GetValue(nowTime), -Iposy.GetValue(nowTime), Iposz.GetValue(nowTime), Iatx.GetValue(nowTime), Iaty.GetValue(nowTime), 0.f, 0.f, 1.f, 0.f);
	//gluLookAt( -4.0f, -10.f, 3.0f, 6.f, -10.f, 0.f, 0.f, 1.f, 0.f);
	
	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0.f, 1.f, 0.f );
	glRotatef( (GLfloat)Xrot, 1.f, 0.f, 0.f );

	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[NowColor][0] );
		glCallList( AxesList );
	}

	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable( GL_NORMALIZE );
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT5);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// a point light
	SetPointLight(GL_LIGHT0, 0, 3, 0, 1, 1, 1);	
	SetSpotLight(GL_LIGHT4, 2, 2, 1, 0, 1, 0, 1, 1, 1);

	// draw Starship that takes off:
	int ReflectUnit = 6;
	int RefractUnit = 7;
	float Mix = 0.7f;
	float uWhiteMix = 0.9f;
	float uWhiteorBlack = 1.0f;
	float uWhiteorRed = 1.0f;

	RocketProgram.Use();
	glActiveTexture(GL_TEXTURE0 + ReflectUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	glActiveTexture(GL_TEXTURE0 + RefractUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	RocketProgram.SetUniformVariable("uMix", Mix);
	RocketProgram.SetUniformVariable("uWhiteMix", uWhiteMix);
	RocketProgram.SetUniformVariable("uRefractUnit", RefractUnit);
	RocketProgram.SetUniformVariable("uReflectUnit", ReflectUnit);
	RocketProgram.SetUniformVariable("uWhiteorRed", uWhiteorRed);
	RocketProgram.SetUniformVariable("uWhiteorBlack", uWhiteorBlack);
	
	glPushMatrix();
		//glDisable(GL_TEXTURE_2D);
		glTranslatef(0.f, Ypos1.GetValue(nowTime), 2.5f);
		glRotatef(90, -1, 0, 0.);
		glScalef(0.1f, 0.1f, 0.1f);
		//SetMaterial(0.8, 0.8, 0.8, 15);
		glCallList( Starship );
	glPopMatrix();
	RocketProgram.UnUse();
	
	//Draw Starship that leaves Earth
	RocketProgram.Use();
	glActiveTexture(GL_TEXTURE0 + ReflectUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	glActiveTexture(GL_TEXTURE0 + RefractUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	RocketProgram.SetUniformVariable("uMix", Mix);
	RocketProgram.SetUniformVariable("uWhiteMix", uWhiteMix);
	RocketProgram.SetUniformVariable("uRefractUnit", RefractUnit);
	RocketProgram.SetUniformVariable("uReflectUnit", ReflectUnit);
	RocketProgram.SetUniformVariable("uWhiteorRed", uWhiteorRed);
	RocketProgram.SetUniformVariable("uWhiteorBlack", uWhiteorBlack);

	glPushMatrix();		
		glTranslatef(-2.2f, -100.20f, Zpos1.GetValue(nowTime));
		glRotatef(330, 1, 2, 0.);
		glRotatef(ThetaY.GetValue(nowTime), 0, -2, 0);
		glScalef(ScaleR.GetValue(nowTime), ScaleR.GetValue(nowTime), ScaleR.GetValue(nowTime));
		//SetMaterial(0.5, 0.5, 0.5, 15);
		glCallList(Starship);
	glPopMatrix();
	RocketProgram.UnUse();


	// draw the Booster object on Earth
	//uWhiteorBlack = 0.0f;
	//uWhiteorRed = 0.7f;

	RocketProgram.Use();
	glActiveTexture(GL_TEXTURE0 + ReflectUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	glActiveTexture(GL_TEXTURE0 + RefractUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	RocketProgram.SetUniformVariable("uMix", Mix);
	RocketProgram.SetUniformVariable("uWhiteMix", uWhiteMix);
	RocketProgram.SetUniformVariable("uRefractUnit", RefractUnit);
	RocketProgram.SetUniformVariable("uReflectUnit", ReflectUnit);
	RocketProgram.SetUniformVariable("uWhiteorRed", uWhiteorRed);
	RocketProgram.SetUniformVariable("uWhiteorBlack", uWhiteorBlack);
	
	glPushMatrix();
	//glEnable(GL_TEXTURE_2D);
		glTranslatef(0.f, Ypos2.GetValue(nowTime), 2.5f);
		glRotatef(90, -1, 0, 0.);
		glScalef(0.1f, 0.1f, 0.1f);
		if (nowTime <= 10)
			SetPointLight(GL_LIGHT1, 0.0, -2.3, 2.5, 1, 0, 0);
		else
			SetPointLight(GL_LIGHT1, 0.0, -2.3, 2.5, 1, 1, 0);
		//SetMaterial(1., 1., 1., 15);
		glCallList( Booster );
		//glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	RocketProgram.UnUse();
	

	// The booster that detaches in space
	//glEnable(GL_TEXTURE_2D);
	RocketProgram.Use();
	glActiveTexture(GL_TEXTURE0 + ReflectUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	glActiveTexture(GL_TEXTURE0 + RefractUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	RocketProgram.SetUniformVariable("uMix", Mix);
	RocketProgram.SetUniformVariable("uWhiteMix", uWhiteMix);
	RocketProgram.SetUniformVariable("uRefractUnit", RefractUnit);
	RocketProgram.SetUniformVariable("uReflectUnit", ReflectUnit);
	RocketProgram.SetUniformVariable("uWhiteorRed", uWhiteorRed);
	RocketProgram.SetUniformVariable("uWhiteorBlack", uWhiteorBlack);
	glPushMatrix();
		glTranslatef(-2.0f, -100.25, 2.0f);
		glRotatef(30, -1, -2, 0.);
		glScalef(ScaleB.GetValue(nowTime), ScaleB.GetValue(nowTime), ScaleB.GetValue(nowTime));
		SetMaterial(1., 1., 1., 15);
		glCallList(Booster);
	glPopMatrix();
	RocketProgram.UnUse();
	//glDisable(GL_TEXTURE_2D);
	
	// Draw the Earth
	//glEnable(GL_TEXTURE_2D);
	EarthProgram.Use();
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, EarthTex);
	EarthProgram.SetUniformVariable("uTexUnit1", 11);
	glPushMatrix();
		glTranslatef(0.0f, -100.f, 0.0f);
		glScalef(1.2f, 1.2f, 1.2f);
		glCallList(EarthDL);
	glPopMatrix();
	EarthProgram.UnUse();
	//glDisable(GL_TEXTURE_2D);
	
	
	// Draw the Moon
	SetSpotLight(GL_LIGHT2, -4., -100., 3.0, 6.f, -100.f, 0.f, 1, 1, 1);
	//glEnable(GL_TEXTURE_2D);
	MoonProgram.Use();
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, MoonTex);
	MoonProgram.SetUniformVariable("uTexUnit1", 12);
	glPushMatrix();
		glTranslatef(5., -100., 1.0);
		glScalef(1.2f, 1.2f, 1.2f);
		SetMaterial(1, 1, 1, 15);
		glCallList(MoonDL);
	glPopMatrix();
	MoonProgram.UnUse();
	//glDisable(GL_TEXTURE_2D);
	
	// Draw the moon landing surface
	SetPointLight(GL_LIGHT3, 2, 106, 1, 1, 1, 1);
	//glEnable(GL_TEXTURE_2D);
	MoonProgram.Use();
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, MoonTex);
	MoonProgram.SetUniformVariable("uTexUnit1", 12);
	glPushMatrix();
		glTranslatef(0, 118, 10);
		glCallList(MoonSurface);
	glPopMatrix();
	MoonProgram.UnUse();
	//glDisable(GL_TEXTURE_2D);
	
	// Draw the rocket landing on the moon
	//uWhiteorBlack = 1.0f;
	//uWhiteorRed = 1.0f;

	RocketProgram.Use();
	glActiveTexture(GL_TEXTURE0 + ReflectUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	glActiveTexture(GL_TEXTURE0 + RefractUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	RocketProgram.SetUniformVariable("uMix", Mix);
	RocketProgram.SetUniformVariable("uWhiteMix", uWhiteMix);
	RocketProgram.SetUniformVariable("uRefractUnit", RefractUnit);
	RocketProgram.SetUniformVariable("uReflectUnit", ReflectUnit);
	
	glPushMatrix();
		glTranslatef(0., Ypos4.GetValue(nowTime), 0.);
		glRotatef(90, -1, 0, 0);
		glScalef(0.1f, 0.1f, 0.1f);
		SetSpotLight(GL_LIGHT4, 0, 106, 2, 0, -1, 0, 1, 1, 1);
		//SetMaterial(0.8, 0.8, 0.8, 15);
		glCallList(Starship);
	glPopMatrix();
	RocketProgram.UnUse();
	
	// explosion
	//glEnable(GL_TEXTURE_2D);
	float uGravity = -0.075;
	float uTime = 0.0;
	float uVelScale = 30.0;
	
	ExplosionProgram.Use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ExplosionTex);
	ExplosionProgram.SetUniformVariable("uTexUnit2", 1);
	ExplosionProgram.SetUniformVariable("uGravity", uGravity);
	ExplosionProgram.SetUniformVariable("uTime", uTime);
	ExplosionProgram.SetUniformVariable("uVelScale", uVelScale);
	glPushMatrix();
		glTranslatef(0.f, Ypos3.GetValue(nowTime), 2.5f);
		glScalef(ScaleEx.GetValue(nowTime), ScaleEx.GetValue(nowTime), ScaleEx.GetValue(nowTime));
		glRotatef(180, 1, 0, 0);
		SetMaterial(1.0, 0.5, 0.0, 3);
		glCallList(Explosion);	
	glPopMatrix();
	ExplosionProgram.UnUse();
	
	//glDisable(GL_TEXTURE_2D);
	
	// Space	
	glPushMatrix();
	SpaceProgram.Use();
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, SpaceTex);
	SpaceProgram.SetUniformVariable("uTexUnit", 5);
		glTranslatef(0.0f, 0.0f, 2.0f);
		glScalef(4.5, 4.5, 4.5);
		glCallList(Space);
	SpaceProgram.UnUse();
	glPopMatrix();

	
	/*
	//landing
	glPushMatrix();
	//float uKa = 0.1f;
	//float uKd = 0.6f;
	//float uKs = 0.3f;
	//float uShininess = 10.0f;
	float uA = 0.0f;
	float uB = 0.6f;
	float uC = 0.0f;
	float uD = 0.0f;
	float uNoiseAmp = 0.0f;
	float uNoiseFreq = 1.0f;
	FloorProgram.Use();
	//FloorProgram.SetUniformVariable("uKa", uKa);
	//FloorProgram.SetUniformVariable("uKd", uKd);
	//FloorProgram.SetUniformVariable("uKs", uKs);
	//FloorProgram.SetUniformVariable("uShininess", uShininess);
	FloorProgram.SetUniformVariable("uA", uA);
	FloorProgram.SetUniformVariable("uB", uB);
	FloorProgram.SetUniformVariable("uC", uC);
	FloorProgram.SetUniformVariable("uD", uD);
	FloorProgram.SetUniformVariable("uNoiseAmp", uNoiseAmp);
	FloorProgram.SetUniformVariable("uNoiseFreq", uNoiseFreq);	
		//glTranslatef(0.0f, 0.0f, 2.0f);
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		glScalef(4.5, 4.5, 4.5);
		glCallList(Space);
	FloorProgram.UnUse();
	glPopMatrix();
	*/
	
	// Draw the Launch Pad
	/*
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	SetMaterial(1, 1, 1, 10);
	glCallList(LaunchPad);
	glPopMatrix();
	*/

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

#ifdef DEMO_Z_FIGHTING
	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.f,   0.f, 1.f, 0.f );
			glCallList( BoxList );
		glPopMatrix( );
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	glDisable( GL_DEPTH_TEST );
	glColor3f( 0.f, 1.f, 1.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1.f, 1.f, 1.f );
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc( Animate );

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:
	/*
	int widths, heights, widthe, heighte, widthm, heightm, widthex, heightex;
	
	char* starshipArray = (char*)"starship.bmp";
	unsigned char* StarshipTexture = BmpToTexture(starshipArray, &widths, &heights);

	char* earthArray = (char*)"Earth.bmp"; 
	unsigned char* Earthtexture = BmpToTexture(earthArray, &widthe, &heighte);
	
	char* moonArray = (char*)"moon.bmp";
	unsigned char* MoonTexture = BmpToTexture(moonArray, &widthm, &heightm);
	
	char* explosionArray = (char*)"explosion.bmp";
	unsigned char* ExplosionTexture = BmpToTexture(explosionArray, &widthex, &heightex);
	*/
	//glGenTextures(1, &StarshipTex);
	glGenTextures(1, &EarthTex);
	glGenTextures(1, &MoonTex);
	glGenTextures(1, &ExplosionTex);
	glGenTextures(1, &SpaceTex);
	glGenTextures(1, &RocketTex);

	// Store the textures bytes in GPU memory
	
	//Shader Stuff CS 457
	//Rocket Shader Init
	glBindTexture(GL_TEXTURE_CUBE_MAP, RocketTex);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	for (int file = 0; file < 6; file++)
	{
		int nums, numt;
		unsigned char* texture2d = BmpToTexture(FaceFiles[file], &nums, &numt);
		if (texture2d == NULL)
			fprintf(stderr, "Could not open BMP 2D texture '%s'", FaceFiles[file]);
		else
			fprintf(stderr, "BMP 2D texture '%s' read -- nums = %d, numt = %d\n", FaceFiles[file], nums, numt);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + file, 0, 3, nums, numt, 0,
			GL_RGB, GL_UNSIGNED_BYTE, texture2d);

		delete[] texture2d;
	}
	RocketProgram.Init();
	bool valid = RocketProgram.Create("rocket.vert", "rocket.frag");
	if (!valid)
		fprintf(stderr, "Could not create the Rocket shader!\n");
	else
		fprintf(stderr, "Rocket shader created!\n");

	//Booster Shader Init
	BoosterS.Init();
	bool valid1 = BoosterS.Create("booster.vert", "booster.frag");
	if (!valid1)
		fprintf(stderr, "Could not create the Booster shader!\n");
	else
		fprintf(stderr, "Booster shader created!\n");

	//Space Texture stuff followed by Initializing space shader
	int nums1, numt1;
	SpaceTexture = BmpToTexture("nvposz.bmp", &nums1, &numt1);
	glBindTexture(GL_TEXTURE_2D, SpaceTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, nums1, numt1, 0, GL_RGB, GL_UNSIGNED_BYTE, SpaceTexture);

	SpaceProgram.Init();
	bool valid2 = SpaceProgram.Create("space.vert", "space.frag");
	if (!valid2)
		fprintf(stderr, "Could not create the Space shader!\n");
	else
		fprintf(stderr, "Space shader created!\n");

	//Earth Texture stuff followed by Earth Shader Init
	int nums2, numt2;
	EarthTexture = BmpToTexture("Earth.bmp", &nums2, &numt2);
	glBindTexture(GL_TEXTURE_2D, EarthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, nums2, numt2, 0, GL_RGB, GL_UNSIGNED_BYTE, EarthTexture);

	EarthProgram.Init();
	bool valid3 = EarthProgram.Create("earth.vert", "earth.frag");
	if (!valid3)
		fprintf(stderr, "Could not create the Earth shader!\n");
	else
		fprintf(stderr, "Earth shader created!\n");

	//Moon Texture stuff followed by Moon Shader Init
	int nums3, numt3;
	MoonTexture = BmpToTexture("moon.bmp", &nums3, &numt3);
	glBindTexture(GL_TEXTURE_2D, MoonTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, nums3, numt3, 0, GL_RGB, GL_UNSIGNED_BYTE, MoonTexture);

	MoonProgram.Init();
	bool valid4 = MoonProgram.Create("earth.vert", "earth.frag");
	if (!valid4)
		fprintf(stderr, "Could not create the Moon shader!\n");
	else
		fprintf(stderr, "Moon shader created!\n");

	//Explosion Texture stuff followed by Explosion Shader Init
	int nums4, numt4;
	ExplosionTexture = BmpToTexture("explosion.bmp", &nums4, &numt4);
	glBindTexture(GL_TEXTURE_2D, ExplosionTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, nums4, numt4, 0, GL_RGB, GL_UNSIGNED_BYTE, ExplosionTexture);
	/*
	ExplosionProgram.Init();
	bool valid5 = ExplosionProgram.Create("explosion.vert", "explosion.geom", "explosion.frag");
	if (!valid5)
		fprintf(stderr, "Could not create the Explosion shader!\n");
	else
		fprintf(stderr, "Explosion shader created!\n");
	*/
	FloorProgram.Init();
	bool valid6 = FloorProgram.Create("pattern1.vert", "pattern.frag");
	if (!valid)
	{
		fprintf(stderr, "Floor program was invalid.\n");
	}
	else
	{
		fprintf(stderr, "Floor Program Shader created!");
	}
	 
	// Starship
	/*
	glBindTexture(GL_TEXTURE_2D, StarshipTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widths, heights, 0, GL_RGB, GL_UNSIGNED_BYTE, StarshipTexture);
	
	// Earth
	glBindTexture(GL_TEXTURE_2D, EarthTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widthe, heighte, 0, GL_RGB, GL_UNSIGNED_BYTE, Earthtexture);
	
	// Moon
	glBindTexture(GL_TEXTURE_2D, MoonTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widthm, heightm, 0, GL_RGB, GL_UNSIGNED_BYTE, MoonTexture);
	
	//Explosion
	glBindTexture(GL_TEXTURE_2D, ExplosionTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widthex, heightex, 0, GL_RGB, GL_UNSIGNED_BYTE, ExplosionTexture);
	*/
	//Keytime animations
	//Starship1 animation
	Ypos1.Init();
	Ypos1.AddTimeValue(0.0, -0.65);
	Ypos1.AddTimeValue(3.0, -0.15);
	Ypos1.AddTimeValue(10.0, 4.35);

	//Booster1 animation
	Ypos2.Init();
	Ypos2.AddTimeValue(0.0, -1.3);
	Ypos2.AddTimeValue(3.0, -0.8);
	Ypos2.AddTimeValue(10.0, 4.0);
	Ypos2.AddTimeValue(20.0, 3.7);
	Ypos2.AddTimeValue(27.0, -0.8);
	Ypos2.AddTimeValue(30.0, -1.3);

	// explosion animation
	ScaleEx.Init();
	ScaleEx.AddTimeValue(0.0, 0.01f);
	ScaleEx.AddTimeValue(1.0, 0.05f);
	ScaleEx.AddTimeValue(10.0, 0.2f);

	Ypos3.Init();
	Ypos3.AddTimeValue(0.0, -1.3);
	Ypos3.AddTimeValue(3.0, -0.8);
	Ypos3.AddTimeValue(10.0, 0.7);
	Ypos3.AddTimeValue(15.0, 0.7);
	Ypos3.AddTimeValue(20.0, 20);


	// Scale factor Booster in space
	ScaleB.Init();
	ScaleB.AddTimeValue(10.1, 0.03f);
	ScaleB.AddTimeValue(14.0, 0.05f);
	ScaleB.AddTimeValue(20, 0.0f);

	// Degree change for rocket in space
	ThetaY.Init();
	ThetaY.AddTimeValue(10.1, 330);
	ThetaY.AddTimeValue(15.0, 270);
	ThetaY.AddTimeValue(20.0, 240);

	// Scale the rocket in space
	ScaleR.Init();
	ScaleR.AddTimeValue(10.1, 0.03f);
	ScaleR.AddTimeValue(14.0, 0.05f);
	ScaleR.AddTimeValue(15.1, 0.04f);
	ScaleR.AddTimeValue(20.0, 0.01f);

	// move the rocket in space
	Zpos1.Init();
	Zpos1.AddTimeValue(10.1, 2.2f);
	Zpos1.AddTimeValue(15.1, 2.3f);
	Zpos1.AddTimeValue(20.0, 2.6f);

	// look at keytimes
	Iposx.Init();
	Iposx.AddTimeValue(0.0, 1.);
	Iposx.AddTimeValue(10.0, 1.);
	Iposx.AddTimeValue(10.1, -4.0);
	Iposx.AddTimeValue(20.0, -4.0);

	Iposy.Init();
	Iposy.AddTimeValue(0.0, -1.0);
	Iposy.AddTimeValue(10.0, -1.0);
	Iposy.AddTimeValue(10.1, -10.0);
	Iposy.AddTimeValue(20.0, -10.0);

	Iposz.Init();
	Iposz.AddTimeValue(0.0, 5.0);
	Iposz.AddTimeValue(10.0, 5.0);
	Iposz.AddTimeValue(10.1, 3.0);
	Iposz.AddTimeValue(20.0, 3.0);

	Iatx.Init();
	Iatx.AddTimeValue(0.0, 0.0);
	Iatx.AddTimeValue(10.0, 0.0);
	Iatx.AddTimeValue(10.1, 6.0);
	Iatx.AddTimeValue(20.0, 6.0);

	Iaty.Init();
	Iaty.AddTimeValue(0.0, 1.0);
	Iaty.AddTimeValue(10.0, 1.0);
	Iaty.AddTimeValue(10.1, -10.0);
	Iaty.AddTimeValue(20.0, -10.0);

	// Land the Starship on the moon
	Ypos4.Init();
	Ypos4.AddTimeValue(30.0, 105.);
	Ypos4.AddTimeValue(35.0, 100.0);
	Ypos4.AddTimeValue(40.0, 99.0);
}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	
	glutSetWindow( MainWindow );

	// create the earth and moon:
	
	SphereDL = glGenLists(1);
	glNewList(SphereDL, GL_COMPILE);
		glColor3f(1, 1, 1);
		OsuSphere(1., 200, 200);
	glEndList();

	EarthDL = glGenLists(1);
	glNewList(EarthDL, GL_COMPILE);
		//glBindTexture(GL_TEXTURE_2D, EarthTex);
		glPushMatrix();
			glScalef(1.0f, 1.0f, 1.0f);
			glCallList(SphereDL);
		glPopMatrix();
	glEndList();

	MoonDL = glGenLists(1);
	glNewList(MoonDL, GL_COMPILE);
		//glBindTexture(GL_TEXTURE_2D, MoonTex);
		glPushMatrix();
			glScalef(1.0f, 1.0f, 1.0f);
			glCallList(SphereDL);
		glPopMatrix();
	glEndList();

	// load the obj files:

	//Starship
	Starship = glGenLists(1);
	glNewList(Starship, GL_COMPILE);
		LoadObjFile("Starship.obj");
	glEndList();

	// Rocket Booster
	Booster = glGenLists(1);
	glNewList(Booster, GL_COMPILE);
		//glBindTexture(GL_TEXTURE_2D, StarshipTex);
		LoadObjFile("SuperHeavy.obj");
	glEndList();

	//Launch Pad
	/*
	LaunchPad = glGenLists(1);
	glNewList(LaunchPad, GL_COMPILE);
	glScalef(0.1f, 0.1f, 0.1f);
		LoadObjFile("pad.obj");
	glEndList();
	*/

	// MoonLanding
	MoonSurface = glGenLists(1);
	glNewList(MoonSurface, GL_COMPILE);
		LoadObjFile("moonSurface.obj");
		//glBindTexture(GL_TEXTURE_2D, MoonTex);
	glEndList();

	// Explosion
	Explosion = glGenLists(1);
	glNewList(Explosion, GL_COMPILE);
		LoadObjFile("explosion.obj");
		//glBindTexture(GL_TEXTURE_3D, ExplosionTex);
	glEndList();

	//Space background
	Space = glGenLists(1);
	glNewList(Space, GL_COMPILE);
	float xmin = -1.f;	// set this to what you want it to be
	float xmax = 1.f;	// set this to what you want it to be
	float ymin = -1.f;	// set this to what you want it to be
	float ymax = 1.f;	// set this to what you want it to be
	float dx = xmax - xmin;
	float dy = ymax - ymin;
	float z = 0.f;		// set this to what you want it to be
	int numy = 128;		// set this to what you want it to be
	int numx = 128;		// set this to what you want it to be
	for (int iy = 0; iy < numy; iy++)
	{
		glBegin(GL_QUAD_STRIP);
		glNormal3f(0., 0., 1.);
		for (int ix = 0; ix <= numx; ix++)
		{
			glTexCoord2f((float)ix / (float)numx, (float)(iy + 0) / (float)numy);
			glVertex3f(xmin + dx * (float)ix / (float)numx, ymin + dy * (float)(iy + 0) / (float)numy, z);
			glTexCoord2f((float)ix / (float)numx, (float)(iy + 1) / (float)numy);
			glVertex3f(xmin + dx * (float)ix / (float)numx, ymin + dy * (float)(iy + 1) / (float)numy, z);
		}
		glEnd();
	}
	glEndList();

	// create the axes:
		
	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			NowProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			NowProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
