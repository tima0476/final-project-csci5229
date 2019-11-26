/****************************************************************************
**
** Adapted from "cube" example code from the Qt library
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
****************************************************************************/


#include "mainwidget.h"

#include <QMouseEvent>

#include <math.h>

MainWidget::MainWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    skyTexture(NULL),
    eyePosition(0,0,0),
    lookDir(0,0,-1),
    th(180.0f)
{
    // Disable mouse tracking - mousepos events will only fire when left mouse button pressed
    setMouseTracking(false);

    //  Set window title
    setWindowTitle("meadow - Timothy Mason");
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the skyTexture
    // and the buffers.
    makeCurrent();
    delete skyTexture;
    delete geometries;
    doneCurrent();
}

void MainWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
        case Qt::Key_W:
            // Move forward
            eyePosition += lookDir/10.0;

            // OpenGL Redraw
            update();
            break;

        case Qt::Key_S:
            // Move forward
            eyePosition -= lookDir/10.0;

            // OpenGL Redraw
            update();
            break;

        default:
            QOpenGLWidget::keyPressEvent(e);
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent *e)
{
    // 1 mouse pixel of L/R movement = 1 degree of theta rotation
    th += e->localPos().x() - mouseLastPosition.x();
    th = fmod(th, 360.0f);
        
    // Update the direction we're looking for the new rotation
    lookDir.setX(Sin(-th));
    lookDir.setY(0.0f);
    lookDir.setZ(Cos(-th));


    // Save the current mouse position so we can calculate a delta on the next mouse move
    mouseLastPosition = QVector2D(e->localPos());

    // Request an OpenGL update
    update();
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Start of a mouse move - save initial mouse position
    mouseLastPosition = QVector2D(e->localPos());
}


void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0,0,0,1);

    initShaders();
    initTextures();


    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);


    geometries = new GeometryEngine;
}


void MainWidget::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}



void MainWidget::initTextures()
{
    // Load cube.png image
    // skyTexture = new QOpenGLTexture(QImage(":/textures/2226.webp").mirrored());                    // Mountain swampy
    skyTexture = new QOpenGLTexture(QImage(":/textures/2374281533.jpeg").mirrored());                 // Mountain Lake
    // skyTexture = new QOpenGLTexture(QImage(":/textures/DEBUG 2374281533.webp").mirrored());                 // Mountain Lake
    // skyTexture = new QOpenGLTexture(QImage(":/textures/2721839643.jpg").mirrored());           // Rocky Mountainous Desert
    // skyTexture = new QOpenGLTexture(QImage(":/textures/skyboxsun25degtest.png").mirrored());       // Flat, arid desert
    // skyTexture = new QOpenGLTexture(QImage(":/textures/stormydays_large.jpg").mirrored());             // Golden sunset
    // skyTexture = new QOpenGLTexture(QImage(":/textures/violentdays_large.jpg").mirrored());        // Fiery sunset

    // Set nearest filtering mode for texture minification
    skyTexture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    skyTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    skyTexture->setWrapMode(QOpenGLTexture::Repeat);
}



void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio to keep pixels square
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3/16, far plane to 3*16, field of view 45 degrees
    const qreal zNear = 3.0/16.0, zFar = 3.0*16.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}


void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 matrix;

    matrix.lookAt(eyePosition, eyePosition+lookDir, QVector3D(0,1,0));

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);


    // Use texture unit 0
    program.setUniformValue("texture", 0);

    skyTexture->bind();

    // Draw cube geometry
    geometries->drawSkyCubeGeometry(&program);
}
