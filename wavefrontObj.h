/****************************************************************************
**
** A class and assiated data structures for parsing and storing 3d object 
** definitions from Wavefront obj format data files.  Note:  This is not a 
** full implementation of the spec (ref: http://www.martinreddy.net/gfx/3d/OBJ.spec).  
** Instead, this only implements the minimum necessary for this application.
**
** Based on class example 26; adapted to Qt5 & C++ OO by Timothy Mason
**
****************************************************************************/
#ifndef __WAVEFRONT_OBJ_H__
#define __WAVEFRONT_OBJ_H__

#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLTexture>
#include <QFile>
#include <QTextStream>

#define DEBUG_OBJ

struct materialData
{
    QString name;
    float Ns;               // Specular exponent
    QVector4D Ka, Kd, Ks;   // Ambient, Diffuse, and Specular colors
    float d;                // transparency [0..1]; 0.0 = fully transparent, 1.0 = fully opaque
    QString map_d_filename; // Alpha texture map filename

    materialData(QString name = "") : name(name),
                                      Ns(0),
                                      Ka(0, 0, 0, 1),
                                      Kd(0, 0, 0, 1),
                                      Ks(0, 0, 0, 1),
                                      d(0) {}
};

struct indexTriple
{
    // Facet indices (obj files index the v, vt, and vn arrays independently)
    GLuint v, vt, vn;

    indexTriple(GLuint v = 0, GLuint vt = 0, GLuint vn = 0) : v(v),
                                                              vt(vt),
                                                              vn(vn) {}
};

struct objectSection
{
    materialData mtl;       // the material for this section of an object
    QVector<indexTriple> f; // the list of indices for the facets of this section
};

struct objectData
{
    QString name; // "o" tag

    QVector<QVector3D> v;  // vertex coordinate
    QVector<QVector2D> vt; // texture coordinate
    QVector<QVector3D> vn; // normal vector

    QVector<objectSection> section; // One object can have multiple sections, with disparate materials
};

class wavefrontObj
{
public:
    wavefrontObj(QString filename);
    bool loadObj(QString filename);
    void debugDump(void);

protected:
private:
    QVector<materialData> material; // .mtl files are parsed and the materials are stored here
    objectData data;                // Main storage

    bool setMaterial(QString name);
    bool loadMaterialFile(QString filename);
};
#endif // __WAVEFRONT_OBJ_H__