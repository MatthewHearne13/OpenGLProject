#version 330 compatibility
 #extension GL_EXT_gpu_shader4: enable
 #extension GL_EXT_geometry_shader4: enable
 layout( triangles )  in;
 layout( points, max_vertices=200 )  out;

 uniform int uLevel;
 uniform float uGravity;
 uniform float uTime;
 uniform float uVelScale;

 vec3    V0, V01, V02;
 vec3    CG;
 in vec2 vST[3];
 out vec2 gST;
 gST = vST;

 void
ProduceVertex( float s, float t)
{
    vec3 v = V0 + s*V01 + t*V02;
    vec3 vel = uVelScale * (v - CG);
    v = CG + vel*uTime + 0.5*vec3(0.,uGravity, 0.)*uTime*uTime;
    gl_Position = gl_ProjectionMatrix * vec4( v, 1.);
    EmitVertex( );
}

void
main()
{
    V01 = ( gl_PositionIn[1] - gl_PositionIn[0]).xyz;
    V02 = ( gl_PositionIn[2] - gl_PositionIn[0]).xyz;
    V0 = gl_PositionIn[0].xyz;
    CG = ( gl_PositionIn[0].xyz + gl_PositionIn[1].xyz + gl_PositionIn[2].xyz) / 3.;

    int numLayers = 1;

    float dt = 1. / float(numLayers);
    float t = 1.;

    for( int it = 0; it <= numLayers; it++ )
    {
        float smax = 1. - t;
        int nums = it + 1;
        float ds = smax / float( nums- 1 );
        float s = 0.;
 
        for( int is = 0; is < nums; is++ )
        {
            ProduceVertex( s, t );
            s += ds;
        }

        t -= dt;
    }
}