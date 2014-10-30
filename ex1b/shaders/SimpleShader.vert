#version 330

uniform float radius;
uniform vec2 offset;
layout(location = 0) in vec4 position;

void main()
{
    gl_Position = vec4(position[0] * radius + offset[0],
                       position[1] * radius + offset[1], 
                       position[2], 
                       position[3]);
}
