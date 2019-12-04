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
#include "wavefrontObj.h"

#define LAND_DIVS 513         // The number of divisions in each cardinal direction for the land grid.  The "Diamond Square" \
                              // terrain generation algorithm requires this to be 2^n+1 where n is a positive integer
#define LAND_TEX_REPS 40      // The number of times the land texture repeats over the width and depth of the world
#define WORLD_DIM 40.0f       // Half the width & depth & height of the world
#define TERRAIN_RANGE 3.0f    // The maximum height range of the terrain
#define TERRAIN_SMOOTH 6.0f   // Larger numbers give smoother terrain
#define WATER_LEVEL -1.5f     // elevation of water surface as offset from avg
#define WATER_TEX_REPS 35.0f  // number of times to repeat the water texture
#define WATER_START_PROX 2.0f // Starting distance from the edge of the water
#define TREE_COUNT 1000       // The number of trees in this world
#define TREE_RANGE_L 0.2f     // The maximum size range of the trees (multiplier)
#define TREE_RANGE_H 1.0f     // The maximum size range of the trees (multiplier)
#define TREE_SINK 0.1f        // how far underground trees extend
#define TREE_MIN_PROX 0.25f   // minimum distance between trees

// Convenience macros to improve readability
#define Coord_2on1(X, Z) ((Z)*LAND_DIVS + (X))
#define Frand(RANGE) (float(rand()) * float(RANGE) / float(RAND_MAX))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

// Packed structures to use for the OpenGL VBOs
struct unlitVertexData
{
    QVector3D position;
    QVector2D texCoord;
};

struct vertexData
{
    QVector3D position;
    QVector2D texCoord;
    QVector3D normal;
};

class GeometryEngine : protected QOpenGLFunctions
{
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void drawSkyCubeGeometry(QOpenGLShaderProgram *program);
    void drawLandGeometry(QOpenGLShaderProgram *program);
    void drawWaterGeometry(QOpenGLShaderProgram *program);
    void drawTreeGeometry(QOpenGLShaderProgram *program);
    float getHeight(float x, float z, bool stayAbove = true);
    bool adjustViewerPos(QVector3D &viewerPos, QVector2D searchDir);
    float getWaterLevel(void) { return waterLevel; }
    void placeTrees(void);

    QVector4D treeSpot[TREE_COUNT]; // xyz for location of each tree.  W will use for random scaling

private:
    void initSkyCubeGeometry();
    void initLandGeometry();
    void initWaterGeometry();
    void initTreeGeometry();

    void diamondSquare(int size, bool presetCenter = false);
    void squareStep(int x, int z, int reach);
    void diamondStep(int x, int z, int reach);

    vertexData landVerts[LAND_DIVS * LAND_DIVS]; // Make this array a class member so we don't have to
                                                 // pass it around on the stack

    QOpenGLBuffer skyVertBuf;
    QOpenGLBuffer skyFacetsBuf;
    QOpenGLBuffer landVertBuf;
    QOpenGLBuffer landFacetsBuf;
    QOpenGLBuffer waterVertBuf;
    QOpenGLBuffer waterFacetsBuf;
    QVector<QOpenGLBuffer> treeVertBuf;
    QVector<QOpenGLBuffer> treeFacetsBuf;
    QVector<QOpenGLTexture *> treeTexture;

    float landAvg, waterLevel;
    wavefrontObj tree;
    float closestTree(float x, float z);
};

#endif // GEOMETRYENGINE_H
