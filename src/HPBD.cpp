#include "HPBD.h"

void HPBD::inil()
{
	PBD *base = new PBD();
	base->inil();
	level_max = 2;
	p.push_back(base);
	build();
	base->shader_init();
}
#include<time.h>
#include<omp.h>
void HPBD::simul()
{
	int i, j;
	long t1,t2;
	omp_set_num_threads(4);
	t1 = clock();
	p[0]->prev_process();
	for(int iter=0; iter<2; iter++)
	{
		j = p.size();
		for(i=j-1;i>=0;i--)
		{
			p[i]->process();
			for(j=0;j<p[i]->obj->p.size();j++)
				if(p[i]->obj->p[j]->father)
					p[i]->obj->p[j]->father->pos = p[i]->obj->p[j]->pos;
		}
	}
	p[0]->post_process();
	t2 = clock();
	printf("time : %ld\n",t2-t1);
}
#include<glut.h>
void HPBD::draw()
{
	int i, j;
	p[0]->draw();
	/*
	for(i=0;i<level_max; i++)
	{
		double color = (i+1)/(double)level_max;
		glColor3d(color, color, color);
		glBegin(GL_LINES);
		for(j=0;j<p[i]->con.size();j++)
		{
			if(p[i]->con[j]->n == 2)
			{
				glVertex3f(p[i]->con[j]->p[0]->pos.x,p[i]->con[j]->p[0]->pos.y,p[i]->con[j]->p[0]->pos.z);
				glVertex3f(p[i]->con[j]->p[1]->pos.x,p[i]->con[j]->p[1]->pos.y,p[i]->con[j]->p[1]->pos.z);
			}
		}
		glEnd();
	}
	*/
}
#include <hash_map>
typedef pair<int, int> pii;
void HPBD::build()
{
	PBD *base = p.back();
	int *edge_num = new int[base->obj->p.size()];
	int *tmp_i = new int[base->obj->p.size()];
	bool *is_fine = new bool[base->obj->p.size()];
	bool *is_mark = new bool[base->obj->p.size()];
	double *weight = new double[base->obj->p.size()];
	hash_map<PBDparticle *,int> h;
	vector<int> *v = new vector<int>[base->obj->p.size()];
	PBDparticle **next_pp = new PBDparticle*[base->obj->p.size()];

	int i, j, k;
	int l_iter;
	for(l_iter = 1; l_iter < level_max; l_iter++)
	{

	base = p.back();
	h.clear();
	for(i=0;i<base->obj->p.size();i++)
	{
		edge_num[i] = 0;
		is_fine[i] = false;
		is_mark[i] = false;
		h[base->obj->p[i]] = i;
		tmp_i[i] = i;
		v[i].clear();
		if(base->obj->p[i]->w<E)
			weight[i] = -1;
		else
			weight[i] = 1/base->obj->p[i]->w;
	}
	for(i=0;i<base->con.size();i++)
	{
		if(base->con[i]->n == 2)
		{
			edge_num[h[base->con[i]->p[0]]]++;
			edge_num[h[base->con[i]->p[1]]]++;
			v[h[base->con[i]->p[0]]].push_back(h[base->con[i]->p[1]]);
			v[h[base->con[i]->p[1]]].push_back(h[base->con[i]->p[0]]);
		}
	}
	for(i=0;i<base->obj->p.size();i++)
	{
		sort(v[i].begin(),v[i].end());
		v[i].erase(unique(v[i].begin(),v[i].end()),v[i].end());
	}
	random_shuffle(tmp_i, tmp_i+base->obj->p.size());
	for(k=0;k<base->obj->p.size();k++)
	{
		i = tmp_i[k];
		if(!is_fine[i])
		{
			if(edge_num[i] >= k_e)
			{
				vector3f sum;
				for(j=0;j<v[i].size();j++)
				{
					if(is_fine[v[i][j]] && edge_num[v[i][j]] <= k_e)
						break;
					vector3f xx = (base->obj->p[v[i][j]]->pos - base->obj->p[i]->pos);
					xx.normalize();
					sum += xx;
				}
				if(j == v[i].size())
				{
					if(v[i].size() > 0)
					{
					sum.normalize();
					double ssum=0;
					for(j=0;j<v[i].size();j++)
					{
					vector3f xx = (base->obj->p[v[i][j]]->pos - base->obj->p[i]->pos);
					xx.normalize();
					ssum = max(ssum,abs(xx*sum));
					}
//					printf("%lf\n",ssum);
					if(ssum > 0.92)	// 18 angle
					{
//						base->obj->p[i]->temp = true;
						continue;
					}
					}
					is_fine[i] = true;
					for(j=0;j<v[i].size();j++)
					{
						edge_num[v[i][j]]--;
					}
				}
			}
		}
	}
	PBD *next = new PBD();
	next->obj = new PBDobject();
	int cnt=0;
	while(1)
	{
		int dddddd = 0;
	for(i=0;i<base->obj->p.size();i++)
	{
		if(is_fine[i])
		{
			if(is_mark[i]) continue;
			int pj;
			double mm = -1;
			for(j=0; j<v[i].size();j++)
				if(!is_fine[v[i][j]])
				{
					if(mm<-0.5 || mm > base->obj->p[i]->pos.dist(base->obj->p[v[i][j]]->pos))
					{
						mm = base->obj->p[i]->pos.dist(base->obj->p[v[i][j]]->pos);
						pj = v[i][j];
					}
				}
			if(mm < -0.5) continue;
			dddddd = 1;
			if(rand()%2==0)continue;
			is_mark[i] = true;
			for(j=0;j<v[i].size();j++)
			{
				if(weight[i] < -0.2)
					weight[v[i][j]] = -1;
				else
					weight[v[i][j]] += weight[i] / v[i].size();
				if(is_mark[v[i][j]] || pj == v[i][j])continue;
				v[pj].push_back(v[i][j]);
				v[v[i][j]].push_back(pj);
			}
			sort(v[pj].begin(),v[pj].end());
			v[pj].erase(unique(v[pj].begin(),v[pj].end()),v[pj].end());
		}
	}
		if(dddddd == 0)break;
		cnt++;
	}
	printf("%d\n",cnt);
	for(i=0;i<base->obj->p.size();i++)
	{
		sort(v[i].begin(),v[i].end());
		v[i].erase(unique(v[i].begin(),v[i].end()),v[i].end());
	}
	for(i=0;i<base->obj->p.size();i++)
	{
		if(!is_fine[i])
		{
			PBDparticle *particle = new PBDparticle();
			particle->set(base->obj->p[i]);
			if(weight[i] < -0.2)
				particle->w = 0;
			else
			{
				particle->w = 1/weight[i];
			}
			base->obj->p[i]->temp = true;
			next->obj->p.push_back(particle);
			for(j=0;j<v[i].size();j++)
			{
				if(!is_fine[v[i][j]] && v[i][j] < i)
				{
					PBDconstraint_distance *constraint = new PBDconstraint_distance();
					constraint->n = 2;
					constraint->p.push_back(particle);
					constraint->p.push_back(next_pp[v[i][j]]);
					constraint->del.push_back(vector3f(0,0,0));
					constraint->del.push_back(vector3f(0,0,0));
					constraint->stiff = next->k_stretch * (1 - (double)(l_iter)/level_max *(0.5));

					constraint->init_dist = particle->pos.dist(next_pp[v[i][j]]->pos);

					next->con.push_back(constraint);
				}
			}
			next_pp[i] = particle;
		}
	}
	p.push_back(next);

	printf("- %d\np : %d\nc : %d\n",l_iter,next->obj->p.size(), next->con.size());

	}

	for(i=0;i<level_max; i++)
	{
//		p[i]->iter_max -= (int)(i*1.5);
//		if(p[i]->iter_max <= 0) p[i]->iter_max = 1;
	}
}