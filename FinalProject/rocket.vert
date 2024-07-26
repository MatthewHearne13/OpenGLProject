#version 330 compatibility

// out variables to be interpolated in the rasterizer and sent to each fragment shader:

out  vec3  vN;	  // normal vector
out  vec3  vL;	  // vector from point to light
out  vec3  vE;	  // vector from point to eye
out  vec2  vST;	  // (s,t) texture coordinates
out  vec3  vMC;   // model coordinates
out  float vFlapCounter; // Counts the flaps

uniform float uFlapWings; // used to make dragon flap wings

// where the light is:

const vec3 LightPosition = vec3(  0., 5., 5. );

vec4
RotateClockwise( float uFlapWings, vec4 n, float xo, float yo )
{
        
        float cz = cos( uFlapWings );
        float sz = sin( uFlapWings );

		         
        // rotate about z:
        float xp =  (n.x-xo)*cz + (n.y-yo)*sz + xo;    // x'
        n.y      = -(n.x-xo)*sz + (n.y-yo)*cz + yo;    // y'
		n.x      =  xp;                  
		return n;
}
vec4
RotateCounterClockwise( float uFlapWings, vec4 n, float xo, float yo )
{
        
        float cz = cos( uFlapWings );
        float sz = sin( uFlapWings );

		         
        // rotate about z:
        float xp =  (n.x-xo)*cz - (n.y-yo)*sz + xo;    // x'
        n.y      = (n.x-xo)*sz + (n.y-yo)*cz + yo;    // y'
		n.x      =  xp;                 // 
		return n;
}

void
main( )
{
	vec4 vert = gl_Vertex;
	vMC = gl_Vertex.xyz;
	
	if (vert.x >= 5.)
	{
		vert = RotateClockwise(uFlapWings, vert, 5., 12.);
	}
	else if (vert.x <= -5.)
	{
		vert = RotateCounterClockwise(uFlapWings, vert, -5., 12.);
	}	
	vST = gl_MultiTexCoord0.st;
	
	vec4 ECposition = gl_ModelViewMatrix * gl_Vertex;

	vN = normalize( gl_NormalMatrix * gl_Normal );  // normal vector

	vL = LightPosition - ECposition.xyz;	    // vector from the point
							// to the light position
	vE = ECposition.xyz - vec3( 0., 0., 0. );       // vector from the point
							// to the eye position
	
	gl_Position = gl_ModelViewProjectionMatrix * vert;
}
