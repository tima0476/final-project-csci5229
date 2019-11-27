/****************************************************************************
**
** Adapted from "cube" example code from the Qt library
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
****************************************************************************/

#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class GeometryEngine : protected QOpenGLFunctions
{
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void drawSkyCubeGeometry(QOpenGLShaderProgram *program);
    void drawLandGeometry(QOpenGLShaderProgram *program);

private:
    void initSkyCubeGeometry();
    void initLandGeometry();

    QOpenGLBuffer skyVertBuf;
    QOpenGLBuffer skyFacetsBuf;
    QOpenGLBuffer landVertBuf;
    QOpenGLBuffer landFacetsBuf;
};

#endif // GEOMETRYENGINE_H
