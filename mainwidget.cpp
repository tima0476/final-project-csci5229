/****************************************************************************
**
** Adapted from "cube" example code from the Qt library
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>

#include <math.h>
#ifdef DEBUG_GEOM
#include <iostream>
using namespace std;
#endif // DEBUG_GEOM

MainWidget::MainWidget(QWidget *parent) : QOpenGLWidget(parent),
                                          geometries(0),
                                          skyTexture(NULL), landTexture(NULL), waterTexture(NULL),
                                          viewerPos(WORLD_DIM - 1.0f, 0, WORLD_DIM - 1.0f),
                                          lookDir(-0.707106781, 0.0f, -0.707106781),
                                          th(225.0f), ph(0.0f) // Default looking at sun (to show off the water's specular spot)
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

QSize MainWidget::minimumSizeHint() const
{
    return QSize(400, 225);
}

QSize MainWidget::sizeHint() const
{
    return QSize(1200, 675);
}

void MainWidget::keyPressEvent(QKeyEvent *e)
{
    QVector2D mvDir(lookDir.x(),lookDir.z());
    mvDir.normalize(); // Move a fixed amount, even if user is starting at the sky or the ground
    mvDir *= MOVE_AMT;

    switch (e->key())
    {
    case Qt::Key_W:
        // Move forward
        geometries->move(viewerPos, mvDir);
        break;

    case Qt::Key_S:
    case Qt::Key_X:
        // Move backwards
        geometries->move(viewerPos, -mvDir);
        break;

    case Qt::Key_A:
        // Move left
        geometries->move(viewerPos, QVector2D(mvDir.y(), -mvDir.x()));
        break;

    case Qt::Key_D:
        // Move right;
        geometries->move(viewerPos, QVector2D(-mvDir.y(), mvDir.x()));
        break;
    
    case Qt::Key_Q:
        // Move diagonal fwd-left
        geometries->move(viewerPos, QVector2D( (mvDir.x()+mvDir.y())/2, (mvDir.y()-mvDir.x()/2)));
        break;
    
    case Qt::Key_E:
        // Move diagonal fwd-right
        geometries->move(viewerPos, QVector2D( (mvDir.x()-mvDir.y())/2, (mvDir.y()+mvDir.x()/2)));
        break;

    case Qt::Key_Z:
        // Move diagonal back-left
        geometries->move(viewerPos, QVector2D( (-mvDir.x()+mvDir.y())/2, (-mvDir.y()-mvDir.x()/2)));
        break;
    
    case Qt::Key_C:
        // Move diagonal back-right
        geometries->move(viewerPos, QVector2D( (-mvDir.x()-mvDir.y())/2, (-mvDir.y()+mvDir.x()/2)));
        break;

    case Qt::Key_Escape:

        // exit application
        close();
    }
    update();

    // Allow the base class to also handle all keypress events
    QOpenGLWidget::keyPressEvent(e);
}

void MainWidget::mouseMoveEvent(QMouseEvent *e)
{
    // 1 mouse pixel of L/R movement = 1 degree of theta rotation (about y axis)
    th -= (e->localPos().x() - mouseLastPosition.x()) / 3.0f;
    ph -= (e->localPos().y() - mouseLastPosition.y()) / 3.0f;

    th = fmod(th, 360.0f); // bound th within -360 to 360 degrees
    ph = fmod(ph, 360.0f); // bound ph within -360 to 360 degrees

    // Update the direction we're looking for the new rotation.  I'm sure there's a way to do this
    // with the Qt quaternion class, but this is straightforward to do it myself.
    lookDir.setX(Sin(th) * Cos(ph));
    lookDir.setY(Sin(ph));
    lookDir.setZ(Cos(th) * Cos(ph));

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

    glClearColor(0.31f, 0.43f, 0.65f, 1); // Sky color sampled from the skybox texture image

    initShaders();
    initTextures();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Instantiate our geometry engine
    geometries = new GeometryEngine;

    //
    // Adjust starting position to be near the lake.
    //
    int retries = 11;
    while (!geometries->adjustViewerPos(viewerPos, QVector2D(-0.707106781, -0.707106781)) && retries--)
    {
        // Lake not found.  Delete this world and make another one.  Oh, the POWER!!!
        delete geometries;
        geometries = new GeometryEngine;
    }

    viewerPos.setY(geometries->getHeight(viewerPos.x(), viewerPos.z()) + EYE_HEIGHT);
}

void MainWidget::initShaders()
{
    // Compile vertex shaders
    if (!skyProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vtexonly.glsl"))
        close();
    if (!mainProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vmain.glsl"))
        close();

    // Compile fragment shaders
    if (!skyProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/ftexonly.glsl"))
        close();
    if (!mainProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fmain.glsl"))
        close();

    // Link shader pipelines
    if (!skyProgram.link())
        close();
    if (!mainProgram.link())
        close();
}

void MainWidget::initTextures()
{
    // Load textures
    skyTexture = new QOpenGLTexture(QImage(":/textures/Sky/2226.png").mirrored());
    landTexture = new QOpenGLTexture(QImage(":/textures/Land/85290912-seamless-tileable-natural-ground-field-texture.jpg").mirrored());
    waterTexture = new QOpenGLTexture(QImage(":/textures/Water/WaterPlain0012_1_270.jpg").mirrored());

    skyTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    landTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    waterTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);

    skyTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    landTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    waterTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    skyTexture->setWrapMode(QOpenGLTexture::Repeat);
    landTexture->setWrapMode(QOpenGLTexture::Repeat);
    waterTexture->setWrapMode(QOpenGLTexture::Repeat);
}

void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio to keep pixels square
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    const qreal zNear = 1.0f / WORLD_DIM, zFar = 3.0f * WORLD_DIM, fov = 55.0;

    // Set perspective projection
    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);
}

void MainWidget::paintGL()
{
    QMatrix4x4 treePos;

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.lookAt(viewerPos, viewerPos + lookDir, QVector3D(0, 1, 0)); // +Y is always up

    // Locate a light source to correspond with the sun in the skybox texture (3/4 up, 3/4 back, on the left face)
    QVector3D lightPos(-WORLD_DIM, WORLD_DIM / 2.0f, -WORLD_DIM / 2.0f);
    lightPos = QVector3D(matrix * lightPos); // transform the light to world coordinates

    // Bind skybox shader pipeline
    if (!skyProgram.bind())
        close();

    // Set modelview-projection matrix
    skyProgram.setUniformValue("mvp_matrix", projection * matrix);

    // Draw the skybox
    skyProgram.setUniformValue("texture", 0);
    skyTexture->bind();
    geometries->drawSkyCubeGeometry(&skyProgram);

    // Bind land & water shader pipeline
    if (!mainProgram.bind())
        close();

    // Set uniforms for the main shader
    mainProgram.setUniformValue("mv_matrix", matrix);
    mainProgram.setUniformValue("mvp_matrix", projection * matrix);
    mainProgram.setUniformValue("normalMatrix", matrix.normalMatrix());
    mainProgram.setUniformValue("lightPosition", lightPos);

    // Draw the water
    waterTexture->bind();
    geometries->drawWaterGeometry(&mainProgram);

    // Draw the land
    mainProgram.setUniformValue("texture", 0);
    landTexture->bind();
    geometries->drawLandGeometry(&mainProgram);

    // Draw all of the trees
    srand(1234.0);     // Cheat to get consistent "random" tree rotations without having to store a rotation angle alongside each tree
    for (int i = 0; i < TREE_COUNT; i++)
    {
        // Set translation matrix for each tree to individually locate and resize them in the world
        treePos = matrix;
        treePos.translate(geometries->treeSpot[i].toVector3D());
        treePos.scale(geometries->treeSpot[i].w(), geometries->treeSpot[i].w(), geometries->treeSpot[i].w());
        treePos.rotate(Frand(360.0),0,1,0);

        mainProgram.setUniformValue("mv_matrix", treePos);
        mainProgram.setUniformValue("mvp_matrix", projection * treePos);

        // Draw a tree
        geometries->drawTreeGeometry(&mainProgram);
    }
}
