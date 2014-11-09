#version 330

uniform vec2 resolution;
uniform vec4 lightLocation;
uniform vec4 fillColor[10];
uniform mat4 transform[10];

layout(pixel_center_integer) in vec4 gl_FragCoord;
flat in int instanceID;

out vec4 outColor;

float get_screen_x(float logical_x)
{
	return (logical_x + 1) * resolution.x / 2;
}

float get_screen_y(float logical_y)
{
	return resolution.y - ((resolution.y * (1-logical_y)) / 2);
}

vec4 to_screen_coord(vec4 point)
{
	vec4 res = point;
	res.x = get_screen_x(point.x);
	res.y = get_screen_y(point.y);
	return res;
}

void main()
{
	vec4 white = vec4(1,1,1,1);
	vec4 black = vec4(0,0,0,1);
	float lightFadeFactor = 0.25;
	float lightRadius = length(transform[instanceID] * vec4(300, 0, 0, 0));
	vec4 screenLightCenter = to_screen_coord(transform[instanceID] * lightLocation);
	
	// Add highlight according to distance from lightCenter (closer means more white)
	outColor = mix(white, fillColor[instanceID], pow(distance(gl_FragCoord, screenLightCenter) / lightRadius, lightFadeFactor));
}
