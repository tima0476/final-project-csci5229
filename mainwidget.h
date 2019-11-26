/****************************************************************************
**
** Adapted from "cube" example code from the Qt library
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
****************************************************************************/

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "geometryengine.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415926/180.0))
#define Sin(x) (sin((x)*3.1415926/180.0))

class GeometryEngine;

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void initShaders();
    void initTextures();

private:
    QOpenGLShaderProgram program;
    GeometryEngine *geometries;

    QOpenGLTexture *skyTexture;

    QMatrix4x4 projection;

    QVector2D mouseLastPosition;
    QVector3D eyePosition;
    QVector3D lookDir;
    float th;
};

#endif // MAINWIDGET_H
