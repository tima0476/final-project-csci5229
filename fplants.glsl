/****************************************************************************
**
** Fragment shader with per-pixel lighting and texture mapping
**
** Adapted from "cube" example code from the Qt5 library, class example 27, 
**  and some vermillion book thrown in.
**
** cube example: https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
** Oh! and this helped too:
** https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Transparent_Textures
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
uniform float shininess;


void main()
{
//    gl_FragColor = gl_Color * texture2D(texture, v_texcoord);
    //  N is the object normal
    vec3 N = normalize(Normal);
    //  L is the light vector
    vec3 L = normalize(Light);
    //  R is the reflected light vector R = 2(L.N)N - L
    vec3 R = reflect(-L,N);
    //  V is the view vector (eye vector)
    vec3 V = normalize(View);

    //  Diffuse light is cosine of light and normal vectors
    float Id = max(dot(L,N) , 0.0);
    //  Specular is cosine of reflected and view vectors
    float Is = (Id>0.0) ? pow(max(dot(R,V),0.0) , 1.0) : 0.0;

    //  Sum color types
    // vec4 color = gl_FrontMaterial.emission
    //             + gl_FrontLightProduct[0].ambient
    //             + Id*gl_FrontLightProduct[0].diffuse
    //             + Is*gl_FrontLightProduct[0].specular;

    // Hack:  For now, just hard-code the material properties.  Figure out how to plug material properties into
    //          Qt sometime later.
    vec4 color = vec4(0.6,0.6,0.6,1)       // Ambient
                + Id*vec4(0.3,0.3,0.3,1)   // Diffuse
                + Is*vec4(0.2,0.2,0.2,1);     // Specular

    //  Apply texture
    gl_FragColor = color * texture2D(texture,v_texcoord);
    if (gl_FragColor.a < 0.5)
    {
        discard;
    }
}
