#include "PBDobject.h"
#include<stdlib.h>
#include<glut.h>
int PBDparticle::cur_id = 0;

static int depthMapWidth = 800;
static int depthMapHeight = 800;
void PBDobject::bind_texture()
{
		GLenum FBOstatus;
	
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	// Try to use a texture depth component
	/*Init Buffer Start*/
	glGenTextures(1, &initDepthTextureId_front);
	glBindTexture(GL_TEXTURE_2D, initDepthTextureId_front);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthMapWidth, depthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//glGenRenderbuffersEXT(1, &initDepthTextureId_front);
	//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, initDepthTextureId_front);
	//glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,depthMapWidth, depthMapHeight);
	// Try to use a texture depth component
	glGenTextures(1, &initColorTextureId_front);
	glBindTexture(GL_TEXTURE_2D, initColorTextureId_front);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, depthMapWidth, depthMapHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glGenTextures(1, &initNormalTextureId_front);
	glBindTexture(GL_TEXTURE_2D, initNormalTextureId_front);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, depthMapWidth, depthMapHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glGenTextures(1, &initLightTextureId_front);
	glBindTexture(GL_TEXTURE_2D, initLightTextureId_front);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, depthMapWidth, depthMapHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*
	unsigned char*	texSnowImage;
	int w = 73;
	int h = 73;
	texSnowImage = SOIL_load_image("sanhak_proto.png", &w, &h, 0, SOIL_LOAD_RGBA);
	glGenTextures(1, &texSnowObj);
	glBindTexture(GL_TEXTURE_2D, texSnowObj);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w ,h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texSnowImage);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_BLEND);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	*/
	// create a framebuffer object


//	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, initDepthTextureId_front);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, initDepthTextureId_front, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D, initColorTextureId_front, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,GL_TEXTURE_2D, initNormalTextureId_front, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT,GL_TEXTURE_2D, initLightTextureId_front, 0);
	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	GLenum *bufs = new GLenum[3];
	bufs[0] = GL_COLOR_ATTACHMENT0_EXT;
	bufs[1] = GL_COLOR_ATTACHMENT1_EXT;
	bufs[2] = GL_COLOR_ATTACHMENT2_EXT;
	glDrawBuffers(3, bufs);
//	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

	// attach the texture to FBO depth attachment point


	// check FBO status
	FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
void PBDobject::bind()
{
	int i, j, k;
	if(positionData) delete[] positionData;
	if(normalData) delete[] normalData;
	if(h_positionData) delete[] positionData;
	if(h_normalData) delete[] normalData;
	k = f.size();

	h_positionData =  new float[12*k];
	h_normalData = new float[12*k];
	update();

	bind_texture();
}
#include<hash_map>
void PBDobject::update()
{
	int i, j, k;
	j=p.size();
	k=f.size();
	hash_map<int, vector3f> normal_mean;
	hash_map<int, int> normal_mean_num;
	for(i=0;i<j;i++)
	{
		normal_mean[p[i]->id] = vector3f();
		normal_mean_num[p[i]->id] = 0;
	}
	for(i=0;i<k;i++)
	{
		PBDface * face = f[i];
		if(!face->alive) continue;
		int id;
		
		vector3f n1;
		vector3f p01 = f[i]->t2->pos - f[i]->t1->pos;
		vector3f p12 = f[i]->t3->pos - f[i]->t2->pos;
		n1 = p01 % p12;
		n1.normalize();

		id = face->id1;
		normal_mean[id] += n1;
		normal_mean_num[id] ++;
		id = face->id2;
		normal_mean[id] += n1;
		normal_mean_num[id] ++;
		id = face->id3;
		normal_mean[id] += n1;
		normal_mean_num[id] ++;

	}
	for(i=0;i<j;i++)
	{
		if(normal_mean_num[p[i]->id]>0)
			normal_mean[p[i]->id] /= normal_mean_num[p[i]->id];
		p[i]->normal = normal_mean[p[i]->id];
	}
	for(i=0;i<k;i++)
	{
		h_positionData[12*i + 0] = f[i]->t1->pos.x;
		h_positionData[12*i + 1] = f[i]->t1->pos.y;
		h_positionData[12*i + 2] = f[i]->t1->pos.z;
		h_positionData[12*i + 3] = 0;
		h_positionData[12*i + 4] = f[i]->t2->pos.x;
		h_positionData[12*i + 5] = f[i]->t2->pos.y;
		h_positionData[12*i + 6] = f[i]->t2->pos.z;
		h_positionData[12*i + 7] = 0;
		h_positionData[12*i + 8] = f[i]->t3->pos.x;
		h_positionData[12*i + 9] = f[i]->t3->pos.y;
		h_positionData[12*i + 10] = f[i]->t3->pos.z;
		h_positionData[12*i + 11] = 0;

		h_normalData[12*i + 0] = f[i]->t1->normal.x;
		h_normalData[12*i + 1] = f[i]->t1->normal.y;
		h_normalData[12*i + 2] = f[i]->t1->normal.z;
		h_normalData[12*i + 3] = 0;
		h_normalData[12*i + 4] = f[i]->t2->normal.x;
		h_normalData[12*i + 5] = f[i]->t2->normal.y;
		h_normalData[12*i + 6] = f[i]->t2->normal.z;
		h_normalData[12*i + 7] = 0;
		h_normalData[12*i + 8] = f[i]->t3->normal.x;
		h_normalData[12*i + 9] = f[i]->t3->normal.y;
		h_normalData[12*i + 10] = f[i]->t3->normal.z;
		h_normalData[12*i + 11] = 0;
		/*
		vector3f n1;
		vector3f p01 = f[i]->t2->pos - f[i]->t1->pos;
		vector3f p12 = f[i]->t3->pos - f[i]->t2->pos;
		n1 = p01 % p12;
		n1.normalize();
		normalData[12*i + 0] = n1.x;
		normalData[12*i + 1] = n1.y;
		normalData[12*i + 2] = n1.z;
		normalData[12*i + 3] = 0;
		normalData[12*i + 4] = n1.x;
		normalData[12*i + 5] = n1.y;
		normalData[12*i + 6] = n1.z;
		normalData[12*i + 7] = 0;
		normalData[12*i + 8] = n1.x;
		normalData[12*i + 9] = n1.y;
		normalData[12*i + 10] = n1.z;
		normalData[12*i + 11] = 0;
		*/
	}
}
void PBDobject::draw()
{
	int i, j, k;
	/*
	glPointSize(2.0);
	glBegin(GL_POINTS);
	for(i=0;i<p.size();i++)
	{
		double xx,yy,zz;
		xx = p[i]->pos.x;
		yy = p[i]->pos.y;
		zz = p[i]->pos.z;
		glVertex3d(xx,yy,zz);
	}
	glEnd();
	*/
	/*
	glUseProgram(shader->program);
	glBindVertexArray(shader->vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, 3*f.size());
	*/
	
	glPointSize(1.0);
	glPolygonMode(GL_FRONT_AND_BACK,show_type);
	glBegin(GL_TRIANGLES);
	for(i=0;i<f.size();i++)
	{
		if(!f[i]->alive) continue;
		vector3f n1;
		vector3f p01 = f[i]->t2->pos - f[i]->t1->pos;
		vector3f p12 = f[i]->t3->pos - f[i]->t2->pos;
		n1 = p01 % p12;
		n1.normalize();
		double xx,yy,zz;
		double xxx,yyy,zzz;
		xx = f[i]->t1->pos.x;
		yy = f[i]->t1->pos.y;
		zz = f[i]->t1->pos.z;
		xxx = f[i]->t1->normal.x;
		yyy = f[i]->t1->normal.y;
		zzz = f[i]->t1->normal.z;
		if(f[i]->t1->temp) glColor3f(0.1, 0.8, 0.8);
		else  glColor3f(0.8, 0.1, 0.1);
		glNormal3d(xxx,yyy,zzz);
		glVertex3d(xx,yy,zz);
		xx = f[i]->t2->pos.x;
		yy = f[i]->t2->pos.y;
		zz = f[i]->t2->pos.z;
		xxx = f[i]->t2->normal.x;
		yyy = f[i]->t2->normal.y;
		zzz = f[i]->t2->normal.z;
		if(f[i]->t2->temp) glColor3f(0.1, 0.8, 0.8);
		else  glColor3f(0.8, 0.1, 0.1);
		glNormal3d(xxx,yyy,zzz);
		glVertex3d(xx,yy,zz);
		xx = f[i]->t3->pos.x;
		yy = f[i]->t3->pos.y;
		zz = f[i]->t3->pos.z;
		xxx = f[i]->t3->normal.x;
		yyy = f[i]->t3->normal.y;
		zzz = f[i]->t3->normal.z;
		if(f[i]->t3->temp) glColor3f(0.1, 0.8, 0.8);
		else  glColor3f(0.8, 0.1, 0.1);
		glNormal3d(xxx,yyy,zzz);
		glVertex3d(xx,yy,zz);
	}
	glEnd();
}
void PBDobject::draw(Shader *shader)
{
	int i, j, k;
	// write buffer to fbo

	//

//	glUniform1f(glGetUniformLocation(shader->program, "pointScale"), 8.0 / tanf(45*0.5f*(float)3.141592/180.0f));
//	glUniform1f(glGetUniformLocation(shader->program, "pointRadius"), 0.25);

	glBindVertexArray(shader->vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, 3*f.size());
}