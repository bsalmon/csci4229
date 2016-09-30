/*
 * hw4.c
 * Brian Salmon
 * 
 * A simple program that draws 3 trees 
 * and a plane flying by.
 * Heavily relied upon class provided examples
 * Found online github example for first person perspective
 * 
 *  3D Objects
 *
 *  Demonstrates how to draw objects in 3D.
 *
 *  Key bindings:
 *  m/M        Cycle through different sets of objects
 *  a          Toggle axes
 *  arrows     Change view angle
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
double zh=0;      //  Rotation
int fp = 0;       //  First Peson Toggle
int rot = 0.0;    //  Rotation for first person
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world
int axes=0;       //  Display axes
int mode=0;       //  What to display

int flying=1;
int FPS=60;
static double flight=0;

double Ex = 0;
double Ey = 0;
double Ez = 5;
double Cx = 0; 
double Cz = 0;

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}



/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   
   if(fp) {
      gluPerspective(fov,asp,dim/4,4*dim);
   }
   else {
   
	//  Perspective transformation
	if (mode)
      gluPerspective(fov,asp,dim/10,10*dim);
	//  Orthogonal projection
	else
		glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
	}
	//  Switch to manipulating the model matrix
	glMatrixMode(GL_MODELVIEW);
	//  Undo previous transformations
	glLoadIdentity();
}






/*
 *  Draw vertex in polar coordinates
 */


static void Vertex(double th,double ph)
{
   glColor3f(0 , 250 , 0);
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}


/*
 *  Draw a sphere (version 2)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}

static void Tree(double x, double y, double z,
		      double dx, double dy, double dz,
		      double th){
  
  // Save transformation
  glPushMatrix();
  
  // Offset and scale
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);

  // Body of tree
  glBegin(GL_QUADS);
  //Front
  glColor3ub(102,51,0);
  glVertex3f(-.25,-.25, .25);
  glVertex3f(+.25,-.25, .25);
  glVertex3f(+.25,+3, .25);
  glVertex3f(-.25,+3, .25);
  //  Back
  glColor3ub(102,0,0);
  glVertex3f(+.25,-.25,-.25);
  glVertex3f(-.25,-.25,-.25);
  glVertex3f(-.25,+3,-.25);
  glVertex3f(+.25,+3,-.25);
  //  Right
  glColor3ub(204,102,0);
  glVertex3f(+.25,-.25,+.25);
  glVertex3f(+.25,-.25,-.25);
  glVertex3f(+.25,+3,-.25);
  glVertex3f(+.25,+3,+.25);
  //  Left
  glColor3ub(204,102,0);
  glVertex3f(-.25,-.25,-.25);
  glVertex3f(-.25,-.25,+.25);
  glVertex3f(-.25,+3,+.25);
  glVertex3f(-.25,+3,-.25);
  //  Top
  glColor3ub(51,0,0);
  glVertex3f(-.25,+3,+.25);
  glVertex3f(+.25,+3,+.25);
  glVertex3f(+.25,+3,-.25);
  glVertex3f(-.25,+3,-.25);
  //  Bottom
  glColor3ub(51,0,0);
  glVertex3f(-.25,-.25,-.25);
  glVertex3f(+.25,-.25,-.25);
  glVertex3f(+.25,-.25,+.25);
  glVertex3f(-.25,-.25,+.25);
  glEnd();

  // Top of tree:
  
  sphere(0, 4, 0, 1.1);
}


static void SolidPlane(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz)
{
   // Dimensions used to size airplane
   const double wid=0.05;
   const double nose=+0.50;
   const double cone= 0.20;
   const double wing= 0.00;
   const double strk=-0.20;
   const double tail=-0.50;
   //  Unit vector in direction of flght
   double D0 = sqrt(dx*dx+dy*dy+dz*dz);
   double X0 = dx/D0;
   double Y0 = dy/D0;
   double Z0 = dz/D0;
   //  Unit vector in "up" direction
   double D1 = sqrt(ux*ux+uy*uy+uz*uz);
   double X1 = ux/D1;
   double Y1 = uy/D1;
   double Z1 = uz/D1;
   //  Cross product gives the third vector
   double X2 = Y0*Z1-Y1*Z0;
   double Y2 = Z0*X1-Z1*X0;
   double Z2 = X0*Y1-X1*Y0;
   //  Rotation matrix
   double mat[16];
   mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
   mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
   mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
   mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

   //  Save current transforms
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glMultMatrixd(mat);
   //  Nose (4 sided)
   glColor3f(0,0,1);
   glBegin(GL_TRIANGLES);
   glVertex3d(nose, 0.0, 0.0);
   glVertex3d(cone, wid, wid);
   glVertex3d(cone,-wid, wid);

   glVertex3d(nose, 0.0, 0.0);
   glVertex3d(cone, wid,-wid);
   glVertex3d(cone,-wid,-wid);

   glVertex3d(nose, 0.0, 0.0);
   glVertex3d(cone, wid, wid);
   glVertex3d(cone, wid,-wid);

   glVertex3d(nose, 0.0, 0.0);
   glVertex3d(cone,-wid, wid);
   glVertex3d(cone,-wid,-wid);
   glEnd();
   //  Fuselage (square tube)
   glBegin(GL_QUADS);
   glVertex3d(cone, wid, wid);
   glVertex3d(cone,-wid, wid);
   glVertex3d(tail,-wid, wid);
   glVertex3d(tail, wid, wid);

   glVertex3d(cone, wid,-wid);
   glVertex3d(cone,-wid,-wid);
   glVertex3d(tail,-wid,-wid);
   glVertex3d(tail, wid,-wid);

   glVertex3d(cone, wid, wid);
   glVertex3d(cone, wid,-wid);
   glVertex3d(tail, wid,-wid);
   glVertex3d(tail, wid, wid);

   glVertex3d(cone,-wid, wid);
   glVertex3d(cone,-wid,-wid);
   glVertex3d(tail,-wid,-wid);
   glVertex3d(tail,-wid, wid);

   glVertex3d(tail,-wid, wid);
   glVertex3d(tail, wid, wid);
   glVertex3d(tail, wid,-wid);
   glVertex3d(tail,-wid,-wid);
   glEnd();
   //  Wings (plane triangles)
   glColor3f(1,1,0);
   glBegin(GL_TRIANGLES);
   glVertex3d(wing, 0.0, wid);
   glVertex3d(tail, 0.0, wid);
   glVertex3d(tail, 0.0, 0.5);

   glVertex3d(wing, 0.0,-wid);
   glVertex3d(tail, 0.0,-wid);
   glVertex3d(tail, 0.0,-0.5);
   glEnd();
   //  Vertical tail (plane triangle)
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3d(strk, 0.0, 0.0);
   glVertex3d(tail, 0.3, 0.0);
   glVertex3d(tail, 0.0, 0.0);
   glEnd();
   //  Undo transformations
   glPopMatrix();
}


void timer(int v) {
  if (flying) {
    flight += .1;
    if (flight > 13.0) {
      flight -= 13.0;
    }
    glutPostRedisplay();
  }
  glutTimerFunc(1000/FPS, timer, v);

}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   
   
   if (fp) {
      Cx = +2*dim*Sin(rot); // Change camera dimensions 
      Cz = -2*dim*Cos(rot);

      gluLookAt(Ex,Ey,Ez, Cx+Ex,Ey,Cz+Ez, 0,1,0); //  Use gluLookAt
  }
  else
      {
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
  }
   
   Tree(0,0,2 , 1,1,1, 0);
   Tree(3,0,4 , 1.5,1.5,1.5 , -30);
   Tree(-3,0,2 , 1,1,1 , 0);
   SolidPlane(3-flight,6,2,  -1,0,0, 0, 1,0);
   
   
   glEnd();
   
   
   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);
   //  Print the text string
   if(fp){
	   Print("First Person,  Angle=%d",rot);
   }
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
      
   //  Toggle first person
   else if (ch == 'f' || ch == 'F')
   {
      fp = 1-fp;
   }

   if (fp) {
      double dt = 0.05;
      if (ch == 'w' || ch == 'W'){
         Ex += Cx*dt;
         Ez += Cz*dt;
      }
      else if (ch == 'a' || ch == 'A'){
         rot -= 3;
      }
      else if (ch == 's' || ch == 'S'){
         Ex -= Cx*dt;
         Ez -= Cz*dt;
      }
      else if (ch == 'd' || ch == 'D'){
         rot += 3;
      }
      
      //  Keep angles to +/-360 degrees
      rot %= 360;
   }
   else {
      //  Reset view angle
      if (ch == '0')
         th = ph = 0;
      //  Switch display mode
      else if (ch == 'm' || ch == 'M')
         mode = 1-mode;
   }
   
   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}



/*
 *  GLUT calls this toutine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Homework 4");
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call timer function
   glutTimerFunc(100, timer, 0);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
