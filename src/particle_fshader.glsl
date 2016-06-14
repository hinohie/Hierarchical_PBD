

#version 430 compatibility

in vec3 Position;
in vec3 Normal;
in vec3 Color;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragNormal;
layout(location = 2) out vec4 FragLight;

void main()
{
	
	vec4 LightPosition =  gl_ModelViewMatrix * gl_LightSource[0].position;
	vec4 LightPosition2 = gl_ModelViewMatrix * vec4(-64.0, 164.0, 64.0, 1.0);
	vec4 LightPosition3 = gl_ModelViewMatrix * vec4(0.0, -64.0, 0.0, 1.0);
	vec3 LightIntensity = vec3(1.0, 1.0, 1.0);

	/*
	vec3 Ka = vec3(0.05,0.05,0.05);
	vec3 Kd = vec3(0.9,0.9,0.9);
	vec3 Ks = vec3(0.05,0.05,0.05);
	float shininess = 100.0f;
	*/
	vec3 Ka = vec3(gl_FrontMaterial.ambient * (gl_LightSource[0].ambient + gl_LightModel.ambient));
	vec3 Kd = vec3(gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse);
	vec3 Ks = vec3(gl_FrontMaterial.specular * gl_LightSource[0].specular);
	float shininess = 100;

	vec3 n = normalize(Normal);
	vec3 s = normalize(vec3(LightPosition) - Position);
	vec3 s2 = normalize(vec3(LightPosition2) - Position);
	vec3 s3 = normalize(vec3(LightPosition3) - Position);
	vec3 v = normalize(vec3(-Position));
	vec3 r = reflect(-s, n);
	vec3 r2 = reflect(-s2, n);
	vec3 r3 = reflect(-s3, n);

	FragColor = vec4 (
						Color 
		, 1.0); 
	FragNormal = vec4(n,1.0);
	FragLight = vec4 (
//						Color *
						LightIntensity * (
						Ka + (
						Kd*max(dot(s,n),0.0) +
						Ks*pow(max(dot(r,v),0.0), shininess))
						/*+	0.2*(
						Kd*max(dot(s2,n),0.0) +
						Ks*pow(max(dot(r2,v),0.0), shininess)) + 0.5*(
						Kd*max(dot(s3,n),0.0) +
						Ks*pow(max(dot(r3,v),0.0), shininess))
						*/
						)
		, 1.0); 

//	FragColor = vec4 (vec3(1.0) - vec3(clamp((gl_FragCoord.z -0.92) * 10,0,1)) ,1.0);
}

