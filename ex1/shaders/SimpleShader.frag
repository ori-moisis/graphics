#version 330

uniform vec4 fillColor;
uniform int squareSize;

layout(pixel_center_integer) in vec4 gl_FragCoord;
out vec4 outColor;

void main()
{
	if ((int(gl_FragCoord.x / squareSize) % 2) + (int(gl_FragCoord.y / squareSize) % 2) != 1) {
		outColor = fillColor;
	} else {
		outColor = vec4(0, 0 ,0 , 1.0);
	}
}
