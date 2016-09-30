/*
 * Simple program to demonstrate generating coordinates
 * using the Lorenz Attractor
 * Heavily relied upon class provided examples
 * Also used online examples for comparison
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

/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;

int th=0;  //  Azimuth of view angle
int ph=0;  //  Elevation of view angle

double pa[50000][3];


/*
 * Convenience function for text
 */
#define LEN 8192  //  Maximum amount of text
void Print(const char* format , ...)
{
   char    buf[LEN]; // Text storage
   char*   ch=buf;   // Text pointer
   //  Create text to be display
   va_list args;
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display text string
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}




void lorenz(void){
	int i;
   /*  Coordinates  */
   double x = 1;
   double y = 1;
   double z = 1;
   /*  Time step  */
   double dt = 0.001;

   //printf("%5d %8.3f %8.3f %8.3f\n",0,x,y,z);
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0;i<50000;i++)
   {
      double dx = s*(y-x);
      double dy = x*(r-z)-y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;
      pa[i][0] = 0.02*x;
	  pa[i][1] = 0.02*y;
      pa[i][2] = 0.02*z;
      //printf("%5d %8.3f %8.3f %8.3f\n",i+1,x,y,z);
   }
	
}

void display(void){
	
	lorenz();
	
	//Clear screen and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//  Reset transformations
	glLoadIdentity();
	
	//-----------------
	//  Set view angle
  glRotated(ph,1,0,0);
  glRotated(th,0,1,0);
  int i;
  glBegin(GL_LINE_STRIP);
  for (i=0;i<50000;i++)
  {
    glColor3dv(pa[i]-1);
    glVertex3dv(pa[i]);
  }
  glEnd();
  //  Draw axes in white
  glColor3f(1,1,1);
  glBegin(GL_LINES);
  glVertex3d(0,0,0);
  glVertex3d(1,0,0);
  glVertex3d(0,0,0);
  glVertex3d(0,1,0);
  glVertex3d(0,0,0);
  glVertex3d(0,0,1);
  glEnd();
  //  Label axes
  glRasterPos3d(1,0,0);
  Print("X");
  glRasterPos3d(0,1,0);
  Print("Y");
  glRasterPos3d(0,0,1);
  Print("Z");
  //  Display parameters
  glWindowPos2i(5,5);
  Print("View Angle=%d,%d",th,ph);
	//-----------------
	//  Flush and swap buffer
	glFlush();
	glutSwapBuffers();
}

/*
 * This function is called by GLUT when the window is resized
 */
void reshape(int width,int height)
{
   //  Calculate width to height ratio
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set viewport as entire window
   glViewport(0,0, width,height);
   //  Select projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Set projection to identity
   glLoadIdentity();
   //  Orthogonal projection:  unit cube adjusted for aspect ratio
   glOrtho(-w2h,+w2h, -1.0,+1.0, -1.0,+1.0);
   //  Select model view matrix
   glMatrixMode(GL_MODELVIEW);
   //  Set model view to identity
   glLoadIdentity();
}

void special(int key,int x,int y)
{
   //  Right arrow - increase rotation by 5 degree
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow - decrease rotation by 5 degree
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   //  Request display update
   glutPostRedisplay();
   
}


void key(unsigned char ch,int x,int y)
{	
	//increase sigma
	if (ch == 'a' || ch == 'A')
	s += 1;
	//decrease sigma
	else if (ch == 'z' || ch == 'Z')
	s -= 1;
	
	//increase beta
	if (ch == 's' || ch == 'S')
	b += 1;
	//decrease beta
	else if (ch == 'x' || ch == 'X')
	b -= 1;
	
	//increase r
	if (ch == 'd' || ch == 'D')
	r += 1;
	//decrease r
	else if (ch == 'c' || ch == 'C')
	r -= 1;
	
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}
/*
 *  Main
 */
int main(int argc, char *argv[])
{
	
	glutInit(&argc,argv);
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);
	
	glutCreateWindow("Lorenz Attractor");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(key);
	//glutIdleFunc(idle);
	
	glutMainLoop();
   
   
   return 0;
}
