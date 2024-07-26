#version 330 compatibility
in vec2 gST;
uniform sampler2D uTexUnit2;

void
main()
{
    vec3 newcolor = texture(uTexUnit2, gST).rgb;
    gl_FragColor = vec4(newcolor, 1.);
}