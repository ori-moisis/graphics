#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int shininess;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec4 color;

void main()
{
	gl_Position = projection * view * model * position;
	
	vec3 lightColor1 = vec3(1.0, 0.9, 0.7); // First light color
	vec3 lightColor2 = vec3(0.6, 0.6, 1.0); // Second light color
	vec3 ambientColor = vec3(1.0, 1.0, 1.0); // Ambient light color
	
	vec3 lightPosition1 = vec3(  3.0, 2.0,  1.0); // First light position
	vec3 lightPosition2 = vec3( -3.0, 0.0,  1.0); // Second light position
	
	vec3 ka = vec3(0.1, 0.1, 0.1); // Ambient coefficient
	vec3 kd = vec3(0.3, 0.3, 0.3); // Diffuse coefficient
	vec3 ks = vec3(0.3, 0.3, 0.3); // Specular coefficient

	vec3 l1 = lightPosition1 - gl_Position.xyz;
	vec3 l2 = lightPosition2 - gl_Position.xyz;
	vec3 r1 = reflect(l1, normal);
	vec3 r2 = reflect(l2, normal);
	
	vec3 color1 = kd * lightColor1 * dot(l1, normal) + ks * lightColor1 * (int(dot(gl_Position.xyz, r1)) ^ shininess);
	vec3 color2 = kd * lightColor2 * dot(l2, normal) + ks * lightColor2 * (int(dot(gl_Position.xyz, r2)) ^ shininess);
	

	color.xyz = color1 + color2 + ka * ambientColor;
}
