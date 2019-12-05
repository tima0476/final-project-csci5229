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
                                          viewerPos(-WORLD_DIM / 2, 0, WORLD_DIM / 2),
                                          lookDir(0.7f, 0.0f, -0.7f),
                                          th(315.0f), ph(0.0f)
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

    mvDir.setY(0);     // For this sim, we are locked to the ground.
    mvDir.normalize(); // Move a fixed amount, even if user is starting at the sky or the ground
    mvDir *= MOVE_AMT;

    switch (e->key())
    {
    case Qt::Key_W:
        // Move forward
        viewerPos += mvDir;
        break;

    case Qt::Key_S:
        // Move backwards
        viewerPos -= mvDir;
        break;

    case Qt::Key_A:
        // Move left
        viewerPos.setX(viewerPos.x() + mvDir.z());
        viewerPos.setZ(viewerPos.z() - mvDir.x());
        break;

    case Qt::Key_D:
        // Move right;
        viewerPos.setX(viewerPos.x() - mvDir.z());
        viewerPos.setZ(viewerPos.z() + mvDir.x());
        break;

    case Qt::Key_Escape:
    case Qt::Key_Q:
        // exit application
        close();
    }

    // Allow the base class to also handle all keypress events
    QOpenGLWidget::keyPressEvent(e);

    // Maintain a fixed eye height above the ground
    viewerPos.setY(geometries->getHeight(viewerPos.x(), viewerPos.z()) + EYE_HEIGHT);

    update();
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
    lookDir.setX(-Sin(th) * Cos(ph));
    lookDir.setY(Sin(ph));
    lookDir.setZ(-Cos(th) * Cos(ph));

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

    // Enable back face culling
    // glEnable(GL_CULL_FACE);

    // Instantiate our geometry engine
    geometries = new GeometryEngine;

    // Adjust starting position to be near the lake.
    int retries = 11;
    while (!geometries->adjustViewerPos(viewerPos, QVector2D(WORLD_DIM / 2.0f, -WORLD_DIM).normalized()) && retries--)
    {
        // Lake not found.  Delete this world and make another one.  Oh, the POWER!!!
        delete geometries;
        geometries = new GeometryEngine;
    }

    // Now find a lookDir that looks along the edge of the lake.

    // Check the terrain 3 clicks away at angle th.  If it is above water, turn left.  Otherwise, turn right.
    // Keep turning until the point 3 water proximity clicks away transitions above or below water.
    QVector3D testVec(-Sin(th) * WATER_START_PROX * 3.0f, 0, -Cos(th) * WATER_START_PROX * 3.0);
    QVector3D testLoc(viewerPos + testVec);
    bool startAbove(geometries->getHeight(testLoc.x(), testLoc.z(), false) > geometries->getWaterLevel());
    float inc(startAbove ? 1.0f : -1.0f);
    bool curr(startAbove);
    while (startAbove == (curr = geometries->getHeight(testLoc.x(), testLoc.z(), false) > geometries->getWaterLevel()) && th > -720.0f && th < 720.0f)
    {
        th += inc;
        testVec = QVector3D(Sin(th) * WATER_START_PROX * -3.0f, 0, Cos(th) * WATER_START_PROX * -3.0f);
        testLoc = viewerPos + testVec;
    }
    if (th <= -720.0f || th >= 720.f)
        // The simplistic shoreline search failed.  Revert to a default lookdir
        th = 247.0f;

    lookDir.setX(-Sin(th) * Cos(ph));
    lookDir.setY(Sin(ph));
    lookDir.setZ(-Cos(th) * Cos(ph));

    // Set our eye initial height based on the terrain
    viewerPos.setY(geometries->getHeight(viewerPos.x(), viewerPos.z()) + EYE_HEIGHT);
}

void MainWidget::initShaders()
{
    // Compile vertex shaders
    if (!skyProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vtexonly.glsl"))
        close();
    if (!mainProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vtexlight.glsl"))
        close();
    if (!plantProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vplants.glsl"))
        close();

    // Compile fragment shaders
    if (!skyProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/ftexonly.glsl"))
        close();
    if (!mainProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/ftexlight.glsl"))
        close();
    if (!plantProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fplants.glsl"))
        close();

    // Link shader pipelines
    if (!skyProgram.link())
        close();
    if (!mainProgram.link())
        close();
    if (!plantProgram.link())
        close();
}

void MainWidget::initTextures()
{
    // Load textures
    skyTexture = new QOpenGLTexture(QImage(":/textures/Sky/2226.png").mirrored());
    landTexture = new QOpenGLTexture(QImage(":/textures/Land/85290912-seamless-tileable-natural-ground-field-texture.jpg").mirrored());
    waterTexture = new QOpenGLTexture(QImage(":/textures/Water/WaterPlain0012_1_270.jpg").mirrored());

    // Set nearest filtering mode for texture minification
    skyTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    landTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    waterTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);

    // Set bilinear filtering mode for texture magnification
    skyTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    landTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    waterTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    skyTexture->setWrapMode(QOpenGLTexture::Repeat);
    landTexture->setWrapMode(QOpenGLTexture::Repeat);
    waterTexture->setWrapMode(QOpenGLTexture::Repeat);
}

void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio to keep pixels square
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    const qreal zNear = 3.0f / WORLD_DIM, zFar = 3.0f * WORLD_DIM, fov = 55.0;

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

    // Set modelview-projection matrix
    mainProgram.setUniformValue("mv_matrix", matrix);
    mainProgram.setUniformValue("mvp_matrix", projection * matrix);
    mainProgram.setUniformValue("normalMatrix", matrix.normalMatrix());
    mainProgram.setUniformValue("lightPosition", lightPos);

    // Draw the land
    mainProgram.setUniformValue("texture", 0);
    landTexture->bind();
    geometries->drawLandGeometry(&mainProgram);

    // Draw the water
    waterTexture->bind();
    geometries->drawWaterGeometry(&mainProgram);

    // Bind plant shader pipeline
    if (!plantProgram.bind())
        close();

    plantProgram.setUniformValue("normalMatrix", matrix.normalMatrix());
    plantProgram.setUniformValue("lightPosition", lightPos);

    // Draw all of the trees
    for (int i = 0; i < TREE_COUNT; i++)
    {
        // Set translation matrix
        treePos = matrix;
        treePos.translate(geometries->treeSpot[i].toVector3D());
        treePos.scale(geometries->treeSpot[i].w(), geometries->treeSpot[i].w(), geometries->treeSpot[i].w());
        treePos.rotate(-90, 1, 0, 0); // Compensate for tree obj oriented on different axis

        plantProgram.setUniformValue("mv_matrix", treePos);
        plantProgram.setUniformValue("mvp_matrix", projection * treePos);

        // Draw a tree
        geometries->drawTreeGeometry(&plantProgram);
    }
}
