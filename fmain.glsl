/****************************************************************************
**
** Fragment shader with per-pixel lighting and texture mapping
**
** Adapted from "cube" example code from the Qt5 library, class example 27, 
**  and some vermillion book thrown in.
**
** cube example: https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
** Technique for texture cutouts based on alpha-channel:
**   https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Transparent_Textures
**
****************************************************************************/
varying vec2 v_texcoord;
varying vec3 View;
varying vec3 Light;
varying vec3 Normal;

uniform sampler2D texture;
uniform vec3 MatAmbient;
uniform vec3 MatDiffuse;
uniform vec3 MatSpecular;
uniform float MatShininess;


void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(Light);
    vec3 R = reflect(-L,N);
    vec3 V = normalize(View);

    float Id = max(dot(L,N) , 0.0);
    float Is = (Id>0.0) ? pow(max(dot(R,V),0.0) , MatShininess) : 0.0;

    // Sum color types
    vec4 color = vec4(MatAmbient + Id*MatDiffuse + Is*MatSpecular, 1.0);

    //  Apply texture
    gl_FragColor = color * texture2D(texture,v_texcoord);

    // Discard ("cutout") any fragments with post-texture alpha less than threshold 0.5
    if (gl_FragColor.a < 0.5)
        discard;
}
