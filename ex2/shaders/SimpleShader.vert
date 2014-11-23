#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int isArcball;

layout(location = 0) in vec4 position;

out vec4 color;

void main()
{
	if (isArcball == 1)
	{
		color = vec4(1,0,0,0);
	}
	else
	{
		color = ((model * position) + 1) / 2;
	}
    gl_Position = projection * view * model * position;
}
