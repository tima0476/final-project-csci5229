//
//  OpenGL MainWindow Widget
//
#include <QtOpenGL>
#include <main-window.h>

//
//  Constructor
//
MainWindow::MainWindow(QWidget *parent) 
    : QGLWidget(parent)
{
   th = ph = 30;      //  Set intial display angles
   asp = 1;           //  Aspect ratio
   dim = 50;          //  World dimension
   x0 = y0 = z0 = 1;  //  Starting location
   mouse = 0;         //  Mouse movement
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
//  Draw the window
//
void MainWindow::paintGL()
{
   //  Clear screen and Z-buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //  Reset transformations
   glLoadIdentity();
   
   //  Set rotation
   glRotated(ph , 1,0,0);
   glRotated(th , 0,1,0);

   // TODO: Draw the scene


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

   //
   //  Emit signal with display angles and dimensions
   //
   // emit angles("th,ph= "+QString::number(th)+","+QString::number(ph));

   //  Done
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
   if (asp>1)
      glOrtho(-dim*asp, +dim*asp, -dim, +dim, -3*dim, +3*dim);
   else
      glOrtho(-dim, +dim, -dim/asp, +dim/asp, -3*dim, +3*dim);

   //  Back to model view
   glMatrixMode(GL_MODELVIEW);
}
