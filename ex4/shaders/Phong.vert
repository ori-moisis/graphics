#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out vec4 interNormal;
out vec4 interPos;

void main()
{
    gl_Position = projection * view * model * position;
    interNormal = normalize(view * model * normal);
    interPos = position;
}
