#version 330

uniform mat4 model;
uniform mat4 view;
uniform int shininess;
uniform int texScale;
uniform int turbCoeff;
uniform int texMode;

uniform sampler2D myTexture;

in vec4 interNormal;
in vec4 interPos;

layout(pixel_center_integer) in vec4 gl_FragCoord;
out vec4 outColor;



// snoise and turb from here:
// http://glsl.heroku.com/e#812.1

vec3 mod289(vec3 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
	return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
{
	const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
	const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);
	
	// First corner
	vec3 i  = floor(v + dot(v, C.yyy) );
	vec3 x0 =   v - i + dot(i, C.xxx) ;
	
	// Other corners
	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min( g.xyz, l.zxy );
	vec3 i2 = max( g.xyz, l.zxy );
	
	//   x0 = x0 - 0.0 + 0.0 * C.xxx;
	//   x1 = x0 - i1  + 1.0 * C.xxx;
	//   x2 = x0 - i2  + 2.0 * C.xxx;
	//   x3 = x0 - 1.0 + 3.0 * C.xxx;
	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
	vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y
	
	// Permutations
	i = mod289(i);
	vec4 p = permute( permute( permute(
									   i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
							  + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
					 + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
	
	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	float n_ = 0.142857142857; // 1.0/7.0
	vec3  ns = n_ * D.wyz - D.xzx;
	
	vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)
	
	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)
	
	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);
	
	vec4 b0 = vec4( x.xy, y.xy );
	vec4 b1 = vec4( x.zw, y.zw );
	
	//vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
	//vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
	vec4 s0 = floor(b0)*2.0 + 1.0;
	vec4 s1 = floor(b1)*2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));
	
	vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
	vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;
	
	vec3 p0 = vec3(a0.xy,h.x);
	vec3 p1 = vec3(a0.zw,h.y);
	vec3 p2 = vec3(a1.xy,h.z);
	vec3 p3 = vec3(a1.zw,h.w);
	
	//Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	
	// Mix final noise value
	vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
	m = m * m;
	return 42.0 * dot( m*m, vec4(dot(p0,x0), dot(p1,x1),
								 dot(p2,x2), dot(p3,x3) ) );
}

float turb(vec3 v)
{
	const float base_freq = 0.2; // SPIDER
	
	vec4 noisevec;
	noisevec.x = snoise(v * base_freq*1.0) * 8.0;
	noisevec.y = snoise(v * base_freq*2.0) * 4.0;
	noisevec.z = snoise(v * base_freq*4.0) * 2.0;
	noisevec.w = snoise(v * base_freq*8.0) * 1.0;
	// noisevec = (noisevec / 8.0 + 1.0) / 2.0;
	noisevec = noisevec / 8.0;
	// noisevec = noisevec * noisevec;
	
	float intensity = abs(noisevec[0] - 0.20) +
	abs(noisevec[1] - 0.10) +
	abs(noisevec[2] - 0.05) +
	abs(noisevec[3] - 0.025);
	return intensity;
}





void main()
{
	float pi = 3.141592653589793;
	vec3 lightColor1 = vec3(1.0, 0.9, 0.7); // First light color
	vec3 lightColor2 = vec3(0.6, 0.6, 1.0); // Second light color
	vec3 ambientColor = vec3(1.0, 1.0, 1.0); // Ambient light color
	
	vec3 lightPosition1 = vec3(  3.0, 2.0,  1.0); // First light position
	vec3 lightPosition2 = vec3( -3.0, 0.0,  1.0); // Second light position
	
	vec3 ka = vec3(0.1, 0.1, 0.1); // Ambient coefficient
	vec3 kd = vec3(0.3, 0.3, 0.3); // Diffuse coefficient
	vec3 ks = vec3(0.3, 0.3, 0.3); // Specular coefficient
	
	vec3 darkWood = vec3(0.2f, 0.1f, 0.02f);
	vec3 lightWood = vec3(0.5f, 0.3f, 0.12f);
	
	vec4 texPos = model * interPos;
	vec4 posForLight = view * model * interPos;
	vec4 normalForLight = interNormal;
	vec3 v = normalize(posForLight.xyz);
	
	float d1 = distance(lightPosition1, posForLight.xyz);
	float d2 = distance(lightPosition2, posForLight.xyz);
	float a = 0.0;
	float b = 0.0;
	float c = 0.01;
	float d1factor = max(0.1, a + b*d1 + c*pow(d1,2));
	float d2factor = max(0.1, a + b*d2 + c*pow(d2,2));
	

	vec3 l1 = normalize(lightPosition1 - posForLight.xyz);
	vec3 l2 = normalize(lightPosition2 - posForLight.xyz);
	vec3 r1 = normalize(reflect(normalForLight.xyz, l1));
	vec3 r2 = normalize(reflect(normalForLight.xyz, l2));
	
	
	if (texMode == 1) {
		float sinParam = texScale * (texPos.x + turb((turbCoeff / 10.0) * (texPos.zyx + 1)));
		float bla = sin(pi * (sinParam + 1));
		float kdVal = bla * bla * bla;
		kd = vec3((kdVal + 0.5) / 2);
	}
	if (texMode == 2) {
		texPos = texPos + 1;
		float d = texScale * sqrt(texPos.y * texPos.y + texPos.z * texPos.z) + (turbCoeff / 10.0f) * turb(texPos.zyx);		
		float cosParam = 2 * pi * (d - floor(d));
		float woodParam = abs(cos(cosParam));
		kd = mix(darkWood, lightWood, woodParam);
		ks = vec3(0);
	}
	if (texMode == 3) {
		// Find the reflect direction
		vec4 vReflect = normalize(reflect(posForLight, normalForLight));
		float theta = (atan(vReflect.x, vReflect.z)) / pi;
		float phi = (atan(vReflect.y, sqrt((vReflect.x * vReflect.x) + (vReflect.z * vReflect.z)))) * 2 / pi;
		
		
		outColor.xyz = vec3(vReflect.x / vReflect.z);
		
		//outColor = vec4((theta + 1) / 2);
		//outColor = texture(myTexture, vec2(theta, phi));
		return;
	}
	
	vec3 color1 = (kd * lightColor1 * max(0, dot(l1, normalForLight.xyz))) / d1factor;
	color1 += (ks * lightColor1 * pow(max(0, dot(v, r1)), shininess)) / d1factor;
	
	vec3 color2 = (kd * lightColor2 * max(0, dot(l2, normalForLight.xyz))) / d2factor;
	color2 += (ks * lightColor2 * pow(max(0, dot(v, r2)), shininess)) / d2factor;
	
	outColor.xyz = color1 + color2 + ka * ambientColor;
}
