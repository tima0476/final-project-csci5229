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
    double x0,y0,z0;  //  Start position 
    int th, ph;       //  Display angles
    bool mouse;       //  Mouse pressed
    QPoint pos;       //  Mouse position
    double dim;       //  Display size
    double asp;       //  Sceen aspect ratio

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
    void project();                        //  Set projection
};

#endif // MAIN_WINDOW_H
