//
//  OpenGL MainWindow Widget
//

#ifndef MAIN_WINDOW_H

#include <QGLWidget>
#include <QString>

class MainWindow : public QGLWidget
{
Q_OBJECT //  Qt magic macro

private: 
    bool mouse;     //  Mouse pressed
    int th, ph;     //  Display angles
    QPoint pos;     //  Mouse position
    double dim;     //  Display size
    double asp;     //  Sceen aspect ratio
    int sky[2];     //  Sky textures
    double Ux;      //  Up
    double Uy;      //  Up
    double Uz;      //  Up
    double Ox;      //  LookAt
    double Oy;      //  LookAt
    double Oz;      //  LookAt
    double Ex;      //  Eye
    double Ey;      //  Eye
    double Ez;      //  Eye

public:
   MainWindow(QWidget* parent=0);                        //  Constructor
   QSize sizeHint() const {return QSize(400,400);}   //  Default size of widget

public slots:
    void setDIM(double DIM); //  Slot to set dim
    void reset(void);        //  Reset view angles

signals:
    void dimen(double dim);     //  Signal for display dimensions

protected:
    void initializeGL();                   //  Initialize widget
    void resizeGL(int width, int height);  //  Resize widget
    void paintGL();                        //  Draw widget
    void mousePressEvent(QMouseEvent*);    //  Mouse pressed
    void mouseReleaseEvent(QMouseEvent*);  //  Mouse released
    void mouseMoveEvent(QMouseEvent*);     //  Mouse moved
    void wheelEvent(QWheelEvent*);         //  Mouse wheel

private:
    void Sky(double D);                     // Draw skybox
    void project();                        //  Set projection
};

#endif // MAIN_WINDOW_H
