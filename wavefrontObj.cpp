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

#include "wavefrontObj.h"

#include <iostream>
#include <string>

using namespace std;

wavefrontObj::wavefrontObj(QString filename)
{
    loadObj(filename);
}

bool wavefrontObj::loadObj(QString filename)
{
    QString fn(":/obj/" + filename);
    QFile infile(fn);
    if (infile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&infile);
        QString line;
        while (in.readLineInto(&line))
        {
            if (line.isEmpty())
            {
                continue; // skip blank lines else token[0] will segfault
            }
            QStringList token = line.split(' ', QString::SkipEmptyParts);
            QString cmd = token[0]; // The first token on a line is the command
            if (cmd == "o")
            {
                // Object name.  Everything after the command is the name
                data.name = line.right(line.size() - cmd.size()).trimmed();
            }
            else if (cmd == "v")
            {
                // Vertex coordinates (spec says can be 3 or 4, but we will assume only 3)
                data.v << QVector3D(token[1].toFloat(), token[2].toFloat(), token[3].toFloat());
            }
            else if (cmd == "vn")
            {
                // Normal coordinates (always 3)
                data.vn << QVector3D(token[1].toFloat(), token[2].toFloat(), token[3].toFloat());
            }
            else if (cmd == "vt")
            {
                // Texture coordinates (always 2)
                data.vt << QVector2D(token[1].toFloat(), token[2].toFloat());
            }
            else if (cmd == "f")
            {
                // Facets.  Just stuff vertex #'s into data structures here.  They will be consolidated and converted
                // to OpenGL VBO's external to this class

                // Make sure there is a section available to add facets to
                if (data.section.isEmpty())
                    data.section.resize(1);

                int n = token.size() - 1;
                for (int i = 1; i <= n; i++)
                {
                    QStringList indices = token[i].split('/', QString::KeepEmptyParts);
                    if (indices.size() == 3)
                    {
                        // v//vn format will automagically be handled correctly because split() with KeepEmptyParts
                        // option will return empty string in position 1, and that empty string will parse to zero
                        indexTriple t(indices[0].toUInt(), indices[1].toUInt(), indices[2].toUInt(), i == 1 || i == n);
                        data.section.last().f << t;
                    }
                    else if (indices.size() == 1)
                    {
                        indexTriple t(indices[0].toUInt(), 0, 0, i == 1 || i == n);
                        data.section.last().f << t;
                    }
                    else
                    {
                        cerr << "Invalid facet " << token[i].toStdString() << endl;
                        return false;
                    }
                }
            }
            else if (cmd == "usemtl")
            {
                // Switch the material.
                setMaterial(token[1]);
            }
            else if (cmd == "mtllib")
            {
                // Load a materials file
                loadMaterialFile(token[1]);
            }
            // add handing for more commands here.  For now, anything not handled above  will just be ignored
        }
    }
    else
    {
        cerr << "Cannot open file " << filename.toStdString() << endl;
#ifdef DEBUG_OBJ
        debugDump();
#endif // DEBUG_OBJ
        return false;
    }
#ifdef DEBUG_OBJ
    debugDump();
#endif // DEBUG_OBJ
    return true;
}

bool wavefrontObj::setMaterial(QString name)
{
    // Search materials for a matching name
    for (int i = 0; i < material.size(); i++)
    {
        if (material[i].name == name)
        {
            // Match!  Start a new objectSection with this material
            objectSection s;
            s.mtl = material[i];
            data.section << s;

            return true;
        }
    }
    cerr << "Unknown material " << name.toStdString() << endl;
    return false;
}

bool wavefrontObj::loadMaterialFile(QString filename)
{
    QString fn(":/obj/" + filename);
    QFile infile(QString(":/obj/") + filename);
    if (infile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&infile);
        QString line;
        while (in.readLineInto(&line))
        {
            if (line.isEmpty())
            {
                continue; // skip blank lines else token[0] will segfault
            }

            QStringList token = line.split(' ', QString::SkipEmptyParts);

            QString cmd = token[0]; // First token on a line is the command
            if (cmd == "newmtl")
            {
                // New Material - start a materialData struct
                if (token.size() == 2)
                {
                    materialData md;
                    md.name = token[1];

                    material << md;
                }
                else
                {
                    cerr << "Warning while parsing " << filename.toStdString() << ": malformed newmtl command" << endl;
                }
            }
            else if (material.isEmpty())
            {
                // If no materials named yet, then skip this line
            }
            else if (cmd == "Ka")
            {
                // Three floats for the ambient color
                if (token.size() == 4)
                {
                    material.last().Ka.setX(token[1].toFloat());
                    material.last().Ka.setY(token[2].toFloat());
                    material.last().Ka.setZ(token[3].toFloat());
                }
                else
                {
                    cerr << "Warning while parsing " << filename.toStdString() << ": malformed Ka command" << endl;
                }
            }
            else if (cmd == "Kd")
            {
                // Three floats for the ambient color
                if (token.size() == 4)
                {
                    material.last().Kd.setX(token[1].toFloat());
                    material.last().Kd.setY(token[2].toFloat());
                    material.last().Kd.setZ(token[3].toFloat());
                }
                else
                {
                    cerr << "Warning while parsing " << filename.toStdString() << ": malformed Kd command" << endl;
                }
            }
            else if (cmd == "Ks")
            {
                // Three floats for the ambient color
                if (token.size() == 4)
                {
                    material.last().Ks.setX(token[1].toFloat());
                    material.last().Ks.setY(token[2].toFloat());
                    material.last().Ks.setZ(token[3].toFloat());
                }
                else
                {
                    cerr << "Warning while parsing " << filename.toStdString() << ": malformed Ks command" << endl;
                }
            }
            else if (cmd == "Ns")
            {
                // One float for the Material shininess
                if (token.size() == 2)
                {
                    material.last().Ns = token[1].toFloat();
                }
                else
                {
                    cerr << "Warning while parsing " << filename.toStdString() << ": malformed Ns command" << endl;
                }
            }
            else if (cmd == "d")
            {
                // One float for the dissolve factor
                if (token.size() == 2)
                {
                    material.last().d = token[1].toFloat();
                }
                else
                {
                    cerr << "Warning while parsing " << filename.toStdString() << ": malformed d command" << endl;
                }
            }
            else if (cmd == "map_Kd")
            {
                // Texture.  Everything after the command is the filename
                material.last().map_d_filename = ":/obj/" + line.right(line.size() - token[0].size()).trimmed();
            }
        }
    }
    else
    {
        cerr << "Cannot open materials file " << filename.toStdString() << endl
             << endl;
        return false;
    }

    return true;
}

// A better way would be to subclass the QVector classes and provide toStr() methods, but this is more expedient
// in the short term...
#define V2(X) "(" << (X).x() << "," << (X).y() << ")"
#define V3(X) "(" << (X).x() << "," << (X).y() << "," << (X).z() << ")"
#define V4(X) "(" << (X).x() << "," << (X).y() << "," << (X).z() << "," << (X).w() << ")"

void wavefrontObj::debugDump(void)
{
    cout << "wavefrontObj::debugDump()..." << endl;

    cout << "Name = '" << data.name.toStdString() << "'" << endl;

    cout << data.v.size() << " vertices:" << endl;
    for (int i = 0; i < data.v.size(); i++)
        cout << "  " << V3(data.v[i]) << endl;

    cout << data.vt.size() << " texture coordinates:" << endl;
    for (int i = 0; i < data.vt.size(); i++)
        cout << "  " << V2(data.vt[i]) << endl;

    cout << data.vn.size() << " normals:" << endl;
    for (int i = 0; i < data.vn.size(); i++)
        cout << "  " << V3(data.vn[i]) << endl;

    cout << data.section.size() << " object sections:" << endl;

    for (int i = 0; i < data.section.size(); i++)
    {
        cout << "  Dump section " << i + 1 << "..." << endl;
        objectSection *s = &data.section[i];
        cout << "    Material '" << s->mtl.name.toStdString() << "'" << endl;
        cout << "                  Ns = " << s->mtl.Ns << endl;
        cout << "                  Ka = " << V4(s->mtl.Ka) << endl;
        cout << "                  Kd = " << V4(s->mtl.Kd) << endl;
        cout << "                  Ks = " << V4(s->mtl.Ks) << endl;
        cout << "                   d = " << s->mtl.d << endl;
        cout << "      map_d_filename = '" << s->mtl.map_d_filename.toStdString() << "'" << endl;
        cout << "    " << s->f.size() << " facets:" << endl;
        for (int j = 0; j < s->f.size(); j++)
        {
            cout << "      v=" << s->f[j].v << "  vt=" << s->f[j].vt << "  vn=" << s->f[j].vn;
            if (s->f[j].edge)
                cout << " EDGE";
            cout << endl;
        }
    }

    cout << endl
         << endl;
}