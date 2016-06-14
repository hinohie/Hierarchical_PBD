#ifndef __PBD_CONSTRAINT_H__
#define __PBD_CONSTRAINT_H__

#include "vector3f.h"
#include "PBDobject.h"
#include<vector>
#include<algorithm>
using namespace std;

class PBDconstraint{
public:
	int n;
	double dt;
	double stiff;
	bool alive;
	vector<PBDparticle *> p;
	vector<vector3f> del;
	
	PBDconstraint()
	{
		alive = true;
		int i;
		n=0;
		dt = 0.02;
		stiff = 1;
		p.clear();
		del.clear();
	}
	PBDconstraint(vector<PBDparticle *> &_p)
	{
		alive = true;
		int i;
		n=_p.size();
		dt = 0.02;
		stiff = 1;
		p.clear();
		del.clear();
		for(i=0;i<n;i++)
		{
			p.push_back(_p[i]);
			del.push_back(vector3f(0,0,0));
		}
	}
	virtual ~PBDconstraint()
	{
		int i;
		p.clear();
	}

	void simulate();
	virtual double constraint() = 0;
	virtual void del_constraint() = 0;
};

class PBDconstraint_distance : public PBDconstraint{
public:
	double init_dist;
	double constraint();
	void del_constraint();

	PBDconstraint_distance() : PBDconstraint()
	{
	}
	PBDconstraint_distance(vector<PBDparticle *> &p) : PBDconstraint(p)
	{
		if(p.size()>=2)
		{
			init_dist = p[0]->pos.dist(p[1]->pos);
		}
	}
	~PBDconstraint_distance(){}
};
class PBDconstraint_bending : public PBDconstraint{
public:
	double init_angle;
	double constraint();
	void del_constraint();

	PBDconstraint_bending() : PBDconstraint()
	{
	}
	PBDconstraint_bending(vector<PBDparticle *> &p) : PBDconstraint(p)
	{
		if(p.size()>=4)
		{
			vector3f p01 = p[1]->pos - p[0]->pos;
			vector3f p02 = p[2]->pos - p[0]->pos;
			vector3f p03 = p[3]->pos - p[0]->pos;
			vector3f p012 = p01 % p02;
			vector3f p013 = p01 % p03;
			p012.normalize();
			p013.normalize();
			init_angle = acos(p012 * p013);
		}
	}
	~PBDconstraint_bending(){}
};
class PBDconstraint_volume : public PBDconstraint{
public:
	double init_volume;
	double k_press;
	double constraint();
	void del_constraint();

	PBDconstraint_volume() : PBDconstraint()
	{
		k_press = 1;
	}
	PBDconstraint_volume(vector<PBDparticle *> &p) : PBDconstraint(p)
	{
		k_press = 1;
		if(p.size() >= 4)
		{
			int i;
			double res=0;
			for(i=0;i<n;i+=3)
			{
				res += (p[i+0]->pos%p[i+1]->pos)*p[i+2]->pos;
			}
			init_volume = res;
		}
	}
};


class PBDcollision : public PBDconstraint{
public:
	vector3f coll_pos;
	vector3f norm_vector;
	PBDcollision() : PBDconstraint()
	{
	}
	PBDcollision(vector<PBDparticle *> &_p, vector3f col, vector3f norm) : PBDconstraint(_p)
	{
		coll_pos = col;
		norm_vector = norm;
	}
	virtual ~PBDcollision()
	{
		int i;
		p.clear();
	}

	void simulate();
	double constraint();
	void del_constraint();
};
#endif