#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int shininess;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out vec4 color;

void main()
{
	vec3 lightColor1 = vec3(1.0, 0.9, 0.7); // First light color
	vec3 lightColor2 = vec3(0.6, 0.6, 1.0); // Second light color
	vec3 ambientColor = vec3(1.0, 1.0, 1.0); // Ambient light color
	
	vec3 lightPosition1 = vec3(  3.0, 2.0,  1.0); // First light position
	vec3 lightPosition2 = vec3( -3.0, 0.0,  1.0); // Second light position
	
	vec3 ka = vec3(0.1, 0.1, 0.1); // Ambient coefficient
	vec3 kd = vec3(0.3, 0.3, 0.3); // Diffuse coefficient
	vec3 ks = vec3(0.3, 0.3, 0.3); // Specular coefficient


	gl_Position = projection * view * model * position;
	
	vec4 posForLight = view * model * position;	
	vec4 normalForLight = normalize(view * model * normal);
	vec3 v = normalize(posForLight.xyz);
	
	float d1 = distance(lightPosition1, posForLight.xyz);
	float d2 = distance(lightPosition2, posForLight.xyz);
	float a = 0.0;
	float b = 0.02;
	float c = 0.01;
	float d1factor = max(0.1, a + b*d1 + c*pow(d1,2));
	float d2factor = max(0.1, a + b*d2 + c*pow(d2,2));
	
	vec3 l1 = normalize(lightPosition1 - posForLight.xyz);
	vec3 l2 = normalize(lightPosition2 - posForLight.xyz);
	vec3 r1 = normalize(reflect(normalForLight.xyz, l1));
	vec3 r2 = normalize(reflect(normalForLight.xyz, l2));
	
	
	vec3 color1 = (kd * lightColor1 * max(0, dot(l1, normalForLight.xyz))) / d1factor;
	color1 += (ks * lightColor1 * pow(max(0, dot(v, r1)), shininess)) / d1factor;
	
	vec3 color2 = (kd * lightColor2 * max(0, dot(l2, normalForLight.xyz))) / d2factor;
	color2 += (ks * lightColor2 * pow(max(0, dot(v, r2)), shininess)) / d2factor;
	
	vec3 color3 = ka * ambientColor;
	
	color.xyz = color1 + color2 + color3;
}
