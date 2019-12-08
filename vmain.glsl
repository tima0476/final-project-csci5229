/****************************************************************************
**
** Vertex shader with per-pixel lighting, texture mapping, and transparency
**
** Adapted from "cube" example code from the Qt5 library and class example 27
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
****************************************************************************/

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;

attribute vec4 a_position;  // bind this to vertex coordinate array
attribute vec3 a_normal;    // Array of normals
attribute vec2 a_texcoord;  // Array of texture coordinates 

varying vec2 v_texcoord;
varying vec3 View;
varying vec3 Light;
varying vec3 Normal;


void main()
{
    vec4 P = mv_matrix * a_position;
    Light  = lightPosition.xyz - P.xyz;
    Normal = normalMatrix * a_normal;
    View  = -P.xyz;
    v_texcoord = a_texcoord;
    gl_Position = mvp_matrix * a_position;
}
