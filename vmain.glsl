/****************************************************************************
**
** Vertex shader with per-pixel lighting, texture mapping, and transparency
**
** Adapted from example code:
**   https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
**
****************************************************************************/

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform mat3 normalMatrix;

attribute vec4 a_position;  // bind this to vertex coordinate array
attribute vec3 a_normal;    // Array of normals
attribute vec2 a_texcoord;  // Array of texture coordinates 

varying vec2 v_texcoord;
varying vec3 N;
varying vec3 v;

void main(void)  
{     
    v = vec3(mv_matrix * a_position);       
    N = normalize(normalMatrix * a_normal);

    v_texcoord = a_texcoord;    // texture coordinate pass-through
    gl_Position = mvp_matrix * a_position;  
}
          