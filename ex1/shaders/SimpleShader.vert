#version 330

uniform float radius;
layout(location = 0) in vec4 position;

void main()
{
    gl_Position = vec4(position[0] * radius ,position[1] * radius, position[2], position[3]);
}
