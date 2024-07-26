#version 330 compatibility
in vec2 vST;
uniform sampler2D uTexUnit1;

void
main()
{
    vec3 newcolor = texture(uTexUnit1, vST).rgb;
    gl_FragColor = vec4(newcolor, 1.);
}