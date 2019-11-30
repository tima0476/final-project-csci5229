/****************************************************************************
**
** Adapted from VBO Indexer code from the VBO tutorial at http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
**
** This is a utility function to consolidate three indexed arrays (vertices, texture coords, and normals) with disparate indices (as
** is found in obj files) into a single packed array with a single index as required by OpenGL VBO's
**
** Specifically, this is based from the example code at https://github.com/opengl-tutorials/ogl/blob/master/common/vboindexer.hpp
** I have adapted it to the Qt5 class library and removed uneeded functions
**
****************************************************************************/

#ifndef VBOINDEXER_HPP
#define VBOINDEXER_HPP

void indexVBO(
    std::vector<QVector3D> &in_vertices,
    std::vector<QVector2D> &in_uvs,
    std::vector<QVector3D> &in_normals,
    std::vector<GLuint> &out_indices,
    std::vector<QVector3D> &out_vertices,
    std::vector<QVector2D> &out_uvs,
    std::vector<QVector3D> &out_normals
);

#endif
