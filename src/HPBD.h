#ifndef __HIERARCHICAL_PBD_H__
#define __HIERARCHICAL_PBD_H__

#include "PBD.h"

class HPBD{
public:
	const static int k_e = 2;
	int level_max;
	vector<PBD *> p;

	void build();
	
	void inil();
	void simul();
	void draw();
};
#endif