#ifndef __PBD_SHADER_H__
#define __PBD_SHADER_H__

#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

//#include <cuda_runtime.h>
//#include <cuda_gl_interop.h>


class Shader
{
public:
	GLuint program;
	GLuint vaoHandle;

	GLuint positionBufferHandle;
	GLuint normalBufferHandle;
	GLuint colorBufferHandle;
	GLuint depthBufferHandle;

public:
	Shader(void);
	~Shader(void);

public:
	char* readShaderSource(const char* shaderFile);
	void InitShader(const char* vShaderFile, const char* fShaderFile);
};

#endif

