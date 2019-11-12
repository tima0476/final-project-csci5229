#ifndef __UTIL_H__
#define __UTIL_H__

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


// Perfectly imperfect transcendental functions to avoid nepharious cases
#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))

class util 
{
    //
    // AttributesC
    //C
private:
protected:
private:

    //
    // Methods
    //
private:
    void reverse(void *, int);
protected:
public:
    unsigned int load_tex_bmp(const char *);
    void err_check(const char* where);
};

#endif // __UTIL_H__