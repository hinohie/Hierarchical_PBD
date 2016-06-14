#ifndef __PBD_SIMULATION_H__
#define __PBD_SIMULATION_H__

#include "vector3f.h"
#include "PBDobject.h"
#include "PBDconstraint.h"
#include "Shader.h"

#include<vector>
#include<algorithm>
using namespace std;

class PBD{
public:
	PBDobject *obj;
	vector<PBDconstraint *> con;
	vector<PBDcollision *> coll;

	Shader *shader;
	Shader *blur_shader;
	
//	struct cudaGraphicsResource *m_cuda_posvbo_resource;
//	struct cudaGraphicsResource *m_cuda_colorvbo_resource;

	double dt;
	int dep_max;

	int iter_max;
	double k_stretch;
	double k_bending;

	PBD(){
		iter_max = 10;
		dt = 0.02;
		k_stretch = 0.9;
		k_bending = 0.0;
		dep_max = 1;
	}
	~PBD()
	{
		if(obj) delete obj;
		int i, j;
		j=con.size();
		for(i=0;i<j;i++)
			delete con[i];
		con.clear();
	}

	void shader_init();

	void inil();
	void simul();
	void draw();

	void process();
	void prev_process();
	void post_process();

	void dfs(int dep, vector3f p, vector3f q, vector3f r);
	void wahaha();

	int m_numBlocks;
	int m_numThreads;
};


#endif /* __PBD_SIMULATION_H__ */