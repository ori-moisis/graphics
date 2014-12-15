#version 330

uniform mat4 projection;

in vec4 position;

out vec4 color;

void main()
{
	color = vec4(1,0,0,0);
    gl_Position = projection * position;
}
