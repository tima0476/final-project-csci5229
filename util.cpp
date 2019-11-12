//
// Utility function class
//
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "util.h"

using namespace std;

//
// Reverse an array of n bytes
//
void util::reverse(void * x, int n)
{
    int k;
    char * ch = (char *)x;
    for (k=0; k<n/2; k++)
    {
        char tmp = ch[k];
        ch[k] = ch[n-1-k];
        ch[n-1-k] = tmp;
    }
}

//
// Load an OpenGL texture from a BMP file
//
unsigned int util::load_tex_bmp(const char * file)
{
    unsigned int   texture;    // Texture name
    unsigned short magic;      // Image magic
    unsigned int   dx,dy,size; // Image dimensions
    unsigned short nbp,bpp;    // Planes and bits per pixel
    unsigned char* image;    // Image data
    unsigned int   off;        // Image offset
    unsigned int   k;          // Counter
    // int            max;        // Maximum texture dimensions

    //  Open file
    ifstream f(file, ios::in | ios::binary );
    if (!f.is_open()) 
        throw runtime_error( string("Cannot open file ") + file);

    //  Check image magic
    f.read((char *)(&magic),2);
   
    if (magic!=0x4D42 && magic!=0x424D) 
        throw runtime_error( string("Image magic not BMP in ") + file);

    // Read some of the BMP header
    f.seekg(8, f.cur);      // skip 8 bytes
    f.read((char *)&off, 4);                // read 4-byte image offset
    f.seekg(4, f.cur);      // skip 4 bytes
    f.read((char *)&dx, 4);                 // read 4-byte image width
    f.read((char *)&dy, 4);                 // read 4-byte image height
    f.read((char *)&nbp, 2);                // read 2-byte # bit planes
    f.read((char *)&bpp, 2);                // read 2-byte # bits per pixel
    f.read((char *)&k, 4);                  // read 4-byte counter 

    //  Reverse bytes on big endian hardware (detected by backwards magic)
    if (magic==0x424D)
    {
        reverse(&off,4);
        reverse(&dx,4);
        reverse(&dy,4);
        reverse(&nbp,2);
        reverse(&bpp,2);
        reverse(&k,4);
    }

    //  Check image parameters
    // glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
    // if (dx<1 || (int)dx>max) throw logic_error(string(file)+" image width "+to_string(dx)+" out of range1-"+to_string(max));
    // if (dy<1 || (int)dy>max) throw logic_error(string(file)+" image height "+to_string(dy)+string(" out of range 1-")+to_string(max));
    if (nbp!=1)  throw logic_error(string(file)+" bit planes is not 1: "+to_string(nbp));
    if (bpp!=24) throw logic_error(string(file)+" bits per pixel is not 24: "+to_string(bpp));
    if (k!=0)    throw logic_error(string(file)+" compressed files not supported");
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k*=2);
   if (k!=dx) throw logic_error(string(file)+string(" image width not a power of two: "+string(dx));
   for (k=1;k<dy;k*=2);
   if (k!=dy) throw logic_error(string(file)+string(" image height not a power of two: "+string(dy));
#endif

    //  Allocate image memory
    size = 3*dx*dy;
    image = new unsigned char[size];

    if (!image) throw logic_error(string("Cannot allocate ")+to_string(size)+" bytes of memory for image "+string(file));

    //  Seek to and read image
    f.seekg(off, f.beg);
    f.read((char *)image, size);
    if (f.rdstate() & f.failbit) throw logic_error(string("Error reading data from image "+string(file)));
    f.close();

    //  Reverse colors (BGR -> RGB)
    for (k=0;k<size;k+=3)
    {
        unsigned char temp = image[k];
        image[k]   = image[k+2];
        image[k+2] = temp;
    }

   //  Sanity check
   err_check("LoadTexBMP");

   //  Generate 2D texture
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);

   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,3,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) throw logic_error(string("Error in glTexImage2D ")+string(file)+" "+to_string(dx)+"x"+to_string(dy));

   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   delete[] image;

   //  Return texture name
   return texture;
}

void util::err_check(const char* where)
{
   int err = glGetError();
   if (err) cerr << "ERROR: " << gluErrorString(err) << "[" << where << "]";
}
