#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec4 position;
in vec4 normal;

out vec4 interNormal;
out vec4 interPos;

void main()
{
    gl_Position = projection * view * model * position;
    interNormal = normalize(view * model * normal);
    interPos = position;
}
