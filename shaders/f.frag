#version 330 core

out vec4 gl_FragColor;

uniform vec3 colour;

void main()
{
    gl_FragColor = vec4(colour,1);
}
