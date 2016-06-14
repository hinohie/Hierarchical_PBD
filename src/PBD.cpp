#include "PBD.h"
#include <omp.h>
#include "ObjIO.h"
#include <hash_set>
static int h,w;
/* Note : this code is for CUDA. and HPBD is not works on CUDA. just watching
static void * mapGLBufferObject(struct cudaGraphicsResource **cuda_vbo_resource)
{
	void *ptr;

	cudaGraphicsMapResources(1, cuda_vbo_resource, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&ptr, &num_bytes, *cuda_vbo_resource);

	return ptr;
}


static void unmapGLBufferObject(struct cudaGraphicsResource *cuda_vbo_resource)
{
	cudaGraphicsUnmapResources(1, &cuda_vbo_resource, 0);
}
*/
void PBD::shader_init()
{
	shader->InitShader("particle_vshader.glsl", "particle_fshader.glsl");

	
	// create and set-up the vertex array object
	glGenVertexArrays(1, &shader->vaoHandle);
	glBindVertexArray(shader->vaoHandle);

	// Enable the vertex attribute arrays
	glEnableVertexAttribArray(0);	// vertex position
	glEnableVertexAttribArray(1);	// vertex normal
	glEnableVertexAttribArray(2);	// vertex color

	// create and populate the buffer objects
	GLuint vboHandles[4];
	glGenBuffers(4, vboHandles);
	shader->positionBufferHandle = vboHandles[0];
	shader->normalBufferHandle	= vboHandles[1];
	shader->colorBufferHandle  = vboHandles[2];

	// populate the position buffer
	/* Note : this code is for CUDA. and HPBD is not works on CUDA. just watching
	glBindBuffer(GL_ARRAY_BUFFER, shader->positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, obj->f.size()*12*sizeof(float), obj->h_positionData, GL_DYNAMIC_DRAW);
	cudaGraphicsGLRegisterBuffer(&m_cuda_posvbo_resource, vboHandles[0], 0);

	// populate the normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, shader->normalBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, obj->f.size()*12*sizeof(float), obj->h_normalData, GL_DYNAMIC_DRAW);
	cudaGraphicsGLRegisterBuffer(&m_cuda_colorvbo_resource, vboHandles[1], 0);
	*/
	// populate the color buffer
	float *tmp_color = new float[12*obj->f.size()];
	for(int i=0;i<obj->f.size();i++)
	{
		if(obj->f[i]->t1->temp){tmp_color[12*i + 0] = 0.1; tmp_color[12*i + 1] = 0.8; tmp_color[12*i + 2] = 0.8;}
		else {tmp_color[12*i + 0] = 0.8; tmp_color[12*i + 1] = 0.1; tmp_color[12*i + 2] = 0.1;}
		if(obj->f[i]->t2->temp){tmp_color[12*i + 4] = 0.1; tmp_color[12*i + 5] = 0.8; tmp_color[12*i + 6] = 0.8;}
		else {tmp_color[12*i + 4] = 0.8; tmp_color[12*i + 5] = 0.1; tmp_color[12*i + 6] = 0.1;}
		if(obj->f[i]->t3->temp){tmp_color[12*i + 8] = 0.1; tmp_color[12*i + 9] = 0.8; tmp_color[12*i + 10] = 0.8;}
		else {tmp_color[12*i + 8] = 0.8; tmp_color[12*i + 9] = 0.1; tmp_color[12*i + 10] = 0.1;}
		tmp_color[12*i + 3] = tmp_color[12*i + 7] = tmp_color[12*i + 11] = 1.0;
	}
	glBindBuffer(GL_ARRAY_BUFFER, shader->colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, obj->f.size()*12*sizeof(float), tmp_color, GL_DYNAMIC_DRAW);
	delete[] tmp_color;
//	cudaGraphicsGLRegisterBuffer(&m_cuda_colorvbo_resource, vboHandles[2], 0);

	// link step
	glBindVertexBuffer(0, shader->positionBufferHandle, 0, sizeof(GLfloat)*4);
	glBindVertexBuffer(1, shader->normalBufferHandle, 0, sizeof(GLfloat)*4);
	glBindVertexBuffer(2, shader->colorBufferHandle, 0, sizeof(GLfloat)*4);

	glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);
	glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(2, 2);

	// blur_shader
	blur_shader->InitShader("cartoon_vshader.glsl","cartoon_fshader.glsl");

	
	// create and set-up the vertex array object
	glGenVertexArrays(1, &blur_shader->vaoHandle);
	glBindVertexArray(blur_shader->vaoHandle);

	// Enable the vertex attribute arrays
	glEnableVertexAttribArray(0);	// vertex position
	glEnableVertexAttribArray(1);	// vertex normal
	glEnableVertexAttribArray(2);	// vertex color

	// create and populate the buffer objects
	GLuint blur_vboHandles[4];
	glGenBuffers(4, blur_vboHandles);
	blur_shader->positionBufferHandle = blur_vboHandles[0];
	blur_shader->normalBufferHandle	= blur_vboHandles[1];
	blur_shader->colorBufferHandle  = blur_vboHandles[2];

	// populate the position buffer
	/* Note : this code is for CUDA. and HPBD is not works on CUDA. just watching
	glBindBuffer(GL_ARRAY_BUFFER, blur_shader->positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, obj->f.size()*12*sizeof(float), obj->positionData, GL_DYNAMIC_DRAW);
	cudaGraphicsGLRegisterBuffer(&m_cuda_posvbo_resource, blur_vboHandles[0], 0);

	// populate the normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, blur_shader->normalBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, obj->f.size()*12*sizeof(float), obj->normalData, GL_DYNAMIC_DRAW);
	cudaGraphicsGLRegisterBuffer(&m_cuda_colorvbo_resource, blur_vboHandles[1], 0);
	*/
	// populate the color buffer
	tmp_color = new float[12*obj->f.size()];
	for(int i=0;i<obj->f.size();i++)
	{
		if(obj->f[i]->t1->temp){tmp_color[12*i + 0] = 0.1; tmp_color[12*i + 1] = 0.8; tmp_color[12*i + 2] = 0.8;}
		else {tmp_color[12*i + 0] = 0.8; tmp_color[12*i + 1] = 0.1; tmp_color[12*i + 2] = 0.1;}
		if(obj->f[i]->t2->temp){tmp_color[12*i + 4] = 0.1; tmp_color[12*i + 5] = 0.8; tmp_color[12*i + 6] = 0.8;}
		else {tmp_color[12*i + 4] = 0.8; tmp_color[12*i + 5] = 0.1; tmp_color[12*i + 6] = 0.1;}
		if(obj->f[i]->t3->temp){tmp_color[12*i + 8] = 0.1; tmp_color[12*i + 9] = 0.8; tmp_color[12*i + 10] = 0.8;}
		else {tmp_color[12*i + 8] = 0.8; tmp_color[12*i + 9] = 0.1; tmp_color[12*i + 10] = 0.1;}
		tmp_color[12*i + 3] = tmp_color[12*i + 7] = tmp_color[12*i + 11] = 1.0;
	}
	glBindBuffer(GL_ARRAY_BUFFER, blur_shader->colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, obj->f.size()*12*sizeof(float), tmp_color, GL_DYNAMIC_DRAW);
	delete[] tmp_color;

	// link step
	glBindVertexBuffer(0, blur_shader->positionBufferHandle, 0, sizeof(GLfloat)*4);
	glBindVertexBuffer(1, blur_shader->normalBufferHandle, 0, sizeof(GLfloat)*4);
	glBindVertexBuffer(2, blur_shader->colorBufferHandle, 0, sizeof(GLfloat)*4);

	glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);
	glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(2, 2);
}

void PBD::inil()
{
	m_numBlocks = 1024;
	m_numThreads = 512;
	if(false){
	obj = NULL;
	objModel *model = new objModel();
//	model->getModel("resource/fsaverage.lh.pial.obj");
//	model->SimpleSquareModel(25, 25, -1, 1, -1, 2, 2, vector3f(1,0,0), vector3f(0,0,1));
	model->getModel("resource/dragon.obj");
//	model->getModel("resource/Tiger.obj");
	model->rebuild(E);
	printf("%d %d\n",model->vv.size(), model->obj.size());
	
	hash_set<long long int> edge_set;
	hash_map<long long int, PBDparticle *> edge_map;
	PBDparticle ** particle_id_map = new PBDparticle *[model->vv.size()];

	// model to PBDobject and constraint

	k_bending = 1 - pow((1-k_bending),1.0/iter_max);
	k_stretch = 1 - pow((1-k_stretch),1.0/iter_max);

	obj = new PBDobject();
	int i, j;
	for(i=0;i<model->vv.size();i++)
	{
//		PBDparticle *particle = new PBDparticle(model->vv[i]/1500 + vector3f(1.1, 1, -0.4));	//Tiger
//		PBDparticle *particle = new PBDparticle(model->vv[i] + vector3f(0, 0.5, 0));			// Simple
//		PBDparticle *particle = new PBDparticle(model->vv[i]/80 + vector3f(0.25, 2.55, 0.2));		//Brain
		PBDparticle *particle = new PBDparticle(model->vv[i]/80 + vector3f(0.23, 0.75, -0.5));		//Dragon

		obj->p.push_back(particle);
		particle_id_map[i] = particle;
	}
	vector<PBDparticle *> volp;
	for(int ii=0;ii<model->obj.size();ii++)
	{
		objObject *oobj = model->obj[ii];
		for(i=0;i<oobj->f.size();i++)
		{
			objFace *face = oobj->f[i];
			PBDparticle *zero = particle_id_map[face->vv[0]];
			PBDconstraint_distance *cond;
			vector<PBDparticle *> pp;
			long long int ppii,qqii,rrii;
			pp.push_back(NULL);
			pp.push_back(NULL);
			for(j=1;j+1<face->vv.size();j++)
			{
				PBDparticle *one = particle_id_map[face->vv[j]];
				PBDparticle *two = particle_id_map[face->vv[j+1]];
				PBDface *ff = new PBDface(zero,one,two);
				obj->f.push_back(ff);
				volp.push_back(zero);
				volp.push_back(one);
				volp.push_back(two);
				pp[0] = one;
				pp[1] = two;
				ppii = pp[0]->id;
				ppii = ppii * 2147483647 + pp[1]->id;
				qqii = pp[1]->id;
				qqii = qqii * 2147483647 + pp[0]->id;
				if(ppii == qqii || edge_set.find(ppii) != edge_set.end() || edge_set.find(qqii) != edge_set.end()) continue;
				edge_set.insert(ppii);
				cond = new PBDconstraint_distance(pp);
				cond->stiff = k_stretch;
				con.push_back(cond);

				ppii = face->vv[0];
				ppii = ppii * 2147483647 + face->vv[j];
				qqii = face->vv[j];
				qqii = qqii * 2147483647 + face->vv[j+1];
				rrii = face->vv[j+1];
				rrii = rrii * 2147483647 + face->vv[0];

				edge_map[ppii] = two;
				edge_map[qqii] = zero;
				edge_map[rrii] = one;
			}
			pp[0] = zero;
			pp[1] = particle_id_map[face->vv[1]];
				ppii = pp[0]->id;
				ppii = ppii * 2147483647 + pp[1]->id;
				qqii = pp[1]->id;
				qqii = qqii * 2147483647 + pp[0]->id;
			if(ppii == qqii || edge_set.find(ppii) != edge_set.end() || edge_set.find(qqii) != edge_set.end()){}
			else
			{
				edge_set.insert(ppii);
			cond = new PBDconstraint_distance(pp);
			cond->stiff = k_stretch;
			con.push_back(cond);
			}
			pp[0] = zero;
			pp[1] = particle_id_map[face->vv[j]];
				ppii = pp[0]->id;
				ppii = ppii * 2147483647 + pp[1]->id;
				qqii = pp[1]->id;
				qqii = qqii * 2147483647 + pp[0]->id;
			if(ppii == qqii || edge_set.find(ppii) != edge_set.end() || edge_set.find(qqii) != edge_set.end()){}
			else
			{
				edge_set.insert(ppii);
			cond = new PBDconstraint_distance(pp);
			cond->stiff = k_stretch;
			con.push_back(cond);
			}
		}
	}
	printf("dist_constraint done\n");
	for(hash_map<long long int, PBDparticle *>::iterator it = edge_map.begin(); it != edge_map.end(); it++)
	{
		long long int ppii,qqii;
		long long int pid1,pid2;
		ppii = it->first;
		qqii = ppii/2147483647 + (ppii%2147483647)*2147483647;
		pid1 = ppii/2147483647;
		pid2 = ppii%2147483647;
		PBDparticle *p1 = particle_id_map[pid1];
		PBDparticle *p2 = particle_id_map[pid2];
		PBDparticle *p3 = edge_map[ppii];
		PBDparticle *p4 = edge_map[qqii];
		if(p4 == NULL || p3 == NULL) continue;
		vector<PBDparticle *> vecp;
		vecp.push_back(p1);
		vecp.push_back(p2);
		vecp.push_back(p3);
		vecp.push_back(p4);
		PBDconstraint_bending *conb = new PBDconstraint_bending(vecp);
		conb->stiff = k_bending;
		con.push_back(conb);
	}
	printf("bending_constraint done\n");
	PBDconstraint_volume *conv = new PBDconstraint_volume(volp);
	printf("%lf\n",conv->init_volume);
	conv->k_press = 1.2;
	con.push_back(conv);
	printf("vertex : %d\nface : %d\nconstraint : %d\n",obj->p.size(), obj->f.size(), con.size());
	delete model;
	delete[] particle_id_map;
	}
	else
	{
	obj = new PBDobject();
	int i, j, k;
	h = w = 100;

	double sx,sy;
	double dx,dy;
	sx = 0;
	sy = 0;
	dy = 1.0 / h;
	dx = 1.0 / w;

	for(i=0;i<=w;i++)
	for(j=0;j<=h;j++)
	{
		PBDparticle *particle = new PBDparticle(vector3f(sy+dy*j, 1.0,sx + dx*i));
		if(abs(i)<=3 && (abs(j-h/5)<=3 || abs(j-h+h/5)<=3)) particle->w = 0;
//		else if(abs(i-w/2)<=1 && abs(j-3*h/8) <= 1) particle->w = 0;
//		else if(abs(i-0)<=1 && abs(j-7*h/8) <= 3) particle->w = 0;
//		else if(vector2f(j-h/2,i-w/2).dist()<=0.4) particle->w = 0;
//		else particle->w = 1;
		obj->p.push_back(particle);
		
	}
	k_bending = 1 - pow((1-k_bending),1.0/iter_max);
	k_stretch = 1 - pow((1-k_stretch),1.0/iter_max);

	for(i=0;i<w;i++)
	for(j=0;j<h;j++)
	{
		PBDface *face1 = new PBDface();
		face1->id1 = (i+0)*(h+1) + (j+0);
		face1->id2 = (i+1)*(h+1) + (j+0);
		face1->id3 = (i+0)*(h+1) + (j+1);
		face1->t1 = obj->p[(i+0)*(h+1) + (j+0)];
		face1->t2 = obj->p[(i+1)*(h+1) + (j+0)];
		face1->t3 = obj->p[(i+0)*(h+1) + (j+1)];
		obj->f.push_back(face1);

		PBDface *face2 = new PBDface();
		face2->id1 = (i+0)*(h+1) + (j+1);
		face2->id2 = (i+1)*(h+1) + (j+0);
		face2->id3 = (i+1)*(h+1) + (j+1);
		face2->t1 = obj->p[(i+0)*(h+1) + (j+1)];
		face2->t2 = obj->p[(i+1)*(h+1) + (j+0)];
		face2->t3 = obj->p[(i+1)*(h+1) + (j+1)];
		obj->f.push_back(face2);

		// constraint - stretch		
		vector<PBDparticle *> cond_v1;
		cond_v1.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_v1.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		vector<PBDparticle *> cond_v2;
		cond_v2.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_v2.push_back(obj->p[(i+0)*(h+1) + (j+1)]);
		vector<PBDparticle *> cond_v3;
		cond_v3.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_v3.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		vector<PBDparticle *> cond_v4;
		cond_v4.push_back(obj->p[(i+0)*(h+1) + (j+1)]);
		cond_v4.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		PBDconstraint_distance *cond1 = new PBDconstraint_distance(cond_v1);
		PBDconstraint_distance *cond2 = new PBDconstraint_distance(cond_v2);
	//	PBDconstraint_distance *cond3 = new PBDconstraint_distance(cond_v3);
		PBDconstraint_distance *cond4 = new PBDconstraint_distance(cond_v4);

		cond1->stiff = k_stretch;
		cond2->stiff = k_stretch;
	//	cond3->stiff = k_stretch;
		cond4->stiff = k_stretch;
		con.push_back(cond1);
		con.push_back(cond2);
	//	con.push_back(cond3);
		con.push_back(cond4);

		if(j == h-1)
		{
		vector<PBDparticle *> cond_sp1;
		cond_sp1.push_back(obj->p[(i+0)*(h+1) + (j+1)]);
		cond_sp1.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		PBDconstraint_distance *condp1 = new PBDconstraint_distance(cond_sp1);
		condp1->stiff = k_stretch;
		con.push_back(condp1);
		}
		if(i==w-1)
		{
		vector<PBDparticle *> cond_sp1;
		cond_sp1.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_sp1.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		PBDconstraint_distance *condp1 = new PBDconstraint_distance(cond_sp1);
		condp1->stiff = k_stretch;
		con.push_back(condp1);
		}
		
		// constraint - bending
		// side bending
		vector<PBDparticle *> cond_w1;
		cond_w1.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_w1.push_back(obj->p[(i+0)*(h+1) + (j+1)]);
		cond_w1.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_w1.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		PBDconstraint_bending *cond5 = new PBDconstraint_bending(cond_w1);
		vector<PBDparticle *> cond_w2;
		cond_w2.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_w2.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		cond_w2.push_back(obj->p[(i+0)*(h+1) + (j+1)]);
		cond_w2.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
//		PBDconstraint_bending *cond6 = new PBDconstraint_bending(cond_w2);

		cond5->stiff = k_bending;
//		cond6->stiff = k_bending;
		con.push_back(cond5);
//		con.push_back(cond6);
		/*
		if(i+1<w)
		{
		vector<PBDparticle *> cond_sp1;
		cond_sp1.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_sp1.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		cond_sp1.push_back(obj->p[(i+0)*(h+1) + (j+1)]);
		cond_sp1.push_back(obj->p[(i+2)*(h+1) + (j+0)]);
		vector<PBDparticle *> cond_sp2;
		cond_sp2.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		cond_sp2.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_sp2.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_sp2.push_back(obj->p[(i+2)*(h+1) + (j+1)]);
		vector<PBDparticle *> cond_sp3;
		cond_sp3.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_sp3.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		cond_sp3.push_back(obj->p[(i+0)*(h+1) + (j+1)]);
		cond_sp3.push_back(obj->p[(i+2)*(h+1) + (j+1)]);
		vector<PBDparticle *> cond_sp4;
		cond_sp4.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		cond_sp4.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_sp4.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_sp4.push_back(obj->p[(i+2)*(h+1) + (j+0)]);
		PBDconstraint_bending *condq1 = new PBDconstraint_bending(cond_sp1);
		PBDconstraint_bending *condq2 = new PBDconstraint_bending(cond_sp2);
		PBDconstraint_bending *condq3 = new PBDconstraint_bending(cond_sp3);
		PBDconstraint_bending *condq4 = new PBDconstraint_bending(cond_sp4);
		condq1->stiff = k_bending;
		condq2->stiff = k_bending;
		condq3->stiff = k_bending;
		condq4->stiff = k_bending;
		con.push_back(condq1);
		con.push_back(condq2);
		con.push_back(condq3);
		con.push_back(condq4);

		}
		if(j+1<h)
		{
		vector<PBDparticle *> cond_sp1;
		cond_sp1.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_sp1.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_sp1.push_back(obj->p[(i+0)*(h+1) + (j+1)]);
		cond_sp1.push_back(obj->p[(i+1)*(h+1) + (j+2)]);
		vector<PBDparticle *> cond_sp2;
		cond_sp2.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_sp2.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_sp2.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		cond_sp2.push_back(obj->p[(i+0)*(h+1) + (j+2)]);
		vector<PBDparticle *> cond_sp3;
		cond_sp3.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_sp3.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_sp3.push_back(obj->p[(i+0)*(h+1) + (j+1)]);
		cond_sp3.push_back(obj->p[(i+0)*(h+1) + (j+2)]);
		vector<PBDparticle *> cond_sp4;
		cond_sp4.push_back(obj->p[(i+1)*(h+1) + (j+0)]);
		cond_sp4.push_back(obj->p[(i+0)*(h+1) + (j+0)]);
		cond_sp4.push_back(obj->p[(i+1)*(h+1) + (j+1)]);
		cond_sp4.push_back(obj->p[(i+1)*(h+1) + (j+2)]);
		PBDconstraint_bending *condq1 = new PBDconstraint_bending(cond_sp1);
		PBDconstraint_bending *condq2 = new PBDconstraint_bending(cond_sp2);
		PBDconstraint_bending *condq3 = new PBDconstraint_bending(cond_sp3);
		PBDconstraint_bending *condq4 = new PBDconstraint_bending(cond_sp4);

		condq1->stiff = k_bending;
		condq2->stiff = k_bending;
		condq3->stiff = k_bending;
		condq4->stiff = k_bending;
		con.push_back(condq1);
		con.push_back(condq2);
		con.push_back(condq3);
		con.push_back(condq4);
		}
		*/
	}
	printf("vertex : %d\nface : %d\nconstraint : %d\n",obj->p.size(), obj->f.size(), con.size());
//	obj->bind();
	}
	obj->bind();
	shader = new Shader();
	blur_shader = new Shader();
	shader_init();
}

#include<time.h>
#include<omp.h>
void PBD::simul()
{
	if(!obj)return;
	long t1, t2;
	omp_set_num_threads(4);
	t1 = clock();
	prev_process();
	process();
	post_process();
	t2 = clock();
	printf("time = %ld\n",t2 - t1);
}

void PBD::prev_process()
{
	if(!obj)return;
	int i, j;
	int iter;
	// add external force
	for(i=0;i<obj->p.size();i++)
	{
		// gravity;
		obj->p[i]->vel += obj->p[i]->w * vector3f(0.0,-0.098,0.0) *dt;
//		if(obj->p[i]->pos.z > 0.0)obj->p[i]->vel += vector3f(0,0,0.3)*dt;
//		else if (obj->p[i]->pos.z > -0.3) obj->p[i]->vel += vector3f(0,0,-1)*dt;
//		if(i < 20) obj->p[i]->pos += vector3f(0,0,1);
//		else if(i >= obj->p.size() - 20) obj->p[i]->pos += vector3f(0, 0, -1);
		PBDparticle *pp = obj->p[i];
		while(pp)
		{
			pp->pre_pos = obj->p[i]->pos;
			pp = pp->daughter;
		}
	}
	// velocity damping
	for(i=0;i<obj->p.size();i++)
	{
		obj->p[i]->vel *= 0.99;
	}
	for(i=0;i<obj->p.size();i++)
	{
		obj->p[i]->pos = obj->p[i]->pos + obj->p[i]->vel * dt;
		PBDparticle *pp = obj->p[i];
		while(pp)
		{
			pp->pos = obj->p[i]->pos;
			pp = pp->daughter;
		}
	}
}
void PBD::process()
{
	if(!obj)return;
	int i, j;
	int iter;
	//detect collision
	for(i=0;i<obj->p.size();i++)
	{
		if(obj->p[i]->pos.y < 0)
		{
			vector<PBDparticle *> vp;
			vector3f q = obj->p[i]->pos;
			q.y = 0;
			vector3f ns = vector3f(0,1,0);
			vp.push_back(obj->p[i]);

			PBDcollision *col = new PBDcollision(vp,q,ns);
			coll.push_back(col);
		}
		vector3f center;
		double radius;
		
		center = vector3f(0.2,0.7,0.5);
		radius = 0.2;
		if(obj->p[i]->pos.dist(center) < radius)
		{
			
			vector<PBDparticle *> vp;
			vector3f q;
			vector3f ns = obj->p[i]->pos - center;
			ns.normalize();
			q = center + ns * radius;
			vp.push_back(obj->p[i]);

			PBDcollision *col = new PBDcollision(vp,q,ns);
			coll.push_back(col);
			
		}
		/*
		center = vector3f(0.8,0.65,0.8);
		radius = 0.1;
		if(obj->p[i]->pos.dist(center) < radius)
		{
			
			vector<PBDparticle *> vp;
			vector3f q;
			vector3f ns = obj->p[i]->pos - center;
			ns.normalize();
			q = center + ns * radius;
			vp.push_back(obj->p[i]);

			PBDcollision *col = new PBDcollision(vp,q,ns);
			coll.push_back(col);
		}*/
	}
	// constraint
	
	for(iter = 0; iter < iter_max; iter++)
	{
		j = con.size();
		#pragma omp parallel for
		for(i=j-1;i>=0;i--)
		{
//			printf("[%d - %d]\n",omp_get_thread_num(),i);
			con[i]->simulate();
		}
		j = coll.size();
		#pragma omp parallel for
		for(i=0;i<j;i++)
		{
			coll[i]->simulate();
		}
	}
	#pragma omp parallel for
	for(i=0;i<j;i++)
	{
		delete coll[i];
	}
	coll.clear();
}
void PBD::post_process()
{
	if(!obj)return;
	int i, j;
	int iter;

	// update
	for(i=0;i<obj->p.size();i++)
	{
		obj->p[i]->vel = (obj->p[i]->pos - obj->p[i]->pre_pos)/dt;
	}

// chage here if you use CUDA
//	obj->positionData =  (float *)mapGLBufferObject(&m_cuda_posvbo_resource);
//	obj->update();
//	unmapGLBufferObject(m_cuda_posvbo_resource);

	obj->update();
	
	glBindBuffer(GL_ARRAY_BUFFER, shader->positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, obj->f.size()*12*sizeof(float), obj->h_positionData, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, shader->normalBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, obj->f.size()*12*sizeof(float), obj->h_normalData, GL_DYNAMIC_DRAW);
}
#include<glut.h>
vector<vector3f> v;
double radius = 1.0;
static int pre_dep_max = 0;
void PBD::dfs(int dep, vector3f p, vector3f q, vector3f r)
{
 if(dep > dep_max) return;
 vector3f s,t,u;
 s = (p+q)/2;
 s.normalize();
 s *= radius;
 v.push_back(s);
 t = (r+q)/2;
 t.normalize();
 t *= radius;
 v.push_back(t);
 u = (p+r)/2;
 u.normalize();
 u *= radius;
 v.push_back(u);
 dfs(dep+1, s, t, u);
 dfs(dep+1, s, t, q);
 dfs(dep+1, s, p, u);
 dfs(dep+1, r, t, u);
}
void PBD::wahaha()
{
	vector3f a[6];
	v.push_back(vector3f(radius,0,0));
	v.push_back(vector3f(-radius,0,0));
	v.push_back(vector3f(0,radius,0));
	v.push_back(vector3f(0,-radius,0));
	v.push_back(vector3f(0,0,radius));
	v.push_back(vector3f(0,0,-radius));
	
	dfs(1, v[0], v[2], v[4]);
	dfs(1, v[0], v[2], v[5]);
	dfs(1, v[0], v[3], v[4]);
	dfs(1, v[0], v[3], v[5]);
	dfs(1, v[1], v[2], v[4]);
	dfs(1, v[1], v[2], v[5]);
	dfs(1, v[1], v[3], v[4]);
	dfs(1, v[1], v[3], v[5]);
}
void PBD::draw()
{
	obj->draw();

	/*
	if(pre_dep_max != dep_max)
	{
	v.clear();
	wahaha();
	pre_dep_max = dep_max;
	}
	glPointSize(3.0);
	glBegin(GL_POINTS);
	glColor3f(1, 1, 1);
	for(int i=0;i<v.size();i++)
	{
		vector3f dn = v[i];
		dn.normalize();
		glNormal3d(dn.x,dn.y,dn.z);
		glVertex3d(v[i].x, v[i].y, v[i].z);
	}
	glEnd();
	*/
}