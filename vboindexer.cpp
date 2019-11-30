/****************************************************************************
**
** Adapted from VBO Indexer code from the VBO tutorial at http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
**
** This is a utility function to consolidate three indexed arrays (vertices, texture coords, and normals) with disparate indices (as
** is found in obj files) into a single packed array with a single index as required by OpenGL VBO's
**
** Specifically, this is based from the example code at https://github.com/opengl-tutorials/ogl/blob/master/common/vboindexer.cpp
** I have adapted it to the Qt5 class library and removed uneeded functions
**
****************************************************************************/

#include <vector>
#include <map>

#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

#include "vboindexer.h"
#include <string.h> // for memcmp
#include <math.h>   // for fabs

// Returns true iif v1 can be considered equal to v2
bool is_near(float v1, float v2)
{
    return fabs(v1 - v2) < 0.01f;
}

struct PackedVertex
{
    QVector3D position;
    QVector2D uv;
    QVector3D normal;
    bool operator<(const PackedVertex that) const
    {
        return memcmp((void *)this, (void *)&that, sizeof(PackedVertex)) > 0;
    };
};

bool getSimilarVertexIndex(
    PackedVertex &packed,
    std::map<PackedVertex, GLuint> &VertexToOutIndex,
    GLuint &result)
{
    // Use a hashed dictionary (std::map) for fast searching
    std::map<PackedVertex, GLuint>::iterator it = VertexToOutIndex.find(packed);
    if (it == VertexToOutIndex.end())
    {
        return false;
    }
    else
    {
        result = it->second;
        return true;
    }
}

void indexVBO(
    std::vector<QVector3D> &in_vertices,
    std::vector<QVector2D> &in_uvs,
    std::vector<QVector3D> &in_normals,
    std::vector<GLuint> &out_indices,
    std::vector<QVector3D> &out_vertices,
    std::vector<QVector2D> &out_uvs,
    std::vector<QVector3D> &out_normals)
{
    std::map<PackedVertex, GLuint> VertexToOutIndex;

    // For each input vertex
    for (unsigned int i = 0; i < in_vertices.size(); i++)
    {
        PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};

        // Try to find a similar vertex in out_XXXX
        GLuint index;
        bool found = getSimilarVertexIndex(packed, VertexToOutIndex, index);

        if (found)
        { // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back(index);
        }
        else
        { // If not, it needs to be added in the output data.
            out_vertices.push_back(in_vertices[i]);
            out_uvs.push_back(in_uvs[i]);
            out_normals.push_back(in_normals[i]);
            GLuint newindex = (GLuint)out_vertices.size() - 1;
            out_indices.push_back(newindex);
            VertexToOutIndex[packed] = newindex;
        }
    }
}
