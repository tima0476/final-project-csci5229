/****************************************************************************
**
** Fragment shader with per-pixel lighting and texture mapping with alpha-
** channel based cutouts
**
** Adapted from example code:
**   https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
**
** Technique for texture cutouts based on alpha-channel:
**   https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Transparent_Textures
**
****************************************************************************/

uniform vec3 lightPosition;
uniform sampler2D texture;
uniform vec4 MatAmbient;
uniform vec4 MatDiffuse;
uniform vec4 MatSpecular;
uniform float MatShininess;

varying vec2 v_texcoord;
varying vec3 N;
varying vec3 v;    

void main (void)  
{  
    vec3 L = normalize(lightPosition - v);   
    vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)  
    vec3 R = normalize(-reflect(L,N));  

    //calculate Ambient Term:  
    vec4 Iamb = MatAmbient;    

    //calculate Diffuse Term:  
    vec4 Idiff = MatDiffuse * max(dot(N,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);     

    // calculate Specular Term:
    vec4 Ispec = MatSpecular 
                * pow(max(dot(R,E),0.0),0.3*MatShininess);
    Ispec = clamp(Ispec, 0.0, 1.0); 

    // write Total Color:  
    gl_FragColor = (Iamb + Idiff + Ispec) * texture2D(texture,v_texcoord);

    if (gl_FragColor.a < 0.5)
        discard;
}
          