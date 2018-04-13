#version 330 core
layout(location = 0) in vec3 v_position;
layout(location = 2) in vec3 v_texCoords;

uniform mat4 view;
out vec2 texCoords;

void main()
{
  texCoords = vec2(v_texCoords.x,1-v_texCoords.y);
  gl_Position = view*vec4(v_position, 1.0);
}
