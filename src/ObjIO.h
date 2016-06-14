#ifndef __OBJECT_IO_H__
#define __OBJECT_IO_H__

#include<algorithm>
#include<vector>
#include<map>
#include<hash_map>
#include<unordered_map>
#include <unordered_set>
#include<queue>
#include <set>
#include <iterator>
#include <cmath>
using namespace std;

#include "vector3f.h"
class objColor{
public:
	double R, G, B, A;
	objColor(){
		R = G = B = A = 1.0;
	}
	objColor(double r, double g, double b, double a = 1.0)
	{
		R = r; G = g; B = b; A = a;
	}
	void setf(double r, double g, double b, double a = 1.0)
	{
		R = r; G = g; B = b; A = a;
	}
	void setd(int r, int g, int b, int a = 255)
	{
		R = (r / 255.0);
		G = (g / 255.0);
		B = (b / 255.0);
		A = (a / 255.0);
	}
	void copy(objColor c)
	{
		R = c.R; G = c.G; B = c.B; A = c.A;
	}
};
class objMeterial{
public:
	char* name;
	objColor Ambient;
	objColor Diffuse;
	objColor Specular;
	double Ns;
	double d;
	double Tr;
	int illum;

	objMeterial(){
		name = NULL;
		reset();
	}
	objMeterial(const objMeterial *me)
	{
		if (me == NULL){ name = NULL; reset(); return; }

		this->name = NULL;
		if (me->name != NULL)
		{
			this->name = new char[strlen(me->name) + 1];
			this->name[strlen(me->name)] = 0;
			strcpy(this->name, me->name);
		}

		this->Ns = me->Ns;
		this->d = me->d;
		this->Tr = me->Tr;
		this->illum = me->illum;

		this->Ambient = me->Ambient;
		this->Diffuse = me->Diffuse;
		this->Specular = me->Specular;

	}
	~objMeterial(){
		reset();
	}
	void reset(){
		if (name != NULL) delete name;
		name = NULL;
		illum = 0;
		Ambient = objColor();
		Diffuse = objColor();
		Specular = objColor();
		Ns = 0;
		d = 0;
		Tr = 0;
	}
};
/**************** illum table ******************
0. Color on and Ambient off
1. Color on and Ambient on
2. Highlight on
3. Reflection on and Ray trace on
4. Transparency: Glass on, Reflection: Ray trace on
5. Reflection: Fresnel on and Ray trace on
6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
8. Reflection on and Ray trace off
9. Transparency: Glass on, Reflection: Ray trace off
10. Casts shadows onto invisible surfaces
*************************************************/
class objFace{
public:
	vector<int> vv;
	vector<int> vn;
	vector<int> vt;
	objFace(){ reset(); }
	objFace(const objFace *me){
		if (me == NULL) { reset(); return; }
		this->vv = vector<int>(me->vv.begin(), me->vv.end());
		this->vn = vector<int>(me->vn.begin(), me->vn.end());
		this->vt = vector<int>(me->vt.begin(), me->vt.end());
	}
	~objFace(){ reset(); }
	void reset()
	{
		vv.clear();
		vn.clear();
		vt.clear();
	}
};
class objObject{
public:
	int n;
	char *name;
	objMeterial *mtl;
	vector<objFace *> f;
	vector<int> vv;
	bool smooth;

	objObject(){
		name = NULL;
		reset();
	}
	objObject(const objObject *me)
	{
		if (me == NULL){ name = NULL; reset(); return; }
		this->name = NULL;
		if (me->name != NULL)
		{
			this->name = new char[strlen(me->name) + 1];
			this->name[strlen(me->name)] = 0;
			strcpy(this->name, me->name);
		}

		this->vv = vector<int>(me->vv.begin(), me->vv.end());
		this->smooth = me->smooth;

		for (int i = 0; i<me->f.size(); i++)
			this->f.push_back(new objFace(me->f[i]));

		mtl = new objMeterial(me->mtl);
	}
	~objObject(){ reset(); }
	void reset(){
		if (name != NULL) delete name;
		name = NULL;
		mtl = NULL;
		for (int i = 0; i<f.size(); i++)
			delete f[i];
		f.clear();
		n = 0;
		smooth = false;
	}
};
class objModel{
public:
	vector<objObject *> obj;
	vector<objMeterial *> mtl;

	vector<vector3f> vv;
	vector<vector2f> vt;
	vector<vector3f> vn;
	vector<vector3f> vp;

	vector<matrix33f> vv_quadric;
	vector<vector3f> vv_vectors;
	vector<double> vv_d2;

	objModel(){ reset(); }
	objModel(const objModel *me){
		if (me == NULL){ reset(); return; }
		this->vv = vector<vector3f>(me->vv.begin(), me->vv.end());
		this->vt = vector<vector2f>(me->vt.begin(), me->vt.end());
		this->vn = vector<vector3f>(me->vn.begin(), me->vn.end());
		this->vp = vector<vector3f>(me->vp.begin(), me->vp.end());

		for (int i = 0; i<me->obj.size(); i++)
			this->obj.push_back(new objObject(me->obj[i]));
		for (int i = 0; i<me->mtl.size(); i++)
			this->mtl.push_back(new objMeterial(me->mtl[i]));
	}
	~objModel(){ reset(); }

	void SimpleSquareModel(int n, int m, double sx, double sy, double sz, double length_n, double length_m,  vector3f axis_n, vector3f axis_m);

	void getModel(const char* src);
	void writeModel(const char *filename);
	void getModel_3ds(const char* filename);
	void parsing_mtl(const char* src);

	void rebuild(double error = E);
	void normalize();
	void resize();
	void flip_normal(bool flip_all = false);
	void decimate(double rate, double alpha = 0.0);
	void reordering();

	void ready_qem();

	void reset(){
		for (int i = 0; i<obj.size(); i++)
			delete obj[i];
		obj.clear();
		for (int i = 0; i<mtl.size(); i++)
			delete mtl[i];
		mtl.clear();

		vv.clear();
		vt.clear();
		vn.clear();
		vp.clear();
	}
};

#endif