
#version 430 compatibility

uniform sampler2D colorMap;
uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D lightMap;
uniform float far;
uniform float near;
uniform float depthMapWidth;
uniform float depthMapHeight;

uniform float show_depth;
uniform float show_normal;

float mask[25] = {0,0,1,0,0,0,1,2,1,0,1,2,-16,2,1,0,1,2,1,0,0,0,1,0,0};
float linearizeDepth(float exp_depth) {
//	return  ((2.0 * near) / (far + near -  exp_depth * (far - near))); 
	return clamp((exp_depth - near)/(far - near),0,1);
}
float depthToScreen(float exp_depth) {
	return  (far+near-(2*near/exp_depth))/(far-near); 
}
float rand(float seed){
	float res = cos(fract(sin(dot(vec2(seed, seed - 5) ,vec2(12.9898,78.233))) * 43758.5453));
    return abs(res);
}
void main()
{
	vec3 depth_outline;
	vec3 color_inside;
	vec3 normal_outline;
	float depth;
	vec3 normalv;
	depth = texture2D(depthMap, gl_TexCoord[0].xy).x;
	depth = linearizeDepth(depth);
	normalv = texture2D(normalMap, gl_TexCoord[0].xy).xyz;
	int ii,jj, kk;
	int i, j;
	float sum=0;
	vec3 sumv = vec3(0);
	for(kk=0;kk<25;kk++)
	{
		i = kk/5;
		j = kk%5;
		float you = texture2D(depthMap, gl_TexCoord[0].xy + vec2(1.0f*(i-3)/depthMapWidth, 1.0f*(j-3)/depthMapHeight)).x;
		you = linearizeDepth(you);
		sum += you * mask[kk];

		vec3 youv = texture2D(normalMap, gl_TexCoord[0].xy + vec2(1.0f*(i-3)/depthMapWidth, 1.0f*(j-3)/depthMapHeight)).xyz;
		sumv += youv * mask[kk];
	}
	float light = texture2D(lightMap, gl_TexCoord[0].xy).x;
	light = clamp(light,0,1);
	if(light < 0.5)
	{
		light = light;
	}
	else
	{
		light = light;
	}

//	sum = depthToScreen(sum);
	depth_outline = vec3(abs(sum)) * show_depth;
	color_inside = vec3(texture2D(colorMap, gl_TexCoord[0].xy)) * light;
//	normal_outline = vec3(sum)*39;
	normal_outline = vec3(abs(dot(sumv,normalv))) * show_normal;
	gl_FragColor = vec4(depth_outline + normal_outline + color_inside,1.0f);

//	gl_FragColor = texture2D(depthMap, gl_TexCoord[0].xy);
}