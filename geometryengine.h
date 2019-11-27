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

#define LAND_DIVS       129     // Number of divisions in each cardinal direction for the land grid.  Can't exceed 256
                                // due to LAND_DIVS^2 must fit into a 16-bit unsigned integer.  And, the "Diamond Square" 
                                // terrain generation algorithm requires it to be a power of 2 plus 1.
                                // Therefore, valid values are:  3, 5, 9, 17, 33, 65, 129
#define LAND_TEX_REPS   10      // The number of times the land texture repeats over the width and depth of the world

// Convenience macros to improve readability
#define Coord_2on1(X,Z) ((Z)*LAND_DIVS + (X))
#define Frand(RANGE)    (float(rand()) * float(RANGE) / float(RAND_MAX))

struct unlitVertexData
{
    QVector3D position;
    QVector2D texCoord;
};

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

    void diamondSquare(int size);
    void squareStep(int x, int z, int reach);
    void diamondStep(int x, int z, int reach);

    unlitVertexData landVerts[LAND_DIVS*LAND_DIVS];     // Make this array a class member so we don't have to 
                                                        // pass it around on the stack

    QOpenGLBuffer skyVertBuf;
    QOpenGLBuffer skyFacetsBuf;
    QOpenGLBuffer landVertBuf;
    QOpenGLBuffer landFacetsBuf;
};

#endif // GEOMETRYENGINE_H
