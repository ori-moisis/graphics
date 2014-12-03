#version 330

in vec4 color;

layout(pixel_center_integer) in vec4 gl_FragCoord;
out vec4 outColor;

void main()
{
	outColor = color;
}
