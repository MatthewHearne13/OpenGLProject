#version 330 compatibility
uniform float  uKa, uKd, uKs;   //lighting coefficients
uniform float  uShininess;
in vec2  vST;
in vec3  vN;
in vec3  vL;
in vec3  vE;
void main ( )
{
    vec3 Normal = normalize(vN);
    vec3 Light  = normalize(vL);
    vec3 Eye    = normalize(vE);

    vec3 myColor = vec3(0.0, 1.0, 0.0);
    vec3 mySpecularColor = vec3(1.0, 1.0, 1.0);

    // possibly change myColor

    // Fragment Lighting Portion
    vec3 ambient = uKa * myColor;
    float d = 0.;
    float s = 0.;
    if( dot(Normal,Light) > 0. )   // only do specular if the light can see the point
    {
        d = dot(Normal,Light);
        vec3 ref = normalize( reflect( -Light, Normal));
        s = pow( max( dot(Eye, ref), 0.), uShininess);   
    }
    vec3 diffuse  = uKd * d * myColor;
    vec3 specular = uKs * s * mySpecularColor;
    gl_FragColor = vec4( ambient + diffuse + specular, 1.);
}