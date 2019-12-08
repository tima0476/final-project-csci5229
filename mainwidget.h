/****************************************************************************
**
** Adapted from "cube" example code from the Qt library
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
****************************************************************************/

#ifndef MAINWIDGET_H
#define MAINWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector2D>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "geometryengine.h"

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415926/180.0))
#define Sin(x) (sin((x)*3.1415926/180.0))

#define EYE_HEIGHT  0.5f
#define MOVE_AMT    0.1f    // amount to move on each keypress

class GeometryEngine;

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

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
    QOpenGLShaderProgram skyProgram, mainProgram, plantProgram;
    GeometryEngine *geometries;

    QOpenGLTexture *skyTexture;
    QOpenGLTexture *landTexture;
    QOpenGLTexture *waterTexture;

    QMatrix4x4 projection;

    QVector2D mouseLastPosition;
    QVector3D viewerPos;
    QVector3D lookDir;
    float th, ph;
};

#endif // MAINWIDGET_H
