#version 330 compatibility

uniform float        uA;  // Amplitude
uniform float        uB;  // Period
uniform float        uC;  // Phase shift
uniform float        uD;  // Decay Rate


// out variables to be interpolated in the rasterizer and sent to each fragment shader:

out  vec3  vN;	  // normal vector
out  vec3  vL;	  // vector from point to light
out  vec3  vE;	  // vector from point to eye
out  vec2  vST;	  // (s,t) texture coordinates
out  vec3  vMC;   // model coordinates

// where the light is:

const vec3 LightPosition = vec3(  0., 5., 5. );
const float PI = 3.14;

void
main( )
{
	vST = gl_MultiTexCoord0.st;
	vMC = gl_Vertex.xyz;
	vec4 ECposition = gl_ModelViewMatrix * gl_Vertex;

	//vN = normalize( gl_NormalMatrix * gl_Normal );  // normal vector

	vL = LightPosition - ECposition.xyz;	    // vector from the point
							// to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point
							// to the eye position

	//Add equation for displacement mapping
	float x = gl_Vertex.x;
	float y = gl_Vertex.y;

	float r = sqrt(pow(x, 2) + pow(y, 2));
	float dzdr = uA * ( -sin(2.*PI*uB*r+uC) * 2.*PI*uB * exp(-uD*r) + cos(2.*PI*uB*r+uC) * -uD * exp(-uD*r) );

	
	float drdx = x / r;
	float drdy = y / r;

	float dzdx = dzdr * drdx;
	float dzdy = dzdr * drdy;

	vec3 Tx = vec3(1., 0., dzdx );
	vec3 Ty = vec3(0., 1., dzdy );

	vN = normalize( cross( Tx, Ty));

	vec3 vert = gl_Vertex.xyz;
	vert.z = uA * cos(2*PI*uB*r+uC) * exp(-uD*r);
		
	gl_Position = gl_ModelViewProjectionMatrix * vec4(vert, 1.);
}
