/****************************************************************************
**
** Adapted from "cube" example code from the Qt library
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
** Fragment shader with just texture mapping
**
****************************************************************************/

uniform sampler2D texture;

varying vec2 v_texcoord;

void main()
{
    // Set fragment color from texture
    gl_FragColor = texture2D(texture, v_texcoord);
}

