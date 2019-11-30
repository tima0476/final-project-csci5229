/****************************************************************************
**
** Adapted from "cube" example code from the Qt library and class example 27
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
** Fragment shader with Phong vertex lighting and texture mapping
**
****************************************************************************/

uniform sampler2D texture;
varying vec2 v_texcoord;


void main()
{
   gl_FragColor = gl_Color * texture2D(texture, v_texcoord);
}

//-------------------------------------------------------------------------------
//  Set the fragment color (ex27 texture.frag)
    // uniform sampler2D tex;

    // void main()
    // {
    //    gl_FragColor = gl_Color * texture2D(tex,gl_TexCoord[0].xy);
    // }

//
// Qt example program hellogl2
//
    // varying highp vec3 vert;
    // varying highp vec3 vertNormal;

    // void main() {
    //    highp vec3 L = normalize(lightPos - vert);
    //    highp float NL = max(dot(normalize(vertNormal), L), 0.0);
    //    highp vec3 color = vec3(0.39, 1.0, 0.0);
    //    highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);
    //    gl_FragColor = vec4(col, 1.0);
    // }

//
//  Per Pixel Lighting shader with texture - adapted from class example 27
//
    // varying vec3 View;
    // varying vec3 Light;
    // varying vec3 Normal;
    // uniform sampler2D tex;

    // void main()
    // {
    //    //  N is the object normal
    //    vec3 N = normalize(Normal);
    //    //  L is the light vector
    //    vec3 L = normalize(Light);
    //    //  R is the reflected light vector R = 2(L.N)N - L
    //    vec3 R = reflect(-L,N);
    //    //  V is the view vector (eye vector)
    //    vec3 V = normalize(View);

    //    //  Diffuse light is cosine of light and normal vectors
    //    float Id = max(dot(L,N) , 0.0);
    //    //  Specular is cosine of reflected and view vectors
    //    float Is = (Id>0.0) ? pow(max(dot(R,V),0.0) , gl_FrontMaterial.shininess) : 0.0;

    //    //  Sum color types
    //    vec4 color = gl_FrontMaterial.emission
    //               + gl_FrontLightProduct[0].ambient
    //               + Id*gl_FrontLightProduct[0].diffuse
    //               + Is*gl_FrontLightProduct[0].specular;

    //    //  Apply texture
    //    gl_FragColor = color * texture2D(tex,gl_TexCoord[0].xy);
    // }


