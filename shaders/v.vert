#version 330 core
layout(location = 0) in vec3 v_position;

uniform mat4 view;

void main()
{
  gl_Position = view*vec4(v_position, 1.0);
}
