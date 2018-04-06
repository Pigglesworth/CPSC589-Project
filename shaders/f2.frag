#version 330 core

in vec3 normal;

out vec4 gl_FragColor;

void main()
{
    float shading = max(0.5,dot(vec3(0,0,-1),normalize(normal)));
	vec3 col = shading*vec3(1.0,1.0,1.0);
    gl_FragColor = vec4(col,1);
}
