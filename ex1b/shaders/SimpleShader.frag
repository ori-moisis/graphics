#version 330

uniform vec4 fillColor;
uniform vec4 lightCenter;
uniform float lightRadius;
uniform float lightFadeFactor;
uniform float shadowRadius;
uniform float shadowFadeFactor;


layout(pixel_center_integer) in vec4 gl_FragCoord;

out vec4 outColor;

void main()
{
	vec4 white = vec4(1,1,1,1);
	vec4 black = vec4(0,0,0,1);
	// Add highlight according to distance from lightCenter (closer means more white)
	vec4 highlightMix = mix(white, fillColor, pow(distance(gl_FragCoord, lightCenter) / lightRadius, lightFadeFactor));
	// Add shadow according to distance from lightCenter (farther means more black)
	outColor = mix(highlightMix, black, pow(distance(gl_FragCoord, lightCenter) / shadowRadius, shadowFadeFactor));
}
