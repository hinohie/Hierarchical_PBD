#include<stdlib.h>
#include<stdio.h>
#include<algorithm>
#include<vector>

#include<GL/glew.h>
#include<glut.h>

using namespace std;

#include "HPBD.h"

HPBD *hpbd;

bool run_simul = true;

static int width, height;

GLenum show_type = GL_FILL;
bool show_axis = true;
bool double_pushed = false;
bool z_rotate = false;

float x_angle;
float y_angle;
float z_angle;
float x_move;
float y_move;
float z_move;
float scale = 1;

float initial_matrix[16] = {0.984808, 0.000000, -0.173648, 0.000000, 0.133022, 0.642788, 0.754407, 0.000000, 
	0.111619, -0.766044, 0.633022, 0.000000, -0.654631, -0.131208, -0.833212, 1.000000};
float matrix_stack[16] = {0.984808, 0.000000, -0.173648, 0.000000, 0.133022, 0.642788, 0.754407, 0.000000, 
	0.111619, -0.766044, 0.633022, 0.000000, -0.654631, -0.131208, -0.833212, 1.000000};

void reshape(int w, int h)
{
	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-0.70f, 0.70f, -0.70f, 0.70f);
	*/
	width = w; height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.01, 0.01, -0.01, 0.01, 0.05, 180.0);
}

/*
static int frame=0;
static int doo = 0;
void capture()
{
	if(doo == 0) doo = 1;
	else doo = 0;
	if(doo) return;
	if(frame > 500) exit(0);
	// Make the BYTE array, factor of 3 because it's RBG.
	BYTE* pixels = new BYTE[ 3 * width * height];

	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	// Convert to FreeImage format & save to file
	FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
	char filename[200];
	sprintf(filename,"F:/Result_Seen/HPBD/Image/%05d.png",frame);
	frame++;
	FreeImage_Save(FIF_PNG, image, filename, 0);

	// Free resources
	FreeImage_Unload(image);
	delete [] pixels;
}
*/
static bool show_depth = true;
static bool show_normal = true;
void grid_draw()
{
	int i, j, k;
	glPushMatrix();
	glTranslatef(0,-0.1,0);
	glColor3d(0.5,0.5,0.5);
	glBegin(GL_LINES);
	for(i=-20;i<=20;i++)
	{
		glVertex3d(-2.1,0,i/10.0);
		glVertex3d(2.1,0,i/10.0);
	}
	glEnd();
	glBegin(GL_LINES);
	for(i=-20;i<=20;i++)
	{
		glVertex3d(i/10.0,0,-2.1);
		glVertex3d(i/10.0,0,2.1);
	}
	glEnd();
	glTranslatef(0,0.1,0);
	glPopMatrix();
}
void display()
{
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, hpbd->p[0]->obj->fbo);
	glViewport(0, 0, 800, 800);
	glPolygonMode(GL_BACK, GL_LINE);
	glCullFace(GL_FRONT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.01, 0.01, -0.01, 0.01, 0.05, 180.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 1,
			 0, 0, 0,
			0, 1 ,0);

	
	glPushMatrix();
	glScalef(scale,scale,scale);
	glTranslatef(x_move, y_move,z_move);
	glRotatef(z_angle,0,0,1);
	glRotatef(x_angle,0,1,0);
	glRotatef(y_angle,1,0,0);
	glMultMatrixf(matrix_stack);
	
	glPolygonMode(GL_FRONT_AND_BACK,show_type);
	glEnable(GL_POINT_SPRITE_ARB);
	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	//Draw
	glUseProgram(hpbd->p[0]->shader->program);
//	if(show_axis)grid_draw();
//	glutSolidSphere(0.2,30,30);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3d(1,1,1);
	hpbd->draw();
	glPushMatrix();
	glColor3d(0.5, 0.5,1.0);
	glTranslated(0.2,0.7,0.5);
	glutSolidSphere(0.19,30,30);
	glPopMatrix();/*
	glPushMatrix();
	glColor3d(0.0, 1.0, 0.0);
	glTranslated(0.8,0.65,0.8);
	glutSolidSphere(0.098,30,30);
	glPopMatrix();*/
	glPopMatrix();
	
	glUseProgram(0);

	
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	// real view here
	glUseProgram(hpbd->p[0]->blur_shader->program);

	glPushMatrix();
	glViewport(0, 0, 800, 800);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, 0, 2);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 1,
			 0, 0, 0,
			0, 1 ,0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,  hpbd->p[0]->obj->initDepthTextureId_front);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,  hpbd->p[0]->obj->initColorTextureId_front);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,  hpbd->p[0]->obj->initNormalTextureId_front);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D,  hpbd->p[0]->obj->initLightTextureId_front);
	glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"depthMap"),0);
	glUniform1i(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"colorMap"),1);
	glUniform1i(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"normalMap"),2);
	glUniform1i(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"lightMap"),3);
	glUniform1f(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"depthMapWidth"),800);
	glUniform1f(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"depthMapHeight"),800);
	glUniform1f(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"near"),0.90);
	glUniform1f(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"far"),1.0);
	glUniform1f(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"show_depth"),show_depth);
	glUniform1f(glGetUniformLocation(hpbd->p[0]->blur_shader->program,"show_normal"),show_normal);
	glColor4d(1,1,1,1);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0); glVertex3d(-1, -1, 0);
	glTexCoord2d(1, 0); glVertex3d(1, -1, 0);
	glTexCoord2d(1, 1); glVertex3d(1, 1.0, 0);
	glTexCoord2d(0, 1); glVertex3d(-1, 1.0, 0);
	glEnd();
	glUseProgram(0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
//	capture();
	

	glutSwapBuffers();
}
void update()
{
	if(hpbd && run_simul)
		hpbd->simul();
	glutPostRedisplay();
}

void MultMatrix4fv(float *p,float *q)
{
	int i, j, k;
	float r[16];
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
		{
			r[i*4+j] = 0;
			for(k=0;k<4; k++)
			{
				r[i*4+j] += p[i*4+k] * q[k*4+j];
			}
		}
	for(i=0;i<16;i++)
		p[i]=r[i];
}


void data_inil();
static unsigned char pre_pushed_key = 'w';
static int ccnt = 0;
void Ckeyboard(unsigned char key, int x, int y)
{
	int cnt=10;
	int pnt = 1;
	int i, j, k;
	switch(key){
	case 'P':
		key += ('a'-'A');
	case 'p':
		show_type = GL_POINT;
		break;
	case 'W':
		key += ('a'-'A');
	case 'w':
		show_type = GL_LINE;
		break;
	case 'S':
		key += ('a'-'A');
	case 's':
		show_type = GL_FILL;
		break;
	case 27:
		x_angle = y_angle = 0;
		x_move = y_move = 0;
		for(int i=0; i<16; i++)
			matrix_stack[i] = initial_matrix[i];
		break;
	case 'X':
		key += ('a'-'A');
	case 'x':
		show_axis ^= true;
		break;
	case 'Z':
		key += ('a'-'A');
	case 'z':
		z_rotate ^= true;
		break;
	case ' ':
		run_simul ^= true;
		break;
	case 'r':
	case 'R':
		delete hpbd;
		data_inil();
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	case 'g':
	case 'G':
		show_depth ^= true;
		break;
	case 'h':
	case 'H':
		show_normal ^= true;
		break;
	}
	if(pre_pushed_key == key)
	{
		if(key == 'w' || key == 's' || key == 'p')
		double_pushed ^= true;
	}
	else
		double_pushed = false;
	pre_pushed_key = key;
}
void Skeyboard(int key, int x,int y)
{
}
void Pmouse(int x,int y)
{

}
static int sx, sy;
static int down_mode=-1;
void Cmouse(int button, int state, int x,int y)
{
	if(state == GLUT_DOWN)
	{
		if(down_mode<0 && button == GLUT_LEFT_BUTTON)
			down_mode = GLUT_LEFT_BUTTON;
		if(down_mode<0 && button == GLUT_RIGHT_BUTTON)
			down_mode = GLUT_RIGHT_BUTTON;
		if(down_mode<0 && button == GLUT_MIDDLE_BUTTON)
			down_mode = GLUT_MIDDLE_BUTTON;
	}
	else if(down_mode >= 0){
		float temp_stack[16];
		glPushMatrix();
		glLoadIdentity();
		glScalef(scale,scale,scale);
		glTranslatef(x_move, y_move, z_move);
		glRotatef(z_angle,0,0,1);
		glRotatef(x_angle,0,1,0);
		glRotatef(y_angle,1,0,0);
		glGetFloatv(GL_MODELVIEW_MATRIX, temp_stack);
		glPopMatrix();
		MultMatrix4fv(matrix_stack, temp_stack);

		down_mode = -1;
		x_angle = y_angle = z_angle = 0;
		x_move = y_move = z_move = 0;
		scale = 1.0;
	}
	sx = sy = -1;
}
void Mmouse(int x,int y)
{
	int tx, ty;
	if(sx<0 || sy<0){tx=0, ty=0;}
	else{tx = x-sx; ty = y-sy;}
	if(down_mode == GLUT_RIGHT_BUTTON)
	{
		x_angle += tx / 10.0;
		if(z_rotate) z_angle += ty/10.0;
		else y_angle += ty / 10.0;
	}
	else if(down_mode == GLUT_LEFT_BUTTON)
	{
		x_move += tx / 1000.0;
		if(z_rotate) z_move -= ty / 1000.0;
		else y_move -= ty / 1000.0;
	}
	else if(down_mode == GLUT_MIDDLE_BUTTON)
	{
		scale *= exp((tx - ty)/100.0);
	}
	sx = x;
	sy = y;
}

void data_inil()
{
	glPushMatrix();
	glLoadIdentity();
	glScalef(0.15,0.15,0.15);
	glTranslated(0, -0.7,0);
	glRotatef(0,0,0,1);
	glRotatef(20,1,0,0);
	glRotatef(-70,0,1,0);
	glGetFloatv(GL_MODELVIEW_MATRIX, initial_matrix);
	glPopMatrix();
	for(int i=0; i<16; i++)
		matrix_stack[i] = initial_matrix[i];
	PBDparticle::cur_id = 0;
	hpbd = new HPBD();
	hpbd->inil();
}
void gl_inil()
{
	int i, j;
	glEnable(GL_DEPTH_TEST);


	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	
	GLfloat LightPosition[4] = {0.8, 1.2, 0.2, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	GLfloat LightAmbient[4]  = {0.2, 0.2, 0.2, 0.2};
	GLfloat LightDiffuse[4]  = {1.0, 1.0, 1.0, 1.0};
	GLfloat LightSpecular[4] = {1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);


}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);

	glutCreateWindow("PBDtest");
	GLenum err = glewInit();

	if(GLEW_OK != err)
	{
		cout << "hull\n" << endl;
		cout << glewGetErrorString(err) <<endl;
	}

	data_inil();
	gl_inil();
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(update);
	
	glutKeyboardFunc( Ckeyboard );
	glutSpecialFunc( Skeyboard );
	glutPassiveMotionFunc( Pmouse );
	glutMouseFunc( Cmouse );
	glutMotionFunc( Mmouse );

	glutMainLoop();

	return 0;
}