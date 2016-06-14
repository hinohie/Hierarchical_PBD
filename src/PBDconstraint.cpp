#include "PBDconstraint.h"
#define M_PI 3.1415926535
void PBDconstraint::simulate()
{
	if(!alive) return;
	double cur_c = constraint();
	del_constraint();

	int i, j;
	double s=0;
	for(i=0;i<n;i++)
	{
		double t = del[i].dist();
		s += p[i]->w * t*t;
	}
	if(s < E)return;
	for(i=0;i<n;i++)
	{
		del[i] = (-cur_c/s)*del[i] * p[i]->w;
		p[i]->pos += del[i] * stiff;
	}
}
void PBDcollision::simulate()
{
	double cur_c = constraint();
	if(cur_c >= 0) return;
	del_constraint();

	int i, j;
	double s=0;
	for(i=0;i<n;i++)
	{
		double t = del[i].dist();
		s += p[i]->w * t*t;
	}
	if(s < E)return;
	for(i=0;i<n;i++)
	{
		del[i] = (-cur_c/s)*del[i] * p[i]->w;
		p[i]->pos += del[i] * stiff;
	}
}

/////////////////////////////////////////////////////
double PBDconstraint_distance::constraint()
{
	double tt = p[0]->pos.dist(p[1]->pos);
	return tt - init_dist;
}
void PBDconstraint_distance::del_constraint()
{
	vector3f nomal = p[0]->pos-p[1]->pos;
	nomal.normalize();
	del[0] = nomal;
	del[1] = nomal*(-1);
}

double PBDconstraint_bending::constraint()
{
	vector3f p01 = p[1]->pos - p[0]->pos;
	vector3f p02 = p[2]->pos - p[0]->pos;
	vector3f p03 = p[3]->pos - p[0]->pos;
	vector3f p012 = p01 % p02;
	vector3f p013 = p01 % p03;
	p012.normalize();
	p013.normalize();
	double tt = acos(p012 * p013);
	tt -= init_angle;
	while(tt>M_PI) tt-= 2*M_PI;
	while(tt<-M_PI)tt+= 2*M_PI;
	return tt;
}
void PBDconstraint_bending::del_constraint()
{	
//	printf("pos = %lf %lf %lf\n", p[0]->pos.x, p[0]->pos.y, p[0]->pos.z);
	vector3f p01 = p[1]->pos - p[0]->pos;
	vector3f p02 = p[2]->pos - p[0]->pos;
	vector3f p03 = p[3]->pos - p[0]->pos;
	vector3f n1 = p01 % p02;
	vector3f n2 = p01 % p03;
	n1.normalize();
	n2.normalize();
	double d = n1 * n2;
	double dd = (1-d*d);
	if(dd<0)dd=0;
	dd=sqrt(dd);
	if(dd<E || (p01%p02).dist() < E || (p01%p03).dist() < E){
		del[0] = del[1] = del[2] = del[3] = vector3f(0,0,0);
		return;
	}
//	printf("%lf %lf  %lf %lf  %lf %lf   %lf %lf %lf   %lf %lf %lf\n",p01.x,p01.y,p02.x,p02.y,p03.x,p03.y,n1.x, n1.y,n1.z, n2.x, n2.y,n2.z);

	vector3f q0,q1,q2,q3;

	q2 = (p01%n2 + (n1%p01)*d)/(p01%p02).dist();
	q3 = (p01%n1 + (n2%p01)*d)/(p01%p03).dist();
	q1 = ((p02%n2 + (n1%p02)*d)/(p01%p02).dist()+(p03%n1 + (n2%p03)*d)/(p01%p03).dist())*(-1);
	q0 = (q1+q2+q3)*(-1);
	
//	printf("q ~ %lf   %lf %lf %lf  %lf %lf %lf  %lf %lf %lf   %lf %lf %lf\n",dd,q0.x,q0.y,q0.z,q1.x,q1.y,q1.z,q2.x,q2.y,q2.z,q3.x,q3.y,q3.z);

	del[0] = q0/dd;
	del[1] = q1/dd;
	del[2] = q2/dd;
	del[3] = q3/dd;
}

double PBDconstraint_volume::constraint()
{
	int i, j;
	double res=0;
	for(i=0;i<n;i+=3)
	{
		res += (p[i+0]->pos%p[i+1]->pos)*p[i+2]->pos;
	}
	return res - init_volume * k_press;
}
void PBDconstraint_volume::del_constraint()
{
	int i, j;
	for(i=0;i<n;i+=3)
	{
		del[i+0] = p[i+1]->pos % p[i+2]->pos;
		del[i+1] = p[i+2]->pos % p[i+0]->pos;
		del[i+2] = p[i+0]->pos % p[i+1]->pos;
	}
}
//////////////////////////////////////////////////////////////////////////////


double PBDcollision::constraint()
{
	double tt = (p[0]->pos - coll_pos) * norm_vector;
	return tt;
}
void PBDcollision::del_constraint()
{
	del[0] = norm_vector;
}