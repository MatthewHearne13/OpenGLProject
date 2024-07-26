#version 330 compatibility

// lighting uniform variables -- these can be set once and left alone:
uniform float       uKa, uKd, uKs;	 // coefficients of each type of lighting -- make sum to 1.0
uniform vec3        uColor;		 // object color
uniform float       uShininess;	 // specular exponent
uniform float       uNoiseAmp;
uniform float       uNoiseFreq;
uniform sampler3D   Noise3;
uniform float       uEta = 2.5; 
uniform float       uMix;
uniform float       uWhiteMix;
uniform samplerCube uReflectUnit;
uniform samplerCube	uRefractUnit;
uniform float       uWhiteOrRed;
uniform float 		uWhiteorBlack;
//want to use the same shader, but want the uWhiteMix color to be dark red for the booster.  
// square-equation uniform variables -- these should be set every time Display( ) is called:

uniform float   uS0, uT0, uD;

// in variables from the vertex shader and interpolated in the rasterizer:

in  vec3  vN;		   // normal vector
in  vec3  vL;		   // vector from point to light
in  vec3  vE;		   // vector from point to eye
in  vec2  vST;		   // (s,t) texture coordinates
in  vec3  vMC;         // model coordinate positions

vec4  WHITE = vec4(uWhiteOrRed, uWhiteorBlack, uWhiteorBlack, 1.);

vec3
RotateNormal( float angx, float angy, vec3 n )
{
        float cx = cos( angx );
        float sx = sin( angx );
        float cy = cos( angy );
        float sy = sin( angy );

        // rotate about x:
        float yp =  n.y*cx - n.z*sx;    // y'
        n.z      =  n.y*sx + n.z*cx;    // z'
        n.y      =  yp;
        // n.x      =  n.x;

        // rotate about y:
        float xp =  n.x*cy + n.z*sy;    // x'
        n.z      = -n.x*sy + n.z*cy;    // z'
        n.x      =  xp;                 // n.y      =  n.y;

        return normalize( n );
}

void
main( )
{
	float s = vST.s;
	float t = vST.t;

	vec4 uSpecularColor = vec4(1., 1., 1., 1.);

		
	vec4 nvx = texture( Noise3, uNoiseFreq*vMC );
	float angx = nvx.r + nvx.g + nvx.b + nvx.a  -  2.;  	// -1. to +1.
	angx *= uNoiseAmp;

	vec4 nvy = texture( Noise3, uNoiseFreq*vec3(vMC.xy,vMC.z+0.5) );
	float angy = nvy.r + nvy.g + nvy.b + nvy.a  -  2.;	// -1. to +1.
	angy *= uNoiseAmp;


	// apply the per-fragment lighting to myColor:

	vec3 Normal = normalize(vN);
	vec3 Light  = normalize(vL);
	vec3 Eye    = normalize(vE);

	vec3 newNormal = RotateNormal( angx, angy, vN );
	newNormal = normalize( gl_NormalMatrix * newNormal );

	vec3 reflectVector = reflect( vE, newNormal);
	vec4 reflectColor  = texture( uReflectUnit, reflectVector);

	vec3 refractVector = refract( vE, newNormal, uEta );
	vec4 refractColor;
	if( all( equal( refractVector, vec3(0.,0.,0.) ) ) )
	{
		refractColor = reflectColor;
	}
	else
	{
		refractColor = texture( uRefractUnit, refractVector );
		refractColor = mix( refractColor, WHITE, uWhiteMix );
	}

	//checking to find right X coord
	vec3 vert = vMC; 
	
	vec4 color = mix( refractColor, reflectColor, uMix);
	
	
	gl_FragColor = vec4( color );
}

