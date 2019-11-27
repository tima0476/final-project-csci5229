/****************************************************************************
**
** Adapted from "cube" example code from the Qt library
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
****************************************************************************/

#include "geometryengine.h"
#include <QVector2D>
#include <QVector3D>

GeometryEngine::GeometryEngine() :
        skyVertBuf(QOpenGLBuffer::VertexBuffer),
        skyFacetsBuf(QOpenGLBuffer::IndexBuffer),
        landVertBuf(QOpenGLBuffer::VertexBuffer),
        landFacetsBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();

    // Generate VBOs
    skyVertBuf.create();
    skyFacetsBuf.create();
    landVertBuf.create();
    landFacetsBuf.create();

    // Initialize the geometries and transfer them to the VBOs
    initSkyCubeGeometry();
    initLandGeometry();
}

GeometryEngine::~GeometryEngine()
{
    skyVertBuf.destroy();
    skyFacetsBuf.destroy();
    landVertBuf.destroy();
    landFacetsBuf.destroy();
}

void GeometryEngine::initLandGeometry()
{
    for (int zi = 0; zi < LAND_DIVS; zi++)
    {
        float zfrac = zi / float(LAND_DIVS - 1);
        for (int xi = 0; xi < LAND_DIVS; xi++)
        {
            float xfrac = xi / float(LAND_DIVS - 1);

            landVerts[Coord_2on1(xi,zi)] = {
                QVector3D(-20.0f + (40.0f * xfrac), -2.0f, -20.0f + (40.0f * zfrac)),    // Vertex Coordinate
                QVector2D(xfrac * LAND_TEX_REPS, zfrac * LAND_TEX_REPS)                 // Texture Coordinate
            };
        }
    }

    // Seed the terrain generator with random heights at the 4 corners:
    landVerts[Coord_2on1(0,           0          )].position.setY(Frand(-4.0f) - 2.0f);
    landVerts[Coord_2on1(LAND_DIVS-1, 0          )].position.setY(Frand(-4.0f) - 2.0f);
    landVerts[Coord_2on1(0,           LAND_DIVS-1)].position.setY(Frand(-4.0f) - 2.0f);
    landVerts[Coord_2on1(LAND_DIVS-1, LAND_DIVS-1)].position.setY(Frand(-4.0f) - 2.0f);

    // Randomize the terrain heights
    diamondSquare(LAND_DIVS);

    GLushort indices[2 * LAND_DIVS * LAND_DIVS - 4];
    GLushort *pi = indices; // Use a walking pointer to fill the facet array since we occasionally need to repeat some indices

    // Build the index list for a series of triangle strips
    for (int zi = 0; zi < (LAND_DIVS - 1); zi++) // rows
    {
        for (int xi = 0; xi < LAND_DIVS; xi++) // columns
        {
            *pi = xi + zi * LAND_DIVS;
            pi++;
            if (zi && !xi)
            {
                // Repeat the first index to signify new row, except for row 0
                *pi = *(pi - 1);
                pi++;
            }

            *pi = xi + (zi + 1) * LAND_DIVS;
            pi++;
        }
        if (zi < (LAND_DIVS - 2))
        {
            // Double the last index in a row to signify end of row, except for last row
            *pi = *(pi - 1);
            pi++;
        }
    }

    landVertBuf.bind();
    landVertBuf.allocate(landVerts, sizeof(landVerts));

    landFacetsBuf.bind();
    skyFacetsBuf.allocate(indices, sizeof(indices));
}

void GeometryEngine::initSkyCubeGeometry()
{
    unlitVertexData vertices[] = {
        // Vertex data for face 0  (Front; z = 20.0)
        {QVector3D(-20.0f, -20.0f,  20.0f), QVector2D(1.00f, 1.0f / 3.0f)}, // v0
        {QVector3D( 20.0f, -20.0f,  20.0f), QVector2D(0.75f, 1.0f / 3.0f)}, // v1
        {QVector3D(-20.0f,  20.0f,  20.0f), QVector2D(1.00f, 2.0f / 3.0f)}, // v2
        {QVector3D( 20.0f,  20.0f,  20.0f), QVector2D(0.75f, 2.0f / 3.0f)}, // v3

        // Vertex data for face 1  (Right; x = 20.0)  
        {QVector3D( 20.0f, -20.0f,  20.0f), QVector2D(0.75f, 1.0f / 3.0f)}, // v4
        {QVector3D( 20.0f, -20.0f, -20.0f), QVector2D(0.50f, 1.0f / 3.0f)}, // v5
        {QVector3D( 20.0f,  20.0f,  20.0f), QVector2D(0.75f, 2.0f / 3.0f)}, // v6
        {QVector3D( 20.0f,  20.0f, -20.0f), QVector2D(0.50f, 2.0f / 3.0f)}, // v7

        // Vertex data for face 2  (Rear; z = -20.0)   
        {QVector3D( 20.0f, -20.0f, -20.0f), QVector2D(0.50f, 1.0f / 3.0f)}, // v8
        {QVector3D(-20.0f, -20.0f, -20.0f), QVector2D(0.25f, 1.0f / 3.0f)}, // v9
        {QVector3D( 20.0f,  20.0f, -20.0f), QVector2D(0.50f, 2.0f / 3.0f)}, // v10
        {QVector3D(-20.0f,  20.0f, -20.0f), QVector2D(0.25f, 2.0f / 3.0f)}, // v11

        // Vertex data for face 3  (Left; x = -20.0)   
        {QVector3D(-20.0f, -20.0f, -20.0f), QVector2D(0.25f, 1.0f / 3.0f)}, // v12
        {QVector3D(-20.0f, -20.0f,  20.0f), QVector2D(0.00f, 1.0f / 3.0f)}, // v13
        {QVector3D(-20.0f,  20.0f, -20.0f), QVector2D(0.25f, 2.0f / 3.0f)}, // v14
        {QVector3D(-20.0f,  20.0f,  20.0f), QVector2D(0.00f, 2.0f / 3.0f)}, // v15

        // Vertex data for face 4  (Bottom; y = -20.0) 
        {QVector3D(-20.0f, -20.0f, -20.0f), QVector2D(0.25f, 1.0f / 3.0f)}, // v16
        {QVector3D( 20.0f, -20.0f, -20.0f), QVector2D(0.50f, 1.0f / 3.0f)}, // v17
        {QVector3D(-20.0f, -20.0f,  20.0f), QVector2D(0.25f, 0.0f)},        // v18
        {QVector3D( 20.0f, -20.0f,  20.0f), QVector2D(0.50f, 0.0f)},        // v19

        // Vertex data for face 5  (Top; y = 20.0)    
        {QVector3D(-20.0f,  20.0f,  20.0f), QVector2D(0.25f, 1.0f)},        // v20
        {QVector3D( 20.0f,  20.0f,  20.0f), QVector2D(0.50f, 1.0f)},        // v21
        {QVector3D(-20.0f,  20.0f, -20.0f), QVector2D(0.25f, 2.0f / 3.0f)}, // v22
        {QVector3D( 20.0f,  20.0f, -20.0f), QVector2D(0.50f, 2.0f / 3.0f)} // v23
    };
    
    GLushort indices[] = {
         1,  0,  3,  2,  2,     // Face 0 (Front)
         5,  5,  4,  7,  6,  6, // Face 1 (Right)
         9,  9,  8, 11, 10, 10, // Face 2 (Rear)
        13, 13, 12, 15, 14, 14, // Face 3 (Left)
        17, 17, 16, 19, 18, 18, // Face 4 (Bottom)
        21, 21, 20, 23, 22      // Face 5 (Top)
    };

    skyVertBuf.bind();
    skyVertBuf.allocate(vertices, sizeof(vertices));

    skyFacetsBuf.bind();
    skyFacetsBuf.allocate(indices, sizeof(indices));
}

void GeometryEngine::drawSkyCubeGeometry(QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    skyVertBuf.bind();
    skyFacetsBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(unlitVertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(unlitVertexData));

    glDrawElements(GL_TRIANGLE_STRIP, skyFacetsBuf.size()/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}

void GeometryEngine::drawLandGeometry(QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    landVertBuf.bind();
    landFacetsBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(unlitVertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(unlitVertexData));

    glDrawElements(GL_TRIANGLE_STRIP, landFacetsBuf.size()/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}

//
// The following code implements the "Diamond Square" recursive terrain generation
// algorithm.  "The idea was first introduced by Fournier, Fussell and Carpenter at SIGGRAPH 1982.": 
// Fournier, Alain; Fussell, Don; Carpenter, Loren (June 1982). "Computer rendering of stochastic models". 
//      Communications of the ACM. 25 (6): 371–384. doi:10.1145/358523.358553 
// https://en.wikipedia.org/wiki/Diamond-square_algorithm
//
// This C++ implementation is based from the example code found at: 
// https://medium.com/@nickobrien/diamond-square-algorithm-explanation-and-c-implementation-5efa891e486f
//
void GeometryEngine::diamondSquare(int size)
{
    int half = size / 2;
    if (half < 1)
        return;
    
    //square steps
    for (int z = half; z < LAND_DIVS; z += size)
        for (int x = half; x < LAND_DIVS; x += size)
            squareStep(x % LAND_DIVS, z % LAND_DIVS, half);
    
    // diamond steps
    int col = 0;
    for (int x = 0; x < LAND_DIVS; x += half)
    {
        col++;
        //If this is an odd column.
        if (col % 2 == 1)
            for (int z = half; z < LAND_DIVS; z += size)
                diamondStep(x % LAND_DIVS, z % LAND_DIVS, half);
        else
            for (int z = 0; z < LAND_DIVS; z += size)
                diamondStep(x % LAND_DIVS, z % LAND_DIVS, half);
    }
    diamondSquare(size / 2);
}

void GeometryEngine::squareStep(int x, int z, int reach)
{
    int count = 0;
    float avg = 0.0f;
    if (x - reach >= 0 && z - reach >= 0)
    {
        avg += landVerts[Coord_2on1(x - reach, z - reach)].position.y();
        count++;
    }
    if (x - reach >= 0 && z + reach < LAND_DIVS)
    {
        avg += landVerts[Coord_2on1(x - reach, z + reach)].position.y();
        count++;
    }
    if (x + reach < LAND_DIVS && z - reach >= 0)
    {
        avg += landVerts[Coord_2on1(x + reach, z - reach)].position.y();
        count++;
    }
    if (x + reach < LAND_DIVS && z + reach < LAND_DIVS)
    {
        avg += landVerts[Coord_2on1(x + reach, z + reach)].position.y();
        count++;
    }
    avg += Frand(reach/2.5f) - reach/5.0f;
    avg /= float(count);
    landVerts[Coord_2on1(x,z)].position.setY(avg);
}

void GeometryEngine::diamondStep(int x, int z, int reach)
{
    int count = 0;
    float avg = 0.0f;
    if (x - reach >= 0)
    {
        avg += landVerts[Coord_2on1(x - reach,z)].position.y();
        count++;
    }
    if (x + reach < LAND_DIVS)
    {
        avg += landVerts[Coord_2on1(x + reach,z)].position.y();
        count++;
    }
    if (z - reach >= 0)
    {
        avg += landVerts[Coord_2on1(x,z - reach)].position.y();
        count++;
    }
    if (z + reach < LAND_DIVS)
    {
        avg += landVerts[Coord_2on1(x,z + reach)].position.y();
        count++;
    }
    avg += Frand(reach/2.5f) - reach/5.0f;
    avg /= float(count);
    landVerts[Coord_2on1(x,z)].position.setY(avg);
}

