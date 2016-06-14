

#version 430 compatibility

out vec3 Position;
out vec3 Normal;
out vec3 Color;

uniform float pointRadius;
uniform float pointScale;


void main()
{		
	Position = vec3(gl_ModelViewMatrix * gl_Vertex);
	Normal = normalize(gl_NormalMatrix * gl_Normal);
	Color = vec3(gl_Color);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	//

	vec3 posEye = vec3(gl_ModelViewMatrix * gl_Vertex);

    float dist = length(posEye);
    gl_PointSize = pointRadius * (pointScale / dist);

    gl_TexCoord[0] = gl_MultiTexCoord0;

    gl_FrontColor = vec4(Color, 1.0);
}

