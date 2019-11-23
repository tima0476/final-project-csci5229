/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
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

//! [0]
GeometryEngine::GeometryEngine()
{
    initializeOpenGLFunctions();

    // Generate VBOs
    for (int i = 0; i < 2; i++)
    {   arrayBuf[i] = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        arrayBuf[i]->create();

        indexBuf[i] = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        indexBuf[i]->create();
    }

    // Initializes cube geometry and transfers it to VBOs
    initCubeGeometry();
}

GeometryEngine::~GeometryEngine()
{
    for (int i = 0; i < 2; i++)
    {
        arrayBuf[i]->destroy();
        delete arrayBuf[i];

        indexBuf[i]->destroy();
        delete indexBuf[i];
    }
}
//! [0]

void GeometryEngine::initCubeGeometry()
{
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.  Split cube into two sets of faces due to textures
    // being split between two separate image files
    VertexData vertices0[] = {
        // Vertex data for face 0  (Front)
        {QVector3D(-10.5f, -10.5f,  10.5f), QVector2D(0.75f, 0.00f)}, // v0
        {QVector3D( 10.5f, -10.5f,  10.5f), QVector2D(0.50f, 0.00f)}, // v1
        {QVector3D(-10.5f,  10.5f,  10.5f), QVector2D(0.75f, 1.00f)}, // v2
        {QVector3D( 10.5f,  10.5f,  10.5f), QVector2D(0.50f, 1.00f)}, // v3

        // Vertex data for face 1  (Right)
        {QVector3D( 10.5f, -10.5f,  10.5f), QVector2D(0.50f, 0.00f)}, // v4
        {QVector3D( 10.5f, -10.5f, -10.5f), QVector2D(0.25f, 0.00f)}, // v5
        {QVector3D( 10.5f,  10.5f,  10.5f), QVector2D(0.50f, 1.00f)}, // v6
        {QVector3D( 10.5f,  10.5f, -10.5f), QVector2D(0.25f, 1.00f)}, // v7

        // Vertex data for face 2  (Rear)
        {QVector3D( 10.5f, -10.5f, -10.5f), QVector2D(0.25f, 0.00f)}, // v8
        {QVector3D(-10.5f, -10.5f, -10.5f), QVector2D(0.00f, 0.00f)}, // v9
        {QVector3D( 10.5f,  10.5f, -10.5f), QVector2D(0.25f, 1.00f)}, // v10
        {QVector3D(-10.5f,  10.5f, -10.5f), QVector2D(0.00f, 1.00f)}, // v11

        // Vertex data for face 3  (Left)
        {QVector3D(-10.5f, -10.5f, -10.5f), QVector2D(1.00f, 0.00f)}, // v12
        {QVector3D(-10.5f, -10.5f,  10.5f), QVector2D(0.75f, 0.00f)}, // v13
        {QVector3D(-10.5f,  10.5f, -10.5f), QVector2D(1.00f, 1.00f)}, // v14
        {QVector3D(-10.5f,  10.5f,  10.5f), QVector2D(0.75f, 1.00f)}  // v15
    };
    VertexData vertices1[] = {
        // Vertex data for face 4  (Bottom)
        {QVector3D(-10.5f, -10.5f, -10.5f), QVector2D(1.00f, 0.00f)}, // 0
        {QVector3D( 10.5f, -10.5f, -10.5f), QVector2D(0.50f, 0.00f)}, // 1
        {QVector3D(-10.5f, -10.5f,  10.5f), QVector2D(1.00f, 1.00f)}, // 2
        {QVector3D( 10.5f, -10.5f,  10.5f), QVector2D(0.50f, 1.00f)}, // 3

        // Vertex data for face 5  (Top)
        {QVector3D(-10.5f,  10.5f,  10.5f), QVector2D(0.50f, 1.00f)}, // 4
        {QVector3D( 10.5f,  10.5f,  10.5f), QVector2D(0.50f, 0.00f)}, // 5
        {QVector3D(-10.5f,  10.5f, -10.5f), QVector2D(0.00f, 1.00f)}, // 6
        {QVector3D( 10.5f,  10.5f, -10.5f), QVector2D(0.00f, 0.00f)}  // 7
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


    GLushort indices0[] = {
         1,  0,  3,  2,  2,     // Face 0 (Front)  - triangle strip ( v0.1,  v0.0,  v0.3,  v0.2)
         5,  5,  4,  7,  6,  6, // Face 1 (Right)  - triangle strip ( v0.5,  v0.4,  v0.7,  v0.6)
         9,  9,  8, 11, 10, 10, // Face 2 (Rear)   - triangle strip ( v0.9,  v0.8, v0.11, v0.10)
        13, 13, 12, 15, 14      // Face 3 (Left)   - triangle strip (v0.13, v0.12, v0.15, v0.14)
    };
    GLushort indices1[] = {
         1,  0,  3,  2,  2,     // Face 4 (Bottom) - triangle strip ( v1.1,  v1.0,  v1.3,  v1.2)
         5,  5,  4,  7,  6      // Face 5 (Top)    - triangle strip ( v1.5,  v1.4,  v1.7,  v1.6)
    };

    arrayBuf[0]->bind();
    arrayBuf[0]->allocate(vertices0, sizeof(vertices0));

    indexBuf[0]->bind();
    indexBuf[0]->allocate(indices0, sizeof(indices0));

    arrayBuf[1]->bind();
    arrayBuf[1]->allocate(vertices1, sizeof(vertices1));

    indexBuf[1]->bind();
    indexBuf[1]->allocate(indices1, sizeof(indices1));
}

//! [2]
void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program, int set)
{
    // Tell OpenGL which VBOs to use
    arrayBuf[set]->bind();
    indexBuf[set]->bind();

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
    glDrawElements(GL_TRIANGLE_STRIP, indexBuf[set]->size()/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}
//! [2]
