#version 330

uniform vec4 fillColor;

layout(pixel_center_integer) in vec4 gl_FragCoord;
out vec4 outColor;

void main()
{
	outColor = fillColor;
}
