/****************************************************************************
**
** Adapted from "cube" example code from the Qt library
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
****************************************************************************/

#include "geometryengine.h"

#include <QVector2D>
#include <QVector3D>

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

GeometryEngine::GeometryEngine() :
        skyFacetsBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();

    // Generate VBOs
    skyVertBuf.create();
    skyFacetsBuf.create();

    // Initializes cube geometry and transfers it to VBOs
    initSkyCubeGeometry();
}

GeometryEngine::~GeometryEngine()
{
    skyVertBuf.destroy();
    skyFacetsBuf.destroy();
}

void GeometryEngine::initSkyCubeGeometry()
{
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.  Split cube into two sets of faces due to textures
    // being split between two separate image files
    VertexData vertices[] = {
        // Vertex data for face 0  (Front)
        {QVector3D(-10.5f, -10.5f,  10.5f), QVector2D(1.00f, 1.0f / 3.0f)}, // v0
        {QVector3D( 10.5f, -10.5f,  10.5f), QVector2D(0.75f, 1.0f / 3.0f)}, // v1
        {QVector3D(-10.5f,  10.5f,  10.5f), QVector2D(1.00f, 2.0f / 3.0f)}, // v2
        {QVector3D( 10.5f,  10.5f,  10.5f), QVector2D(0.75f, 2.0f / 3.0f)}, // v3
    
        // Vertex data for face 1  (Right)  
        {QVector3D( 10.5f, -10.5f,  10.5f), QVector2D(0.75f, 1.0f / 3.0f)}, // v4
        {QVector3D( 10.5f, -10.5f, -10.5f), QVector2D(0.50f, 1.0f / 3.0f)}, // v5
        {QVector3D( 10.5f,  10.5f,  10.5f), QVector2D(0.75f, 2.0f / 3.0f)}, // v6
        {QVector3D( 10.5f,  10.5f, -10.5f), QVector2D(0.50f, 2.0f / 3.0f)}, // v7
    
        // Vertex data for face 2  (Rear)   
        {QVector3D( 10.5f, -10.5f, -10.5f), QVector2D(0.50f, 1.0f / 3.0f)}, // v8
        {QVector3D(-10.5f, -10.5f, -10.5f), QVector2D(0.25f, 1.0f / 3.0f)}, // v9
        {QVector3D( 10.5f,  10.5f, -10.5f), QVector2D(0.50f, 2.0f / 3.0f)}, // v10
        {QVector3D(-10.5f,  10.5f, -10.5f), QVector2D(0.25f, 2.0f / 3.0f)}, // v11
    
        // Vertex data for face 3  (Left)   
        {QVector3D(-10.5f, -10.5f, -10.5f), QVector2D(0.25f, 1.0f / 3.0f)}, // v12
        {QVector3D(-10.5f, -10.5f,  10.5f), QVector2D(0.00f, 1.0f / 3.0f)}, // v13
        {QVector3D(-10.5f,  10.5f, -10.5f), QVector2D(0.25f, 2.0f / 3.0f)}, // v14
        {QVector3D(-10.5f,  10.5f,  10.5f), QVector2D(0.00f, 2.0f / 3.0f)}, // v15
    
        // Vertex data for face 4  (Bottom) 
        {QVector3D(-10.5f, -10.5f, -10.5f), QVector2D(0.25f, 1.0f / 3.0f)}, // v16
        {QVector3D( 10.5f, -10.5f, -10.5f), QVector2D(0.50f, 1.0f / 3.0f)}, // v17
        {QVector3D(-10.5f, -10.5f,  10.5f), QVector2D(0.25f, 0.0f)},        // v18
        {QVector3D( 10.5f, -10.5f,  10.5f), QVector2D(0.50f, 0.0f)},        // v19
    
        // Vertex data for face 5  (Top)    
        {QVector3D(-10.5f,  10.5f,  10.5f), QVector2D(0.25f, 1.0f)},        // v20
        {QVector3D( 10.5f,  10.5f,  10.5f), QVector2D(0.50f, 1.0f)},        // v21
        {QVector3D(-10.5f,  10.5f, -10.5f), QVector2D(0.25f, 2.0f / 3.0f)}, // v22
        {QVector3D( 10.5f,  10.5f, -10.5f), QVector2D(0.50f, 2.0f / 3.0f)}  // v23
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.


    // From https://community.khronos.org/t/correct-usage-gl-triangle-strip/23568

    // A triangle strip is something like this:

    // 0-2-4-6-8  =>
    // |/|/|/|/|  =>
    // 1-3-5-7-9  =>

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
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, skyFacetsBuf.size()/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}
