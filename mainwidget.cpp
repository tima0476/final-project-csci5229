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
    viewerPos(0,0,0),
    lookDir(0,0,-1),
    th(0.0f), ph(0.0f)
{
    // Disable mouse tracking - mousepos events will only fire when left mouse button pressed
    setMouseTracking(false);

    //  Set window title
    setWindowTitle("meadow - Timothy Mason's final project");
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the skyTexture
    // and the buffers.
    makeCurrent();
    delete skyTexture;
    delete landTexture;
    delete geometries;
    doneCurrent();
}

void MainWidget::keyPressEvent(QKeyEvent *e)
{
    QVector3D mvDir(lookDir);
    
    mvDir.setY(0);              // For this sim, we are locked to the ground.  :-(
    // To do:  Instead of squashing Y, point the move vector towards the appropriate (interpolated) point at eye height above
    // the nearest terrain edge in the direction we're looking
    mvDir.normalize();          // Move a fixed amount, even if user is starting at the sky or the ground
    mvDir *= MOVE_AMT;

    switch (e->key()) {
        case Qt::Key_W:
            // Move forward
            viewerPos += mvDir;
 
            // Maintain a fixed eye height above the ground
            viewerPos.setY( geometries->getHeight(viewerPos.x(), viewerPos.z())+EYE_HEIGHT);
            update();
            break;

        case Qt::Key_S:
            // Move backwards
            viewerPos -= mvDir;
 
            // Maintain a fixed eye height above the ground
            viewerPos.setY( geometries->getHeight(viewerPos.x(), viewerPos.z())+EYE_HEIGHT);
 
            update();
            break;

        default:
            // VERY IMPORTANT!  Pass any key events we didn't handle to the base class handler
            QOpenGLWidget::keyPressEvent(e);
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent *e)
{
    // 1 mouse pixel of L/R movement = 1 degree of theta rotation (about y axis)
    th -= e->localPos().x() - mouseLastPosition.x();
    ph -= e->localPos().y() - mouseLastPosition.y();

    th = fmod(th, 360.0f);  // bound th within -360 to 360 degrees
    ph = fmod(ph, 360.0f);  // bound ph within -360 to 360 degrees
        
    // Update the direction we're looking for the new rotation.  I'm sure there's a way to do this
    // with the Qt quaternion class, but this is straightforward to do it myself.
    lookDir.setX(-Sin(th)*Cos(ph));
    lookDir.setY(Sin(ph));
    lookDir.setZ(-Cos(th)*Cos(ph));

    // Save the current mouse position so we can calculate a delta on the next mouse move
    mouseLastPosition = QVector2D(e->localPos());

    // Redraw the scene with the new view
    update();
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Start of a mouse move - save the initial mouse position for delta calculation in mouseMoveEvent
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

    // Instantiate our geometry engine
    geometries = new GeometryEngine;

    // land terrain was generated in the geometry constructor.  Set our eye initial height based on the terrain
    viewerPos.setY( geometries->getHeight(viewerPos.x(), viewerPos.z())+EYE_HEIGHT);
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
    // Load sky texture.  Assumed to be of "cross" layout
    skyTexture = new QOpenGLTexture(QImage(":/textures/2226.webp").mirrored());
    landTexture = new QOpenGLTexture(QImage(":/textures/tileable-img_0062-verydark.png").mirrored());

    // Set nearest filtering mode for texture minification
    skyTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    landTexture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    skyTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    landTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    skyTexture->setWrapMode(QOpenGLTexture::Repeat);
    landTexture->setWrapMode(QOpenGLTexture::Repeat);
}



void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio to keep pixels square
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3/16, far plane to 3*16, field of view 55 degrees
    const qreal zNear = 0.1, zFar = 60.0, fov = 55.0;

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

    matrix.lookAt(viewerPos, viewerPos+lookDir, QVector3D(0,1,0));

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);

    // Draw the land
    program.setUniformValue("texture", 0);
    landTexture->bind();
    geometries->drawLandGeometry(&program);

    // Draw the skybox
    program.setUniformValue("texture", 0);
    skyTexture->bind();
    geometries->drawSkyCubeGeometry(&program);
}

