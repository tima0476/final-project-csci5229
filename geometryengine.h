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

#define LAND_DIVS       513     // Number of divisions in each cardinal direction for the land grid.  The "Diamond Square" 
                                // terrain generation algorithm requires this to be 2^n+1 where n is a positive integer
#define LAND_TEX_REPS   20      // The number of times the land texture repeats over the width and depth of the world
#define WORLD_DIM       20.0f   // Half the width & depth & height of the world
#define TERRAIN_RANGE   4.0f    // The maximum height range of the terrain
#define TERRAIN_SMOOTH  20.0f   // Larger numbers give smoother terrain

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
    float getHeight(float x, float z);

private:
    void initSkyCubeGeometry();
    void initLandGeometry();

    void diamondSquare(int size, bool presetCenter = false);
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
