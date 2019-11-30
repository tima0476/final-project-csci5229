/****************************************************************************
**
** Adapted from "cube" example code from the Qt library and class example 27
**   https://doc.qt.io/qt-5/qtopengl-cube-example.html
**
** vertex shader with Phong vertex lighting and texture mapping
**
****************************************************************************/

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;

attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;

void main()
{
    vec3 P = vec3(mv_matrix * a_position);          // transformed vertex coordinate position
    vec3 N = normalize(normalMatrix * a_normal);    // transformed vertex normal
    vec3 L = normalize(lightPosition - P);          // vector from P towards the light
    vec3 R = reflect(-L,N);                         // reflected light vector
    vec3 V = normalize(-P);                         // view vector (eye is at origin - will be translated by the model-view matrix)

    float Id = max(dot(L,N) , 0.0);
    //  Shininess intensity is cosine of light and reflection vectors to a power
    float Is = (Id>0.0) ? pow(max(dot(R,V) , 0.0) , gl_FrontMaterial.shininess) : 0.0;

    //  Vertex color
    // gl_FrontColor = gl_FrontLightProduct[0].ambient
    //             + Id*gl_FrontLightProduct[0].diffuse
    //             + Is*gl_FrontLightProduct[0].specular;


    gl_FrontColor = vec4(0.5,0.5,0.5,1.0)
                + Id*vec4(1.0,1.0,0.0,1.0)
                + Is*vec4(0.1,0.1,0.1,1.0);

    v_texcoord = a_texcoord;
    gl_Position = mvp_matrix * a_position;
}

//-------------------------------------------------------------------------------
//  Phong lighting (ex27 texture.vert)

    // void main()
    // {
    //    //  P is the vertex coordinate on body
    //    vec3 P = vec3(gl_ModelViewMatrix * gl_Vertex);
    //    //  N is the object normal at P
    //    vec3 N = normalize(gl_NormalMatrix * gl_Normal);
    //    //  Light Position for light 0
    //    vec3 LightPos = vec3(gl_LightSource[0].position);
    //    //  L is the light vector
    //    vec3 L = normalize(LightPos - P);
    //    //  R is the reflected light vector R = 2(L.N)N - L
    //    vec3 R = reflect(-L, N);
    //    //  V is the view vector (eye at the origin)
    //    vec3 V = normalize(-P);

    //    //  Diffuse light intensity is cosine of light and normal vectors
    //    float Id = max(dot(L,N) , 0.0);
    //    //  Shininess intensity is cosine of light and reflection vectors to a power
    //    float Is = (Id>0.0) ? pow(max(dot(R,V) , 0.0) , gl_FrontMaterial.shininess) : 0.0;

    //    //  Vertex color
    //    gl_FrontColor = gl_FrontLightProduct[0].ambient
    //               + Id*gl_FrontLightProduct[0].diffuse
    //               + Is*gl_FrontLightProduct[0].specular;

    //    //  Texture
    //    gl_TexCoord[0] = gl_MultiTexCoord0;

    //    //  Return fixed transform coordinates for this vertex
    //    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    // }

// Qt example program hellogl2
    // attribute vec4 vertex;
    // attribute vec3 normal;
    // varying vec3 vert;
    // varying vec3 vertNormal;
    // uniform mat4 projMatrix;
    // uniform mat4 mvMatrix;
    // uniform mat3 normalMatrix;
    // void main() {
    //    vert = vertex.xyz;
    //    vertNormal = normalMatrix * normal;
    //    gl_Position = projMatrix * mvMatrix * vertex;
    // }



//  Per Pixel Lighting shader with texture - adapted from class example 27

    // varying vec3 View;
    // varying vec3 Light;
    // varying vec3 Normal;

    // void main()
    // {
    //    //  Vertex location in modelview coordinates
    //    vec4 P = gl_ModelViewMatrix * gl_Vertex;
    //    //  Light position
    //    Light  = gl_LightSource[0].position.xyz - P.xyz;
    //    //  Normal
    //    Normal = gl_NormalMatrix * gl_Normal;
    //    //  Eye position
    //    View  = -P.xyz;
    //    //  Texture
    //    gl_TexCoord[0] = gl_MultiTexCoord0;
    //    //  Set vertex position
    //    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    // }


