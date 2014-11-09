#version 330

uniform mat4 transform[10];

layout(location = 0) in vec4 position;

flat out int instanceID;

void main()
{
	gl_Position = transform[gl_InstanceID] * position;
	instanceID = gl_InstanceID;
}
