#version 330 core
layout(location = 0) in vec3 v_position;
layout(location = 2) in vec3 v_normal;

out vec3 normal;

uniform mat4 view;

void main()
{
  gl_Position = view*vec4(v_position, 1.0);
  normal = v_normal;
}
