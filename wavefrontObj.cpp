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
            if (cmd == "v")
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
                    data.section << objectSection();

                // Double-stuff the first and last vertex triple for each facet to delineate discrete polys
                int n = token.size() - 1;
                for (int i = 1; i <= n; i++)
                {
                    QStringList indices = token[i].split('/', QString::KeepEmptyParts);
                    if (indices.size() == 3)
                    {
                        // v//vn format will automagically be handled correctly because split() with KeepEmptyParts
                        // option will return empty string in position 1, and that empty string will parse to zero
                        indexTriple t(indices[0].toUInt(), indices[1].toUInt(), indices[2].toUInt());
                        data.section.last().f << t;
                        if (i == 1 || i == n)
                        {
                            // double-stuff to mark start and end of poly for VBO
                            data.section.last().f << t;
                        }
                    }
                    else if (indices.size() == 1)
                    {
                        indexTriple t(indices[0].toUInt(), 0, 0);
                        data.section.last().f << t;
                        if (i == 1 || i == n)
                        {
                            // double-stuff to mark start and end of poly for VBO
                            data.section.last().f << t;
                        }
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
                    cerr << "Error parsing " << filename.toStdString() << ": malformed newmtl command" << endl;
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
                    cerr << "Error parsing " << filename.toStdString() << ": malformed Ka command" << endl;
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
                    cerr << "Error parsing " << filename.toStdString() << ": malformed Kd command" << endl;
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
                    cerr << "Error parsing " << filename.toStdString() << ": malformed Ks command" << endl;
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
                    cerr << "Error parsing " << filename.toStdString() << ": malformed Ns command" << endl;
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

void wavefrontObj::debugDump(void)
{
    cout << "wavefrontObj::debugDump()" << endl;
    cout << endl
         << endl;
}