#version 330 core

uniform sampler2D tex;

in vec2 texCoords;
out vec4 gl_FragColor;

void main()
{
    vec4 color = texture(tex,texCoords);
	if(color.a < 0.1) discard;
	gl_FragColor = color;
}
