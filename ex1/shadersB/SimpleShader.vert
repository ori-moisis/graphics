#version 330

uniform mat4 perspective;
uniform mat4 transform[32];

layout(location = 0) in vec4 position;

flat out int instanceID;
out mat4 pers;

void main()
{
	gl_Position = perspective * transform[gl_InstanceID] * position;
	instanceID = gl_InstanceID;
	pers = perspective;
}
