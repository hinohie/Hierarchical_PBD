#ifndef __PBD_OBJECT_H__
#define __PBD_OBJECT_H__

#include "vector3f.h"
#include "Shader.h"

class PBDparticle
{
public:
	static int cur_id;
	int id;
	vector3f pos;
	vector3f normal;
	vector3f pre_pos;
	vector3f vel;
	bool temp;
	double w;
	PBDparticle *father;
	PBDparticle *daughter;
	PBDparticle()
	{
		pos = vel = normal = vector3f();
		w = 1;
		id = cur_id++;
		temp = false;
		father = daughter = NULL;
	}
	PBDparticle(vector3f _pos)
	{
		pos = _pos;
		vel =  normal = vector3f();
		w = 1;
		id = cur_id++;
		temp = false;
		father = daughter = NULL;
	}
	PBDparticle(vector3f _pos, vector3f _vel)
	{
		pos = _pos;
		vel = _vel; 
		normal = vector3f();
		w = 1;
		id = cur_id++;
		temp = false;
		father = daughter = NULL;
	}
	PBDparticle(vector3f _pos, vector3f _vel, double _w)
	{
		pos = _pos;
		vel = _vel;
		normal = vector3f();
		w = _w;
		id = cur_id++;
		temp = false;
		father = daughter = NULL;
	}
	void set(PBDparticle *_p)
	{
		pos = _p->pos;
		pre_pos = _p->pre_pos;
		vel = _p->vel;
		normal = _p->normal;
		w = _p->w;
		father = _p;
		_p->daughter = this;
	}
};
class PBDface{
public:
	int id1,id2,id3;
	PBDparticle *t1,*t2,*t3;
	bool alive;
	PBDface(){alive =true;}
	PBDface(PBDparticle *_t1, PBDparticle *_t2, PBDparticle *_t3)
	{
		t1 = _t1;
		t2 = _t2;
		t3 = _t3;
		id1 = _t1->id;
		id2 = _t2->id;
		id3 = _t3->id;
		alive =true;
	}
	~PBDface();
};
class PBDobject{
public:
	vector<PBDparticle *> p;
	vector<PBDface *> f;
	float *positionData;
	float *normalData;
	float *h_positionData;
	float *h_normalData;
	vector3f *particle_positionData;
	PBDobject(){
		positionData = normalData = 0;
		h_positionData = h_normalData = 0;
	}
	~PBDobject()
	{
		f.clear();
		int i, j;
		j=p.size();
		for(i=0;i<j;i++)
			delete p[i];
		p.clear();
	}

	GLuint fbo;
	GLuint initDepthTextureId_front;
	GLuint initColorTextureId_front;
	GLuint initNormalTextureId_front;
	GLuint initLightTextureId_front;
	GLuint texSnowObj;

	void draw();
	void draw(Shader *shader);
	void bind();
	void bind_texture();
	void update();
};

extern unsigned int show_type;

#endif