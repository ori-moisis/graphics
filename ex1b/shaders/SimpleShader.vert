#version 330

uniform mat4 transform;

layout(location = 0) in vec4 position;

void main()
{
	gl_Position = transform * position;
}
