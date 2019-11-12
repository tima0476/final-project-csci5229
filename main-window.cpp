//
//  OpenGL MainWindow Widget
//
#define GL_SILENCE_DEPRECATION

#include <QtOpenGL>
#include "main-window.h"

//
//  Constructor
//
MainWindow::MainWindow(QWidget *parent) : QGLWidget(parent)
{
   util u;              // Create an instance of the utility class
   th = ph = 30;        //  Set intial display angles
   asp = 1;             //  Aspect ratio
   dim = 50;            //  World dimension
   mouse = 0;           //  Mouse movement

   // Load the sky textures
   sky[0] = u.load_tex_bmp("textures/sky0.bmp");
   sky[1] = u.load_tex_bmp("textures/sky1.bmp");

   // Initialize the first person perspective
   Ux = 25;             //  Up
   Uy = 0;              //  Up
   Uz = 0;              //  Up
   Ox = 0;              //  LookAt
   Oy = 0;              //  LookAt
   Oz = 0;              //  LookAt
   Ex = 25;             //  Eye
   Ey = 25;             //  Eye
   Ez = 52;             //  Eye
}

/********************************************************************/
/*************************  Set parameters  *************************/
/********************************************************************/

//
//  Set dim
//
void MainWindow::setDIM(double DIM)
{
   dim = DIM;    //  Set parameter
   project();
   update();     //  Request redisplay
}

//
//  Reset view angle
//
void MainWindow::reset(void)
{
   th = ph = 0;  //  Set parameter
   update();     //  Request redisplay
}

/******************************************************************/
/*************************  Mouse Events  *************************/
/******************************************************************/
//
//  Mouse pressed
//
void MainWindow::mousePressEvent(QMouseEvent* e)
{
   mouse = true;
   pos = e->pos();  //  Remember mouse location
}

//
//  Mouse released
//
void MainWindow::mouseReleaseEvent(QMouseEvent*)
{
    mouse = false;
}

//
//  Mouse moved
//
void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
   if (mouse)
   {
      QPoint d = e->pos()-pos;  //  Change in mouse location
      th = (th+d.x())%360;      //  Translate x movement to azimuth
      ph = (ph+d.y())%360;      //  Translate y movement to elevation
      pos = e->pos();           //  Remember new location
      update();                 //  Request redisplay
   }
}

//
//  Mouse wheel
//
void MainWindow::wheelEvent(QWheelEvent* e)
{
   //  Zoom out
   if (e->delta()<0)
      setDIM(dim+1);
   //  Zoom in
   else if (dim>2)
      setDIM(dim-1);
   //  Signal to change dimension spinbox
   emit dimen(dim);
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/

//
//  Initialize
//
void MainWindow::initializeGL()
{
   glEnable(GL_DEPTH_TEST); //  Enable Z-buffer depth testing
   setMouseTracking(true);  //  Ask for mouse events
}

//
//  Window is resized
//
void MainWindow::resizeGL(int width, int height)
{
   //  Window aspect ration
   asp = (width && height) ? width / (float)height : 1;
   
   //  Viewport is whole screen
   glViewport(0,0,width,height);
   
   //  Set projection
   project();
}
// 
// Draw sky box
// 
void MainWindow::Sky(double D)
{
   glColor3f(1,0,0);
   glEnable(GL_TEXTURE_2D);

   //  Sides
   glBindTexture(GL_TEXTURE_2D,sky[0]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.00,1); glVertex3f(-D,+D,-D);

   glColor3f(0,1,0);
   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);

   glColor3f(0,0,1);
   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);

   glColor3f(1,1,0);
   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1.00,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glEnd();

   //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,sky[1]);
   glBegin(GL_QUADS);
   glColor3f(0,1,1);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);

   glColor3f(1,0,1);
   glTexCoord2f(1.0,1); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.5,1); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.5,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(1.0,0); glVertex3f(-D,-D,-D);
   glEnd();

   glDisable(GL_TEXTURE_2D);
}

//
//  Draw the window
//
void MainWindow::paintGL()
{
   //  Clear screen and Z-buffer
   glClearColor(0,0.3,0.7,0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);

   //  Reset transformations
   glLoadIdentity();
   
   //  Perspective - set eye position
   gluLookAt(Ex,Ey,Ez , Ox,Oy,Oz , Ux,Uy,Uz);

   // Draw the scene
   Sky(3.5 * dim);

   //
   //  Draw Axes
   //
   double len = 25;
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(len,0.0,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,len,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,0.0,len);
   glEnd();
   //  Label Axes
   renderText(len,0.0,0.0,"X");
   renderText(0.0,len,0.0,"Y");
   renderText(0.0,0.0,len,"Z");

   //  Done
   u.err_check("display");
   glFlush();
}

//
//  Set projection
//
void MainWindow::project()
{
   //  Orthogonal projection to dim
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPerspective(55, asp, dim/64, dim*64);

   //  Back to model view
   glMatrixMode(GL_MODELVIEW);
}
