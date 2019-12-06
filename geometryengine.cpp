/****************************************************************************
**
** Adapted from "cube" example code from the Qt library
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
****************************************************************************/

#include <QVector2D>
#include <QVector3D>
#include "geometryengine.h"
#include <float.h> // for FLT_MAX
#include <math.h>  // for sqrt()

#include <iostream>
using namespace std;

GeometryEngine::GeometryEngine() : skyVertBuf(QOpenGLBuffer::VertexBuffer),
                                   skyFacetsBuf(QOpenGLBuffer::IndexBuffer),
                                   landVertBuf(QOpenGLBuffer::VertexBuffer),
                                   landFacetsBuf(QOpenGLBuffer::IndexBuffer),
                                   waterVertBuf(QOpenGLBuffer::VertexBuffer),
                                   waterFacetsBuf(QOpenGLBuffer::IndexBuffer),
                                   waterLevel(-WORLD_DIM),
                                   tree("Spruce.obj")
{
    initializeOpenGLFunctions();

    // Generate VBOs
    skyVertBuf.create();
    skyFacetsBuf.create();
    landVertBuf.create();
    landFacetsBuf.create();
    waterVertBuf.create();
    waterFacetsBuf.create();

    // Initialize the geometries and transfer them to the VBOs
    initSkyCubeGeometry();
    initLandGeometry();
    initWaterGeometry();
    initTreeGeometry();
    placeTrees();
}

GeometryEngine::~GeometryEngine()
{
    for (int i = 0; i < tree.data.section.size(); i++)
        delete treeTexture[i];
    skyVertBuf.destroy();
    skyFacetsBuf.destroy();
    landVertBuf.destroy();
    landFacetsBuf.destroy();
    waterVertBuf.destroy();
    waterFacetsBuf.destroy();
    for (int i = 0; i < treeVertBuf.size(); i++)
    {
        // It is safe to assume the vertex and facet buffer arrays are the same size
        treeVertBuf[i].destroy();
        treeFacetsBuf[i].destroy();
    }
}
// #define V2(X) (X).x() << "," << (X).y()
// #define V3(X) (X).x() << "," << (X).y() << "," << (X).z()
// #define V4(X) (X).x() << "," << (X).y() << "," << (X).z() << "," << (X).w()

void GeometryEngine::initTreeGeometry()
{
    // The obj loader has vertices, texture coordinates, and normal coordinates in three separate arrays which are indexed independently,
    // reflecting the format of an obj file.  For OpenGL VBO's, this has to be consolidated into packed vertex arrays.

    // Since the obj format potentially has multiple object sections with different materials on each section, set an array of dynamic
    // arrays.  Each vertex / index pair will represent a single "object section" such as trunk, branches
    int numSections = tree.data.section.size();

    QVector<vertexData> vertex[numSections];
    QVector<GLushort> index[numSections];

    facetChunk.clear();

    for (int i = 0; i < numSections; i++)
    {
        objectSection *s = &tree.data.section[i];
        // Materials properties
        materialData *mtl = &(s->mtl);

        // Set up the texture for this section
        treeTexture << new QOpenGLTexture(QImage(mtl->map_d_filename).mirrored());
        treeTexture.last()->setMinificationFilter(QOpenGLTexture::NearestMipMapLinear);
        treeTexture.last()->setMagnificationFilter(QOpenGLTexture::NearestMipMapLinear);
        treeTexture.last()->setWrapMode(QOpenGLTexture::Repeat);

        // Iterate through the facets and build the packed vertex array to match it
        facetChunkData fc = {0, 0};
        facetChunk << QVector<facetChunkData>();

        for (int j = 0; j < s->f.size(); j++)
        {
            int i_v = s->f[j].v;      // Index into the obj vertex array
            int i_vt = s->f[j].vt;    // Index into the obj texture coordinate array
            int i_vn = s->f[j].vn;    // Index into the obj normal array
            bool edge = s->f[j].edge; // true if this is the last vertex in a facet

            vertexData vd = {tree.data.v[i_v - 1], tree.data.vt[i_vt - 1], tree.data.vn[i_vn - 1]};

            // If VBO size becomes an issue, optimize by adding code to search if a vertex set is already in the array, and
            // re-use if it is.

            vertex[i] << vd;
            index[i] << vertex[i].size() - 1;
            fc.count++;
            if (edge)
            {
                // repeat the first index of the chunk to close the poly
                vertex[i] << vertex[i][fc.base];
                index[i] << vertex[i].size() - 1;
                fc.count++;

                // end of a chunk.  Add it to the list.
                facetChunk.last() << fc;

                // start the next chunk
                fc.base = vertex[i].size();
                fc.count = 0;
            }
        }

        // for (int j = 0; j < facetChunk.size(); j++)
        // {
        //     cout << "Facet set " << j << endl;
        //     for (int k = 0; k < facetChunk[j].size(); k++)
        //     {
        //         cout << "  " << k << ": base=" << facetChunk[j][k].base << "   count=" << facetChunk[j][k].count << endl;
        //     }
        // }
    }

    // Now create the VBOs and transfer the data
    treeVertBuf.clear();
    treeFacetsBuf.clear();
    for (int i = 0; i < numSections; i++)
    {
        treeVertBuf << QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        treeVertBuf[i].create();
        treeVertBuf[i].bind();
        treeVertBuf[i].allocate(vertex[i].constData(), vertex[i].size() * sizeof(vertex[i][0]));

        treeFacetsBuf << QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        treeFacetsBuf[i].create();
        treeFacetsBuf[i].bind();
        treeFacetsBuf[i].allocate(index[i].constData(), index[i].size() * sizeof(index[i][0]));
    }
}

void GeometryEngine::initLandGeometry()
{
    for (int zi = 0; zi < LAND_DIVS; zi++)
    {
        float zfrac = zi / float(LAND_DIVS - 1);
        for (int xi = 0; xi < LAND_DIVS; xi++)
        {
            float xfrac = xi / float(LAND_DIVS - 1);

            landVerts[Coord_2on1(xi, zi)] = {
                // Vertex
                QVector3D(
                    -WORLD_DIM + (WORLD_DIM * 2.0f * xfrac),  // Vertex x
                    -2.0f,                                    // Vertex y (default flat terrain will be refined below)
                    -WORLD_DIM + (WORLD_DIM * 2.0f * zfrac)), // Vertex z

                // Texture Coordinate
                QVector2D(xfrac * LAND_TEX_REPS, zfrac * LAND_TEX_REPS), // Texture Coordinate

                // Normal vertex
                QVector3D() // compute later after random terrain generated
            };
        }
    }

    // Seed the terrain generator with random heights at the 4 corners.
    landVerts[Coord_2on1(0, 0)].position.setY(Frand(-TERRAIN_RANGE) - (TERRAIN_RANGE / 2.0f));
    landVerts[Coord_2on1(LAND_DIVS - 1, 0)].position.setY(Frand(-TERRAIN_RANGE) - (TERRAIN_RANGE / 2.0f));
    landVerts[Coord_2on1(0, LAND_DIVS - 1)].position.setY(Frand(-TERRAIN_RANGE) - (TERRAIN_RANGE / 2.0f));
    landVerts[Coord_2on1(LAND_DIVS - 1, LAND_DIVS - 1)].position.setY(Frand(-TERRAIN_RANGE) - (TERRAIN_RANGE / 2.0f));

    // Bias terrain to be bowl shaped by forcing a very low elevation of the initial center point
    landVerts[Coord_2on1(LAND_DIVS / 2, LAND_DIVS / 2)].position.setY(-TERRAIN_RANGE - 5.0f);

    // Randomize the terrain heights
    diamondSquare(LAND_DIVS, true);

    // Calculate normals, and also calculate the average elevation for use in determining the water level
    landAvg = 0.0;
    for (int zi = 0; zi < LAND_DIVS; zi++)
    {
        for (int xi = 0; xi < LAND_DIVS; xi++)
        {
            QVector3D p = landVerts[Coord_2on1(xi, zi)].position;

            // Calculate the direction vectors to the (2..4) adjacent grid points.  Handle "edge of the grid" cases with zero vectors
            // because they will then drop out when used in a cross-product.  (default QVector3d with null constructor is a zero vector)
            QVector3D va;
            if (zi)
                va = landVerts[Coord_2on1(xi, zi - 1)].position - p;
            QVector3D vb;
            if (xi < LAND_DIVS - 1)
                vb = landVerts[Coord_2on1(xi + 1, zi)].position - p;
            QVector3D vc;
            if (zi < LAND_DIVS - 1)
                vc = landVerts[Coord_2on1(xi, zi + 1)].position - p;
            QVector3D vd;
            if (xi)
                vd = landVerts[Coord_2on1(xi - 1, zi)].position - p;

            // Magnitude of the normal vector doesn't matter, so just do a sum to get a vector pointing in the average direction.
            // Edge cases will drop out due to having one vector (edge) or both vectors (corner) being zero.
            landVerts[Coord_2on1(xi, zi)].normal = QVector3D::crossProduct(vb, va) + QVector3D::crossProduct(vc, vb) +
                                                   QVector3D::crossProduct(vd, vc) + QVector3D::crossProduct(vd, va);
            landAvg += p.y();
        }
    }
    landAvg /= LAND_DIVS * LAND_DIVS;

    //
    // Now create the facets (index) array for the land grid
    //
    GLuint indices[2 * LAND_DIVS * LAND_DIVS - 4];
    GLuint *pi = indices; // Use a walking pointer to fill the facet array since we occasionally need to repeat some indices at strip boundaries

    // Build the index list (facets) for a series of triangle strips
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
    landFacetsBuf.allocate(indices, sizeof(indices));
}

void GeometryEngine::initWaterGeometry()
{
    // Dynamically set the water level
    waterLevel = landAvg + WATER_LEVEL;

    vertexData vertices[] = {
        // Vertex data for water surface plane
        {QVector3D(-WORLD_DIM, waterLevel, -WORLD_DIM), QVector2D(0.0f, WATER_TEX_REPS), QVector3D(0.0f, 1.0f, 0.0f)},
        {QVector3D(WORLD_DIM, waterLevel, -WORLD_DIM), QVector2D(WATER_TEX_REPS, WATER_TEX_REPS), QVector3D(0.0f, 1.0f, 0.0f)},
        {QVector3D(-WORLD_DIM, waterLevel, WORLD_DIM), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)},
        {QVector3D(WORLD_DIM, waterLevel, WORLD_DIM), QVector2D(WATER_TEX_REPS, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)},
    };

    GLushort indices[] = {
        0, 2, 1, 3};

    waterVertBuf.bind();
    waterVertBuf.allocate(vertices, sizeof(vertices));

    waterFacetsBuf.bind();
    waterFacetsBuf.allocate(indices, sizeof(indices));
}

void GeometryEngine::initSkyCubeGeometry()
{
    unlitVertexData vertices[] = {
        // Vertex data for face 0  (Front)
        {QVector3D(-WORLD_DIM, -WORLD_DIM, WORLD_DIM), QVector2D(1.00f, 1.0f / 3.0f)}, // v0
        {QVector3D(WORLD_DIM, -WORLD_DIM, WORLD_DIM), QVector2D(0.75f, 1.0f / 3.0f)},  // v1
        {QVector3D(-WORLD_DIM, WORLD_DIM, WORLD_DIM), QVector2D(1.00f, 2.0f / 3.0f)},  // v2
        {QVector3D(WORLD_DIM, WORLD_DIM, WORLD_DIM), QVector2D(0.75f, 2.0f / 3.0f)},   // v3

        // Vertex data for face 1  (Right)
        {QVector3D(WORLD_DIM, -WORLD_DIM, WORLD_DIM), QVector2D(0.75f, 1.0f / 3.0f)},  // v4
        {QVector3D(WORLD_DIM, -WORLD_DIM, -WORLD_DIM), QVector2D(0.50f, 1.0f / 3.0f)}, // v5
        {QVector3D(WORLD_DIM, WORLD_DIM, WORLD_DIM), QVector2D(0.75f, 2.0f / 3.0f)},   // v6
        {QVector3D(WORLD_DIM, WORLD_DIM, -WORLD_DIM), QVector2D(0.50f, 2.0f / 3.0f)},  // v7

        // Vertex data for face 2  (Rear)
        {QVector3D(WORLD_DIM, -WORLD_DIM, -WORLD_DIM), QVector2D(0.50f, 1.0f / 3.0f)},  // v8
        {QVector3D(-WORLD_DIM, -WORLD_DIM, -WORLD_DIM), QVector2D(0.25f, 1.0f / 3.0f)}, // v9
        {QVector3D(WORLD_DIM, WORLD_DIM, -WORLD_DIM), QVector2D(0.50f, 2.0f / 3.0f)},   // v10
        {QVector3D(-WORLD_DIM, WORLD_DIM, -WORLD_DIM), QVector2D(0.25f, 2.0f / 3.0f)},  // v11

        // Vertex data for face 3  (Left)
        {QVector3D(-WORLD_DIM, -WORLD_DIM, -WORLD_DIM), QVector2D(0.25f, 1.0f / 3.0f)}, // v12
        {QVector3D(-WORLD_DIM, -WORLD_DIM, WORLD_DIM), QVector2D(0.00f, 1.0f / 3.0f)},  // v13
        {QVector3D(-WORLD_DIM, WORLD_DIM, -WORLD_DIM), QVector2D(0.25f, 2.0f / 3.0f)},  // v14
        {QVector3D(-WORLD_DIM, WORLD_DIM, WORLD_DIM), QVector2D(0.00f, 2.0f / 3.0f)},   // v15

        // Vertex data for face 4  (Bottom)
        {QVector3D(-WORLD_DIM, -WORLD_DIM, -WORLD_DIM), QVector2D(0.25f, 1.0f / 3.0f)}, // v16
        {QVector3D(WORLD_DIM, -WORLD_DIM, -WORLD_DIM), QVector2D(0.50f, 1.0f / 3.0f)},  // v17
        {QVector3D(-WORLD_DIM, -WORLD_DIM, WORLD_DIM), QVector2D(0.25f, 0.0f)},         // v18
        {QVector3D(WORLD_DIM, -WORLD_DIM, WORLD_DIM), QVector2D(0.50f, 0.0f)},          // v19

        // Vertex data for face 5  (Top)    (fudge a tiny bit lower and stretch texture a tiny bit to smooth an annoying  seam in the sky)
        {QVector3D(-WORLD_DIM, WORLD_DIM - 0.1, WORLD_DIM), QVector2D(0.26f, 0.99f)},        // v20
        {QVector3D(WORLD_DIM, WORLD_DIM - 0.1, WORLD_DIM), QVector2D(0.49f, 0.99f)},         // v21
        {QVector3D(-WORLD_DIM, WORLD_DIM - 0.1, -WORLD_DIM), QVector2D(0.26f, 2.0f / 3.0f)}, // v22
        {QVector3D(WORLD_DIM, WORLD_DIM - 0.1, -WORLD_DIM), QVector2D(0.49f, 2.0f / 3.0f)}   // v23
    };

    GLushort indices[] = {
        1, 0, 3, 2, 2,          // Face 0 (Front)
        5, 5, 4, 7, 6, 6,       // Face 1 (Right)
        9, 9, 8, 11, 10, 10,    // Face 2 (Rear)
        13, 13, 12, 15, 14, 14, // Face 3 (Left)
        17, 17, 16, 19, 18, 18, // Face 4 (Bottom)
        21, 21, 20, 23, 22      // Face 5 (Top)
    };

    skyVertBuf.bind();
    skyVertBuf.allocate(vertices, sizeof(vertices));

    skyFacetsBuf.bind();
    skyFacetsBuf.allocate(indices, sizeof(indices));
}

void GeometryEngine::drawTreeGeometry(QOpenGLShaderProgram *program)
{
    // Cycle through the "object sections"
    for (int i = 0; i < treeFacetsBuf.size(); i++)
    {
        program->setUniformValue("texture", 0);
        treeTexture[i]->bind();

        treeVertBuf[i].bind();
        treeFacetsBuf[i].bind();

        // Running calculation of offsets
        quintptr offset = 0;

        //
        // Connect shader plumbing
        //

        // vertex positions
        int vertexLocation = program->attributeLocation("a_position");
        program->enableAttributeArray(vertexLocation);
        program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(vertexData));
        offset += sizeof(QVector3D);

        // texture coordinates
        int texcoordLocation = program->attributeLocation("a_texcoord");
        program->enableAttributeArray(texcoordLocation);
        program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(vertexData));
        offset += sizeof(QVector2D);

        // normals
        int normalLocation = program->attributeLocation("a_normal");
        program->enableAttributeArray(normalLocation);
        program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(vertexData));

        // Draw it (aka spew our chunks)
        for (int j = 0; j < facetChunk[i].size(); j++)
        {
            glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, facetChunk[i][j].count, GL_UNSIGNED_SHORT, NULL, facetChunk[i][j].base);
            // cout << "  " << i << "," << j << ": base=" << facetChunk[i][j].base << "   count=" << facetChunk[i][j].count << endl;
        }
    }
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

    glDrawElements(GL_TRIANGLE_STRIP, skyFacetsBuf.size() / sizeof(GLushort), GL_UNSIGNED_SHORT, NULL);
}

void GeometryEngine::drawWaterGeometry(QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    waterVertBuf.bind();
    waterFacetsBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(vertexData));

    // Offset for texture coordinate (aka uvs)
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(vertexData));

    // Offset for normals
    offset += sizeof(QVector2D);

    // Tell OpenGL programmable pipeline how to locate vertex normal data
    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(vertexData));

    // Now the plumbing is hooked up, draw what's in the buffer!
    glDrawElements(GL_TRIANGLE_STRIP, waterFacetsBuf.size() / sizeof(GLushort), GL_UNSIGNED_SHORT, NULL);
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
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(vertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(vertexData));

    // Offset for normals
    offset += sizeof(QVector2D);

    // Tell OpenGL programmable pipeline how to locate vertex normal data
    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(vertexData));

    // Now the plumbing is hooked up, draw what's in the buffer!
    glDrawElements(GL_TRIANGLE_STRIP, landFacetsBuf.size() / sizeof(GLuint), GL_UNSIGNED_INT, 0);
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
void GeometryEngine::diamondSquare(int size, bool presetCenter)
{
    int half = size / 2;
    if (half < 1)
        return;

    //square steps - skip if center point is pre-set
    if (!presetCenter)
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
    avg += Frand(reach / TERRAIN_SMOOTH) - reach / (TERRAIN_SMOOTH * 2.0f);
    avg /= float(count);
    landVerts[Coord_2on1(x, z)].position.setY(avg);
}

void GeometryEngine::diamondStep(int x, int z, int reach)
{
    int count = 0;
    float avg = 0.0f;
    if (x - reach >= 0)
    {
        avg += landVerts[Coord_2on1(x - reach, z)].position.y();
        count++;
    }
    if (x + reach < LAND_DIVS)
    {
        avg += landVerts[Coord_2on1(x + reach, z)].position.y();
        count++;
    }
    if (z - reach >= 0)
    {
        avg += landVerts[Coord_2on1(x, z - reach)].position.y();
        count++;
    }
    if (z + reach < LAND_DIVS)
    {
        avg += landVerts[Coord_2on1(x, z + reach)].position.y();
        count++;
    }
    avg += Frand(reach / TERRAIN_SMOOTH) - reach / (TERRAIN_SMOOTH * 2.0f);
    avg /= float(count);
    landVerts[Coord_2on1(x, z)].position.setY(avg);
}

// return the y height of the land grid at (x,z)
float GeometryEngine::getHeight(float wx, float wz, bool stayAbove)
{
    // Translate world coordinates to land vertex coordinates
    int x = int((wx + WORLD_DIM) * LAND_DIVS / (WORLD_DIM * 2.0f) + 0.5f);
    int z = int((wz + WORLD_DIM) * LAND_DIVS / (WORLD_DIM * 2.0f) + 0.5f);
    if (stayAbove)
        return (MAX(landVerts[Coord_2on1(x, z)].position.y(), waterLevel)); // Don't go below water
    else
        return (landVerts[Coord_2on1(x, z)].position.y());
}

bool GeometryEngine::adjustViewerPos(QVector3D &viewerPos, QVector2D searchDir)
{
    // Find the edge of the water.
    float y = getHeight(viewerPos.x(), viewerPos.z(), false);
    if (y < waterLevel)
    {
        while (y < waterLevel)
        {
            viewerPos.setX(viewerPos.x() - searchDir.x());
            viewerPos.setZ(viewerPos.z() - searchDir.y());

            if (viewerPos.x() < -WORLD_DIM || viewerPos.x() > WORLD_DIM || viewerPos.z() < -WORLD_DIM || viewerPos.z() > WORLD_DIM)
                return (false);
            y = getHeight(viewerPos.x(), viewerPos.z(), false);
        }
    }
    else
    {
        while (y >= waterLevel)
        {
            viewerPos.setX(viewerPos.x() + searchDir.x());
            viewerPos.setZ(viewerPos.z() + searchDir.y());
            if (viewerPos.x() < -WORLD_DIM || viewerPos.x() > WORLD_DIM || viewerPos.z() < -WORLD_DIM || viewerPos.z() > WORLD_DIM)
                return (false);
            y = getHeight(viewerPos.x(), viewerPos.z(), false);
        }
    }

    // Now we found the edge of the water.  Back off a set amount
    viewerPos.setX(viewerPos.x() - searchDir.x() * WATER_START_PROX);
    viewerPos.setZ(viewerPos.z() - searchDir.y() * WATER_START_PROX);

    return (true);
}

float GeometryEngine::closestTree(float x, float z)
{
    float minDist = FLT_MAX;
    for (int i = 0; i < TREE_COUNT; i++)
    {
        float xd = treeSpot[i].x() - x;
        float zd = treeSpot[i].z() - z;
        minDist = MIN(minDist, xd * xd + zd * zd);
    }
    return sqrt(minDist);
}

void GeometryEngine::placeTrees(void)
{
    float x, y, z;
    for (int i = 0; i < TREE_COUNT; i++)
    {
        do
        {
            x = Frand(WORLD_DIM * 2) - WORLD_DIM;
            z = Frand(WORLD_DIM * 2) - WORLD_DIM;
            y = getHeight(x, z, false);
        } while (y < waterLevel || closestTree(x, z) < TREE_MIN_PROX);

        treeSpot[i] = QVector4D(x, y - TREE_SINK, z, TREE_RANGE_L + Frand(TREE_RANGE_H - TREE_RANGE_L));
    }
}
