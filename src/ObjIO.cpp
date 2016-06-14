#include <stdio.h>
#include <string.h>
#include "ObjIO.h"
#define MAX_BUF 256

void objModel::parsing_mtl(const char *src)
{
	char buf[MAX_BUF];
	char command[100];
	char elc[MAX_BUF];
	char *name = "";
	double elf;
	double r, g, b;
	int eli;
	int len;
	FILE *fp = fopen(src, "rt");
	if (fp == NULL)
	{
		printf("- no mtl file.\n");
		return;
	}
	objMeterial *cur_mtl = NULL;
	int dd = 0;
	while (fgets(buf, MAX_BUF, fp) > 0)
	{
		len = strlen(buf);

		if (sscanf(buf, "%s", command) <= 0)continue;
		if (strcmp(command, "newmtl") == 0) {
			if (cur_mtl != NULL)
				mtl.push_back(cur_mtl);
			int i, j;
			elc[0] = 0;
			for (i = 7; i<len; i++)
				elc[i - 7] = buf[i];
			elc[len - 8] = 0;
			cur_mtl = new objMeterial();
			cur_mtl->name = new char[len - 6];
			strcpy(cur_mtl->name, elc);
		}
		else if (strcmp(command, "Ns") == 0){
			sscanf(buf, "\tNs %lf", &elf);
			cur_mtl->Ns = elf;
		}
		else if (strcmp(command, "d") == 0){
			sscanf(buf, "\td %lf", &elf);
			cur_mtl->d = elf;
		}
		else if (strcmp(command, "Tr") == 0){
			sscanf(buf, "\tTr %lf", &elf);
			cur_mtl->Tr = elf;
		}
		else if (strcmp(command, "illum") == 0){
			sscanf(buf, "\tillum %d", &eli);
			cur_mtl->illum = eli;
		}
		else if (strcmp(command, "Ka") == 0){
			sscanf(buf, "\tKa %lf %lf %lf", &r, &g, &b);
			cur_mtl->Ambient.setf(r, g, b);
		}
		else if (strcmp(command, "Kd") == 0){
			sscanf(buf, "\tKd %lf %lf %lf", &r, &g, &b);
			cur_mtl->Diffuse.setf(r, g, b);
		}
		else if (strcmp(command, "Ks") == 0){
			sscanf(buf, "\tKs %lf %lf %lf", &r, &g, &b);
			cur_mtl->Specular.setf(r, g, b);
		}
	}
	if (cur_mtl != NULL)
		mtl.push_back(cur_mtl);
	fclose(fp);
}
void objModel::getModel(const char* src)
{
	FILE *fp1 = fopen(src, "rt");
	if (fp1 == NULL){
		printf("no file error. %s\n", src);
		return;
	}
	reset();

	char buf[MAX_BUF];
	char command[100];
	char elc[MAX_BUF];
	char directory[MAX_BUF];
	double elf;
	double x, y, z, w;
	int len;
	int jusuck_count = 0;

	int cur_obj_i = -1;
	objObject *cur_obj = NULL;

	int dd = 0;
	strcpy(directory, src);
	for (int i = 0;; i++)
	{
		if (directory[i] == 0)
		{
			while (i >= 0 && directory[i] != '/'){
				i--;
			}
			directory[i + 1] = 0;
			break;
		}
	}
	int line_num = 0;
	while (fgets(buf, MAX_BUF, fp1) > 0)
	{
		line_num++;
		//	printf("#%d\n",line_num);
		len = strlen(buf);
		if (len>0 && buf[0] == '#')
		{
			jusuck_count++;
			continue;
		}
		else{ jusuck_count = 0; }

		if (sscanf(buf, "%s", command) <= 0) continue;

		if (strcmp(command, "mtllib") == 0)	{
			int i, j;
			int si, ei;
			for (si = 0; directory[si]; si++)
				elc[si] = directory[si];
			ei = 7;
			if (buf[ei] == '.' && buf[ei + 1] == '/')
				ei += 2;
			for (i = ei; i<len; i++)
				elc[i - ei + si] = buf[i];
			elc[len - ei + si] = 0;
			parsing_mtl(elc);
		}
		else if (strcmp(command, "v") == 0) {
			int i, j, k;
			int offset = 0;
			double v[5];
			vector3f cur_vv;
			while (buf[offset] != 'v')offset++;
			offset++;
			while (buf[offset] == '\t' || buf[offset] == 32) offset++;
			for (i = 0; offset<len && i < 4; i++, offset++)
			{
				for (j = 0; buf[offset] != 32 && buf[offset] != 10 && buf[offset] != '\t' && buf[offset]>0; j++, offset++)
					elc[j] = buf[offset];
				if (j == 0)break;
				elc[j] = 0;
				sscanf(elc, "%lf", &v[i]);
			}
			switch (i)
			{
			case 0:
				cur_vv = vector3f();
				break;
			case 1:
				cur_vv = vector3f(v[0]);
				break;
			case 2:
				cur_vv = vector3f(v[0], v[1], 0);
				break;
			case 3:
				cur_vv = vector3f(v[0], v[1], v[2]);
				break;
			case 4:
				cur_vv = vector3f(v[0], v[1], v[2]);	// TODO
				break;
			}
			vv.push_back(cur_vv);
		}
		else if (strcmp(command, "vt") == 0) {
			int i, j, k;
			int offset = 0;
			double v[5];
			vector2f cur_vt;
			while (buf[offset] != 't')offset++;
			offset++;
			while (buf[offset] == '\t' || buf[offset] == 32) offset++;
			for (i = 0; offset<len; i++, offset++)
			{
				for (j = 0; buf[offset] != 32 && buf[offset] != 10 && buf[offset] != '\t' && buf[offset]>0; j++, offset++)
					elc[j] = buf[offset];
				if (j == 0)break;
				elc[j] = 0;
				sscanf(elc, "%lf", &v[i]);
			}
			switch (i)
			{
			case 0:
				cur_vt = vector2f();
				break;
			case 1:
				cur_vt = vector2f(v[0]);
				break;
			case 2:
				cur_vt = vector2f(v[0], v[1]);
				break;
			case 3:
				cur_vt = vector2f(v[0], v[1]);
				break;
			}
			vt.push_back(cur_vt);
		}
		else if (strcmp(command, "vn") == 0) {
			int i, j, k;
			int offset = 0;
			double v[5];
			vector3f cur_vn;
			while (buf[offset] != 'n')offset++;
			offset++;
			while (buf[offset] == '\t' || buf[offset] == 32) offset++;
			for (i = 0; offset<len; i++, offset++)
			{
				for (j = 0; buf[offset] != 32 && buf[offset] != 10 && buf[offset] != '\t' && buf[offset]>0; j++, offset++)
					elc[j] = buf[offset];
				if (j == 0)break;
				elc[j] = 0;
				sscanf(elc, "%lf", &v[i]);
			}
			switch (i)
			{
			case 0:
				cur_vn = vector3f();
				break;
			case 1:
				cur_vn = vector3f(v[0]);
				break;
			case 2:
				cur_vn = vector3f(v[0], v[1], 0);
				break;
			case 3:
				cur_vn = vector3f(v[0], v[1], v[2]);
				break;
			}
			vn.push_back(cur_vn);
		}
		else if (strcmp(command, "vp") == 0) {
			int i, j, k;
			int offset = 0;
			double v[5];
			vector3f cur_vp;
			while (buf[offset] != 'p')offset++;
			offset++;
			while (buf[offset] == '\t' || buf[offset] == 32) offset++;
			for (i = 0; offset<len; i++, offset++)
			{
				for (j = 0; buf[offset] != 32 && buf[offset] != 10 && buf[offset] != '\t' && buf[offset]>0; j++, offset++)
					elc[j] = buf[offset];
				if (j == 0)break;
				elc[j] = 0;
				sscanf(elc, "%lf", &v[i]);
			}
			switch (i)
			{
			case 0:
				cur_vp = vector3f();
				break;
			case 1:
				cur_vp = vector3f(v[0]);
				break;
			case 2:
				cur_vp = vector3f(v[0], v[1], 0);
				break;
			case 3:
				cur_vp = vector3f(v[0], v[1], v[2]);
				break;
			}
			vp.push_back(cur_vp);
		}

		else if (strcmp(command, "g") == 0) {
			cur_obj_i++;
			cur_obj = new objObject();
			obj.push_back(cur_obj);
			if (len - 2 > 0)
			{
				obj[cur_obj_i]->name = new char[len - 2];
				sscanf(buf, "g %s", obj[cur_obj_i]->name);
			}
		}
		else if (strcmp(command, "usemtl") == 0) {
			sscanf(buf, "usemtl %s", elc);
			std::vector<objMeterial*>::iterator it;
			for (it = mtl.begin(); it != mtl.end(); it++)
				if (strcmp(elc, (*it)->name) == 0)
					break;
			if (it != mtl.end())
				obj[cur_obj_i]->mtl = *it;
		}
		else if (strcmp(command, "s") == 0) {
			sscanf(buf, "s %s", elc);
			if (strlen(elc)>1)
				cur_obj->smooth = false;
			else
				cur_obj->smooth = true;
		}
		else if (strcmp(command, "f") == 0) {
			objFace *flag = new objFace();
			int v[3] = { 0, 0, 0 };
			int offset = 2;
			while (1)
			{
				int i, j, k, l = 0;
				for (j = 0; offset<len; j++, offset++)
				{
					if (buf[offset] == 32 || buf[offset] == 10 || buf[offset] == '\t')
						break;
					elc[j] = buf[offset];
				}
				if (offset == len || j == 0)break;
				offset++;;
				elc[j] = 0;
				k = 0;
				for (i = 0; i <= j; i++)
				{
					if (i == j || elc[i] == '/')
					{
						v[l++] = k;
						k = 0;
					}
					else
					{
						k *= 10;
						k += (elc[i] - '0');
					}
				}
				if (v[0]>0)
					flag->vv.push_back(v[0] - 1);
				if (v[1]>0)
					flag->vt.push_back(v[1] - 1);
				if (v[2]>0)
					flag->vn.push_back(v[2] - 1);
			}
			if (cur_obj_i < 0)
			{
				cur_obj_i++;
				cur_obj = new objObject();
				obj.push_back(cur_obj);
			}
			obj[cur_obj_i]->f.push_back(flag);
		}
	}
	fclose(fp1);
}

void objModel::flip_normal(bool flip_all)
{
	vector<vector<int> > f;	// face
	vector<vector3f> fnormal;
	vector<bool> fb;
	int fn;
	hash_map<long long int, vector<pair<int, int>> > he; //edge --> face[]
	double pi = acos(0.0) * 2;

	int i, j, k;
	fn = 0;
	for (int i = 0; i<obj.size(); i++)
	{
		objObject *objj = obj[i];
		for (int j = 0; j<objj->f.size(); j++)
		{
			objFace *face = objj->f[j];
			for (int k = 1; k + 1<face->vv.size(); k++)
			{
				vector<int> cf;
				cf.push_back(face->vv[0]);
				cf.push_back(face->vv[k]);
				cf.push_back(face->vv[k + 1]);
				f.push_back(cf);
				vector3f norm;
				norm = (vv[face->vv[k]] - vv[face->vv[0]]) % (vv[face->vv[k + 1]] - vv[face->vv[k]]);
				//				norm.normalize();
				fnormal.push_back(norm);
				fb.push_back(false);
				fn++;
			}
		}
	}

	for (i = 0; i<fn; i++)
	{
		// make edge map
		int id1, id2, temp;
		long long int id;
		for (j = 0; j<3; j++)
		{
			id1 = f[i][j]; id2 = f[i][(j + 1) % 3];
			if (id1 > id2){ temp = id1; id1 = id2; id2 = temp; }
			id = (((long long int)id1) << 31) + id2;
			he[id].push_back(std::pair<int, int>(i, f[i][(j + 2) % 3]));
		}
	}

	queue<long long int> q;
	int cnt = 0;
	for (int ii = 0; ii<fn; ii++)
	{
		if (fb[ii])continue;
		fb[ii] = true;
		for (j = 0; j<3; j++)
		{
			int id1, id2, temp;
			long long int id;
			id1 = f[ii][j]; id2 = f[ii][(j + 1) % 3];
			if (id1 > id2){ temp = id1; id1 = id2; id2 = temp; }
			id = (((long long int)id1) << 31) + id2;
			q.push(id);
		}
		while (!q.empty())
		{
			long long int id = q.front();
			q.pop();
			if (he[id].size() != 2) continue;
			int f1, f2, temp;
			int d1, d2;
			f1 = he[id][0].first;
			d1 = he[id][0].second;
			f2 = he[id][1].first;
			d2 = he[id][1].second;
			if (fb[f2])
			{
				if (fb[f1]) continue;
				else
				{
					temp = f1; f1 = f2; f2 = temp;
					temp = d1; d1 = d2; d2 = temp;
				}
			}
			// fb[f1] = true; fb[f2] = false;
			int id1, id2;
			id1 = id >> 31;
			id2 = (id) % (1LL << 31);

			bool ccd1, ccd2;

			for (j = 0; j<3; j++)
			{
				if (id1 == f[f1][j] && id2 == f[f1][(j + 1) % 3]) ccd1 = true;
				else if (id2 == f[f1][j] && id1 == f[f1][(j + 1) % 3]) ccd1 = false;
			}
			for (j = 0; j<3; j++)
			{
				if (id1 == f[f2][j] && id2 == f[f2][(j + 1) % 3]) ccd2 = true;
				else if (id2 == f[f2][j] && id1 == f[f2][(j + 1) % 3]) ccd2 = false;
			}

			if (!(ccd1 ^ ccd2))
			{
				cnt++;
				fnormal[f2] = fnormal[f2] * -1;
				temp = f[f2][1]; f[f2][1] = f[f2][2]; f[f2][2] = temp;
			}
			fb[f2] = true;

			for (j = 0; j<3; j++)
			{
				id1 = f[f2][j]; id2 = f[f2][(j + 1) % 3];
				if (id1 == d2 || id2 == d2)
				{
					if (id1 > id2){ temp = id1; id1 = id2; id2 = temp; }
					id = (((long long int)id1) << 31) + id2;
					q.push(id);
				}
			}
		}
	}
	fn = 0;
	for (int i = 0; i<obj.size(); i++)
	{
		objObject *objj = obj[i];
		objObject *objk = new objObject();
		for (int j = 0; j<objj->f.size(); j++)
		{
			objFace *face = objj->f[j];
			for (int k = 1; k + 1<face->vv.size(); k++)
			{
				objFace *gace = new objFace();
				if (flip_all)
				{
					gace->vv.push_back(f[fn][0]);
					gace->vv.push_back(f[fn][k + 1]);
					gace->vv.push_back(f[fn][k]);
				}
				else
				{
					gace->vv.push_back(f[fn][0]);
					gace->vv.push_back(f[fn][k]);
					gace->vv.push_back(f[fn][k + 1]);
				}
				objk->f.push_back(gace);
				fn++;
			}
		}
		delete objj;
		obj[i] = objk;
	}

	printf("fliped_face : %d changed\n", cnt);
}
void objModel::rebuild(double error)
{
	int i, j, k;
	int nx, ny, nz;
	nx = ny = nz = 50;
	double dx, dy, dz;
	double min_x, max_x;
	double min_y, max_y;
	double min_z, max_z;
	int *pp;
	min_x = max_x = vv[0].x;
	min_y = max_y = vv[0].y;
	min_z = max_z = vv[0].z;

	hash_map<int, vector<int> > h;

	for (i = 1; i<vv.size(); i++)
	{
		min_x = min(min_x, vv[i].x);
		min_y = min(min_y, vv[i].y);
		min_z = min(min_z, vv[i].z);
		max_x = max(max_x, vv[i].x);
		max_y = max(max_y, vv[i].y);
		max_z = max(max_z, vv[i].z);
	}
	dx = (max_x - min_x + 10 * error) / nx;
	dy = (max_x - min_x + 10 * error) / ny;
	dz = (max_x - min_x + 10 * error) / nz;
	pp = new int[vv.size()];
	for (i = 0; i<vv.size(); i++)
	{
		int tx, ty, tz;
		tx = (vv[i].x - min_x) / dx;
		ty = (vv[i].y - min_y) / dy;
		tz = (vv[i].z - min_z) / dy;
		if (tx<0)tx = 0; if (tx >= nx)tx = nx - 1;
		if (ty<0)ty = 0; if (ty >= ny)ty = ny - 1;
		if (tz<0)tz = 0; if (tz >= nz)tz = nz - 1;
		h[tx*ny*nz + ty*nz + tz].push_back(i);
		pp[i] = i;
	}
	for (i = 0; i<vv.size(); i++)
	{
		int tx, ty, tz;
		tx = (vv[i].x - min_x) / dx;
		ty = (vv[i].y - min_y) / dy;
		tz = (vv[i].z - min_z) / dy;
		if (tx<0)tx = 0; if (tx >= nx)tx = nx - 1;
		if (ty<0)ty = 0; if (ty >= ny)ty = ny - 1;
		if (tz<0)tz = 0; if (tz >= nz)tz = nz - 1;
		int qx, qy, qz;
		for (qx = tx - 1; qx <= tx + 1; qx++)
			for (qy = ty - 1; qy <= ty + 1; qy++)
				for (qz = tz - 1; qz <= tz + 1; qz++)
				{
					if (qx<0 || qx >= nx)continue;
					if (qy<0 || qy >= ny)continue;
					if (qz<0 || qz >= nz)continue;
					vector<int> hv = h[qx*ny*nz + qy*nz + qz];
					for (j = 0; j<hv.size(); j++)
					{
						if (hv[j] > i && vv[i].dist(vv[hv[j]]) < error)
						{
							k = i;
							int l, ll;
							while (pp[k] != k)k = pp[k];
							l = i;
							while (pp[l] != l)
							{
								ll = pp[l];
								pp[l] = k;
								l = ll;
							}
							pp[hv[j]] = k;
						}
					}
				}
	}
	for (i = 0; i<obj.size(); i++)
	{
		objObject *od = obj[i];
		for (j = 0; j<od->f.size(); j++)
		{
			objFace *face = od->f[j];
			for (k = 0; k<face->vv.size(); k++)
			{
				face->vv[k] = pp[face->vv[k]];
			}
		}
	}
}

void objModel::reordering()
{
	int i, j, k;
	vector<int> x;
	for (i = 0; i<obj.size(); i++)
	{
		objObject *od = obj[i];
		for (j = 0; j<od->f.size(); j++)
		{
			objFace *face = od->f[j];
			for (k = 0; k<face->vv.size(); k++)
			{
				x.push_back(face->vv[k]);
			}
		}
	}
	sort(x.begin(), x.end());
	x.erase(unique(x.begin(), x.end()), x.end());
	vector<int> v_map(vv.size());
	unordered_map<int, set<pair<int, int>>> fs;
	for (i = 0; i<vv.size(); i++)
	{
		v_map[i] = -1;
	}
	for (i = 0; i<x.size(); i++)
		v_map[x[i]] = i;
	for (i = 0; i<obj.size(); i++)
	{
		objObject *od = obj[i];
		objObject *odd = new objObject();
		for (j = 0; j<od->f.size(); j++)
		{
			objFace *face = od->f[j];
			objFace *gace = new objFace();
			for (k = 1; k + 1<face->vv.size(); k++)
			{
				if (fs[face->vv[0]].count(pair<int, int>(face->vv[k], face->vv[k + 1])) == 0)
					break;
				if (fs[face->vv[k]].count(pair<int, int>(face->vv[k + 1], face->vv[k])) == 0)
					break;
				if (fs[face->vv[k + 1]].count(pair<int, int>(face->vv[0], face->vv[k])) == 0)
					break;
			}
			if (k + 1 < face->vv.size())
			{
				for (k = 1; k + 1<face->vv.size(); k++)
				{
					fs[face->vv[0]].insert(pair<int, int>(face->vv[k], face->vv[k + 1]));
					fs[face->vv[k]].insert(pair<int, int>(face->vv[k + 1], face->vv[k]));
					fs[face->vv[k + 1]].count(pair<int, int>(face->vv[0], face->vv[k]));
				}
				for (k = 0; k<face->vv.size(); k++)
				{
					gace->vv.push_back(v_map[face->vv[k]]);
					if (face->vn.size())	gace->vn.push_back(v_map[face->vn[k]]);
					if (face->vt.size()) gace->vt.push_back(v_map[face->vt[k]]);
				}
				odd->f.push_back(gace);
			}
		}
		delete od;
		obj[i] = odd;
	}

	vector<vector3f> nvv, nvn;
	vector<vector2f> nvt;
	if (vv.size())
	{
		nvv.resize(x.size());
		vector<matrix33f> nvvq(x.size());
		vector<vector3f> nvvs(x.size());
		vector<double> nvvd(x.size());
		for (i = 0; i <vv.size(); i++)
			if (v_map[i] >= 0)
			{
				nvv[v_map[i]] = vv[i];
				nvvq[v_map[i]] = vv_quadric[i];
				nvvs[v_map[i]] = vv_vectors[i];
				nvvd[v_map[i]] = vv_d2[i];
			}
		vv = nvv;
		vv_quadric = nvvq;
		vv_vectors = nvvs;
		vv_d2 = nvvd;

	}
	if (vn.size())
	{
		nvn.resize(x.size());
		for (i = 0; i <vn.size(); i++)
			if (v_map[i] >= 0)
				nvn[v_map[i]] = vn[i];
		vn = nvn;
	}
	if (vt.size())
	{
		nvt.resize(x.size());
		for (i = 0; i <vt.size(); i++)
			if (v_map[i] >= 0)
				nvt[v_map[i]] = vt[i];
		vt = nvt;
	}
}
void objModel::decimate(double rate, double alpha)
{
	ready_qem();
	normalize();
	const int MAX_NEIGHBOR_NUM = 12;

	struct Edge {
		int v1, v2;
		vector3f position;

		Edge()
		{
		}

		Edge(int v1, int v2)
			: v1(v1), v2(v2)
		{
			update();
		}

		void update()
		{
			if (v1 > v2) {
				std::swap(v1, v2);
			}
		}
	};

	struct EdgeComparator {
		bool operator()(const Edge &e1, const Edge &e2)
		{
			if (e1.v1 - e2.v1) {
				return e1.v1 < e2.v1;
			}

			return e1.v2 < e2.v2;
		}
	};

	struct Face {
		int v1, v2, v3;
	};

	struct Heap{
		std::vector<std::pair<int, double>> ma;
		std::unordered_map<int, int> s_map;
		int n;
		int ml;
		void inil(int _n){ n = _n + 1; ma.resize(n); ml = 1; }
		void build_map()
		{
			int i;
			s_map.clear();
			for (i = 1; i<ml; i++)
			{
				s_map[ma[i].first] = i;
			}
		}
		void push(int eid, double score)
		{
			ma[ml++] = std::pair<int, double>(eid, score);
			int cur = (ml - 1) / 2;
			while (cur>0)
			{
				int left, right;
				left = cur << 1;
				right = left + 1;
				if (left >= ml)
				{
				}
				else if (right >= ml)
				{
					if (ma[cur].second > ma[left].second)
					{
						s_map[ma[cur].first] = left;
						s_map[ma[left].first] = cur;
						std::swap(ma[cur], ma[left]);
					}
				}
				else
				{
					if (ma[cur].second > ma[left].second)
					{
						s_map[ma[cur].first] = left;
						s_map[ma[left].first] = cur;
						std::swap(ma[cur], ma[left]);
					}
					if (ma[cur].second > ma[right].second)
					{
						s_map[ma[cur].first] = right;
						s_map[ma[right].first] = cur;
						std::swap(ma[cur], ma[right]);
					}
				}
				cur /= 2;
			}
		}
		std::pair<int, double> pop()
		{
			ml--;
			s_map[ma[1].first] = ml;
			s_map[ma[ml].first] = 1;
			std::swap(ma[1], ma[ml]);
			int cur, left, right;
			cur = 1;
			while (cur < ml)
			{
				left = cur << 1;
				right = left + 1;
				if (left >= ml)
				{
					break;
				}
				else if (right >= ml)
				{
					if (ma[cur].second > ma[left].second)
					{
						s_map[ma[cur].first] = left;
						s_map[ma[left].first] = cur;
						std::swap(ma[cur], ma[left]);
						cur = left;
					}
					else
						break;
				}
				else
				{
					if (ma[left].second < ma[right].second)
					{
						if (ma[cur].second > ma[left].second)
						{
							s_map[ma[cur].first] = left;
							s_map[ma[left].first] = cur;
							std::swap(ma[cur], ma[left]);
							cur = left;
						}
						else
							break;
					}
					else
					{
						if (ma[cur].second > ma[right].second)
						{
							s_map[ma[cur].first] = right;
							s_map[ma[right].first] = cur;
							std::swap(ma[cur], ma[right]);
							cur = right;
						}
						else
							break;
					}
				}
			}
			return ma[ml];
		}
		void configure(int eid, double nscore)
		{
			int cur, left, right;
			cur = s_map[eid];
			ma[cur].second = nscore;
			while (cur < ml)
			{
				left = cur << 1;
				right = left + 1;
				if (left >= ml)
				{
					break;
				}
				else if (right >= ml)
				{
					if (ma[cur].second > ma[left].second)
					{
						s_map[ma[cur].first] = left;
						s_map[ma[left].first] = cur;
						std::swap(ma[cur], ma[left]);
						cur = left;
					}
					else
						break;
				}
				else
				{
					if (ma[left].second < ma[right].second)
					{
						if (ma[cur].second > ma[left].second)
						{
							s_map[ma[cur].first] = left;
							s_map[ma[left].first] = cur;
							std::swap(ma[cur], ma[left]);
							cur = left;
						}
						else
							break;
					}
					else
					{
						if (ma[cur].second > ma[right].second)
						{
							s_map[ma[cur].first] = right;
							s_map[ma[right].first] = cur;
							std::swap(ma[cur], ma[right]);
							cur = right;
						}
						else
							break;
					}
				}
			}
			while (cur>0)
			{
				int left, right;
				left = cur << 1;
				right = left + 1;
				if (left >= ml)
				{
				}
				else if (right >= ml)
				{
					if (ma[cur].second > ma[left].second)
					{
						s_map[ma[cur].first] = left;
						s_map[ma[left].first] = cur;
						std::swap(ma[cur], ma[left]);
					}
				}
				else
				{
					if (ma[cur].second > ma[left].second)
					{
						s_map[ma[cur].first] = left;
						s_map[ma[left].first] = cur;
						std::swap(ma[cur], ma[left]);
					}
					if (ma[cur].second > ma[right].second)
					{
						s_map[ma[cur].first] = right;
						s_map[ma[right].first] = cur;
						std::swap(ma[cur], ma[right]);
					}
				}
				cur /= 2;
			}
		}
	} heap;

	std::map<Edge, int, EdgeComparator> edge_ids;

	std::unordered_map<int, Face> faces;
	std::unordered_map<int, Edge> edges;

	std::unordered_map<int, std::unordered_set<int>> e_map;
	std::unordered_map<int, std::set<int>> f_map;

	std::vector<std::pair<int, double>> scores;
	std::vector<int> is_boundary;

	for (int i = 0; i < obj.size(); ++i) {
		auto &o = obj[i];

		for (int j = 0; j < o->f.size(); ++j) {
			auto &f = o->f[j];

			for (int k = 2; k < f->vv.size(); ++k) {
				Face face;

				face.v1 = f->vv[0];
				face.v2 = f->vv[k - 1];
				face.v3 = f->vv[k - 0];

				int fid = faces.size();
				faces[fid] = face;

				Edge ed1, ed2, ed3;
				int e1, e2, e3;

				ed1 = Edge(face.v1, face.v2);
				ed2 = Edge(face.v2, face.v3);
				ed3 = Edge(face.v3, face.v1);

				e1 = edge_ids[ed1];
				if (e1 == 0) {
					e1 = edge_ids.size() + 1;
					edge_ids[ed1] = e1;
				}
				edges[e1] = ed1;

				e2 = edge_ids[ed2];
				if (e2 == 0) {
					e2 = edge_ids.size() + 1;
					edge_ids[ed2] = e2;
				}
				edges[e2] = ed2;

				e3 = edge_ids[ed3];
				if (e3 == 0) {
					e3 = edge_ids.size() + 1;
					edge_ids[ed3] = e3;
				}
				edges[e3] = ed3;

				e_map[face.v1].insert(e1);
				e_map[face.v1].insert(e3);

				e_map[face.v2].insert(e1);
				e_map[face.v2].insert(e2);

				e_map[face.v3].insert(e2);
				e_map[face.v3].insert(e3);

				f_map[face.v1].insert(fid);
				f_map[face.v2].insert(fid);
				f_map[face.v3].insert(fid);
			}
		}
	}

	std::unordered_map<int, double> ss;

	for (int i = 0; i < vv.size(); ++i) {
		double sum_x = 0;
		double me_x, cur_x;
		me_x = vn[i] * vv[i];

		for (auto j : e_map[i])
		{
			int n;

			const auto &e = edges[j];

			if (e.v1 == i) {
				n = e.v2;
			}
			else {
				n = e.v1;
			}

			cur_x = vn[i] * vv[n];
			sum_x += std::abs(cur_x - me_x);
		}
		ss[i] = sum_x;
		ss[i] = 0;
	}

	std::unordered_map<int, int> s_map;
	std::unordered_map<int, double> v_score;

	std::unordered_map<int, int> nv_map;
	std::unordered_map<int, std::unordered_set<int>> fv_map;

	auto edge_score = [this, alpha](int e_id, Edge &e)->double
	{
		/*
		double res = 0;
		double dist_12 = vv[e.v1].dist(vv[e.v2]);
		double area_sum=0;
		for(const auto &oe : edge_opposites[e_id])
		{
		double area = ((vv[oe] - vv[e.v1])%(vv[e.v2]-vv[e.v1])).dist();
		area_sum += area;
		}
		area_sum = area_sum / edge_opposites[e_id].size();
		res = dist_12 * dist_12 / (area_sum + 0.01);
		*/
		double res = 0;
		matrix33f A = vv_quadric[e.v1] + vv_quadric[e.v2];
		vector3f v = vv_vectors[e.v1] + vv_vectors[e.v2];
		double d2 = vv_d2[e.v1] + vv_d2[e.v2];
		matrix33f Ainv;
		vector3f position;

		double det = A.invert(Ainv);

		if (std::abs(det) < E)
		{
			vector3f dv = vv[e.v1] - vv[e.v2];
			vector3f Av2 = A * vv[e.v2];
			vector3f Ad = A * dv;
			double denom = 2 * (dv * Ad);
			if (std::abs(denom) < E)// half-edge collapse
			{
				// Evaluate vAv + 2bv + c
				double cost1 = vv[e.v1] * (A*vv[e.v1]) + 2 * (v*vv[e.v2]) + d2;
				double cost2 = vv[e.v1] * (A*vv[e.v1]) + 2 * (v*vv[e.v2]) + d2;
				if (cost1 > cost2)
				{
					position = vv[e.v2];
					res = cost2;
				}
				else
				{
					position = vv[e.v1];
					res = cost1;
				}
			}
			else
			{
				// ( -2*(vector()*dv) - (dv*Av2) - (v2*Ad) ) / ( 2*(dv*Ad) );
				double a = (-2.0f*(v*dv) - (dv*Av2) - (Ad*(vv[e.v2]))) / (2.0f*(dv*Ad));
				if (a < 0.0)
					a = 0.0;
				else if (a > 1.0)
					a = 1.0;

				position = dv * a + vv[e.v2];
				//printf("%lf %lf %lf , %lf %lf %lf %lf %lf %lf\n",VECTOR3F_PRINTF(position),VECTOR3F_PRINTF(vv[e.v1]), VECTOR3F_PRINTF(vv[e.v2]));

				// Evaluate vAv + 2bv + c
				res = position*(A*position) + 2 * (v*position) + d2;
			}
		}
		else
		{
			position = (Ainv * v) * -1;
			//	position = (vv[e.v1] + vv[e.v2])/2;
			res = position * (A*position) + 2 * (v*position) + d2;
		}
		/*
		vector<int> near;
		std::set_union(f_map[e.v1].begin(), f_map[e.v1].end(), f_map[e.v2].begin(), f_map[e.v2].end(), std::back_inserter(near));
		int i, j;
		double costs[2], cost;
		cost=costs[0]=costs[1] = 0;
		int cn1, cn2, cn;
		cn = cn1=cn2=1;
		for(i=0;i <near.size(); i++)
		{
		auto f = faces[near[i]];
		int v3 = -1;
		if((f.v1 == e.v1 && f.v2 == e.v2) || (f.v2 == e.v1 && f.v1 == e.v2))
		v3 = f.v3;
		if((f.v2 == e.v1 && f.v3 == e.v2) || (f.v3 == e.v1 && f.v2 == e.v2))
		v3 = f.v1;
		if((f.v3 == e.v1 && f.v1 == e.v2) || (f.v1 == e.v1 && f.v3 == e.v2))
		v3 = f.v2;
		if(v3>=0) continue;

		int p,q;
		double r;
		if(f.v1 == e.v1){j=0; p=f.v2; q=f.v3;}
		if(f.v1 == e.v2){j=1; p=f.v2; q=f.v3;}
		if(f.v2 == e.v1){j=0; p=f.v3; q=f.v1;}
		if(f.v2 == e.v2){j=1; p=f.v3; q=f.v1;}
		if(f.v3 == e.v1){j=0; p=f.v1; q=f.v2;}
		if(f.v3 == e.v2){j=1; p=f.v1; q=f.v2;}

		vector3f mp, mq;
		vector3f pq;
		mp = vv[p] - position;
		mq = vv[q] - position;
		pq = vv[p]-vv[q];
		double r1,r2,r3;
		r1= mp.dist(); r2 = mq.dist(); r3 = pq.dist();
		if(r1>r2)swap(r1,r2);
		if(r2>r3)swap(r2,r3);
		if(r1>r2)swap(r1,r2);
		cost += (r3*2 - r1 - r2);
		cn++;
		if(j==0){
		mp = vv[p] - vv[e.v1];
		mq = vv[q] - vv[e.v1];
		cn1++;
		}
		else
		{
		mp = vv[p] - vv[e.v2];
		mq = vv[q] - vv[e.v2];
		cn2++;
		}
		mp.normalize();
		mq.normalize();
		r = acos(mp*mq);
		costs[j] += r;
		}
		res += alpha * (cost);
		*/
		res += alpha * vv[e.v1].dist(vv[e.v2]);
		e.position = position;
		return res;
	};

	is_boundary.resize(vv.size());
	for (int i = 0; i<vv.size(); i++)
		is_boundary[i] = 0;
	heap.inil(edges.size());
	for (auto &e : edges) {
		auto f1 = f_map[e.second.v1];
		auto f2 = f_map[e.second.v2];


		std::vector<int> result;
		std::set_intersection(f1.begin(), f1.end(), f2.begin(), f2.end(), std::back_inserter(result));

		double score = ss[e.second.v1] + ss[e.second.v2];
		double dist_12 = vv[e.second.v1].dist(vv[e.second.v2]);
		score += edge_score(e.first, e.second);
		scores.push_back(std::make_pair(e.first, score));
		heap.push(e.first, score);

		if (result.size() == 1) {
			is_boundary[e.second.v1] = 1;
			is_boundary[e.second.v2] = 1;
			score = std::numeric_limits<double>::infinity();
		}

		v_score[e.second.v1] = std::max(v_score[e.second.v1], score);
		v_score[e.second.v2] = std::max(v_score[e.second.v2], score);
	}
	heap.build_map();

	std::sort(scores.begin(), scores.end(), [](std::pair<int, double> &a, std::pair<int, double> &b) {
		return a.second < b.second;
	});

	//for (auto it = scores.begin(); it != scores.end();) {
	//	auto e = edges[it->first];

	//	if ((v_score[e.v1] == std::numeric_limits<double>::infinity()) || (v_score[e.v2] == std::numeric_limits<double>::infinity())) {
	//		it = scores.erase(it);
	//	} else {
	//		it++;
	//	}
	//}
	for (int i = 0; i < scores.size(); ++i) {
		s_map[scores[i].first] = i;
	}

	int del_cnt = 0;
	int vn = vv.size();
	int sn = scores.size();
	std::vector<bool> vb(vn);

	for (int i = 0; i < vn; ++i) {
		nv_map[i] = i;
		fv_map[i].insert(i);
		vb[i] = false;
	}

	int remain_vn = vn;
	int target_vn = vn - vn*rate;
	if (target_vn < 0) target_vn = 0;
	int percent = 100;
	while (remain_vn > target_vn && del_cnt < scores.size()) {
		if (percent != remain_vn * 100 / vn)
		{
			printf("%d %% remains\n", remain_vn * 100 / vn);
			percent = remain_vn * 100 / vn;
		}
		//		printf("%d / %d\n", remain_vn, vn);
		del_cnt++;
		//		auto q = scores[del_cnt++];
		auto q = heap.pop();

		if (q.second == std::numeric_limits<double>::infinity()) {
			printf("Infinity!!!\n");
			break;
		}

		auto e = edges[q.first];
		if (e.v1 == e.v2)continue;
		if (vb[e.v1] || vb[e.v2])
		{
			continue;
		}
		/*if(v_score[e.v1] == std::numeric_limits<double>::infinity() && v_score[e.v2] == std::numeric_limits<double>::infinity())
		{
		continue;
		}*/
		{
			auto f1 = f_map[e.v1];
			auto f2 = f_map[e.v2];
			if (f1.size() <= 2) continue;
			if (f2.size() <= 2) continue;
			std::vector<int> inter;
			std::vector<int> uni;

			std::set_intersection(f1.begin(), f1.end(), f2.begin(), f2.end(), std::back_inserter(inter));
			std::set_union(f1.begin(), f1.end(), f2.begin(), f2.end(), std::back_inserter(uni));
			if (uni.size() - inter.size() >= MAX_NEIGHBOR_NUM) continue;
			int i, j;
			for (i = 0; i<inter.size(); i++)
			{
				auto f = faces[inter[i]];
				int v3;
				if ((f.v1 == e.v1 && f.v2 == e.v2) || (f.v2 == e.v1 && f.v1 == e.v2))
					v3 = f.v3;
				if ((f.v2 == e.v1 && f.v3 == e.v2) || (f.v3 == e.v1 && f.v2 == e.v2))
					v3 = f.v1;
				if ((f.v3 == e.v1 && f.v1 == e.v2) || (f.v1 == e.v1 && f.v3 == e.v2))
					v3 = f.v2;
				if ((is_boundary[v3] == 1 && f_map[v3].size() <= 2) ||
					(is_boundary[v3] == 0 && f_map[v3].size() <= 3))
					break;
			}
			if (i < inter.size())
				continue;
			for (i = 0; i<uni.size(); i++)
			{
				auto f = faces[uni[i]];
				int v3 = -1;
				if ((f.v1 == e.v1 && f.v2 == e.v2) || (f.v2 == e.v1 && f.v1 == e.v2))
					v3 = f.v3;
				if ((f.v2 == e.v1 && f.v3 == e.v2) || (f.v3 == e.v1 && f.v2 == e.v2))
					v3 = f.v1;
				if ((f.v3 == e.v1 && f.v1 == e.v2) || (f.v1 == e.v1 && f.v3 == e.v2))
					v3 = f.v2;
				if (v3 >= 0) continue;

				vector3f p1, q1, r1;
				vector3f p2, q2, r2;
				p2 = vv[f.v1];
				q2 = vv[f.v2];
				r2 = vv[f.v3];
				if (f.v1 == e.v1 || f.v1 == e.v2) p1 = e.position; else p1 = vv[f.v1];
				if (f.v2 == e.v1 || f.v2 == e.v2) q1 = e.position; else q1 = vv[f.v2];
				if (f.v3 == e.v1 || f.v3 == e.v2) r1 = e.position; else r1 = vv[f.v3];

				vector3f n1, n2;
				n1 = (q1 - p1) % (r1 - q1);
				n2 = (q2 - p2) % (r2 - q2);
				n1.normalize();
				n2.normalize();
				if (n1*n2 < 0.2)
					break;
			}
			if (i < uni.size())
				continue;
		}


		remain_vn--;
		if (is_boundary[e.v2])
		{
			std::swap(e.v1, e.v2);
		}

		vv_quadric[e.v1] += vv_quadric[e.v2];
		vv_vectors[e.v1] += vv_vectors[e.v2];
		vv_d2[e.v1] += vv_d2[e.v2];
		ss[e.v1] += ss[e.v2];

		if (!is_boundary[e.v1] && !is_boundary[e.v2])
		{
			vv[e.v1] = e.position;
			//			vv[e.v1] = (vv[e.v1] + vv[e.v2])/2;
			auto es = e_map[e.v1];
			for (auto &edge_id : es)
			{
				auto &edge = edges[edge_id];

				heap.configure(edge_id, ss[edge.v1] + ss[edge.v2] + edge_score(edge_id, edge));

				/*
				int offset = s_map[edge_id];
				scores[offset].second = ss[edge.v1] + ss[edge.v2];
				scores[offset].second += edge_score(edge_id, edge);

				while (offset > del_cnt && scores[offset].second < scores[offset - 1].second) {
				s_map[scores[offset - 1].first] = offset;
				s_map[edge_id] = offset - 1;
				std::swap(scores[offset], scores[offset - 1]);
				offset--;
				}

				while (offset < sn - 1 && scores[offset].second > scores[offset + 1].second) {
				s_map[scores[offset + 1].first] = offset;
				s_map[edge_id] = offset + 1;
				std::swap(scores[offset], scores[offset + 1]);
				offset++;
				}
				*/
			}
		}
		vv[e.v2] = vv[e.v1];
		vv_quadric[e.v2] = vv_quadric[e.v1];
		vv_vectors[e.v2] = vv_vectors[e.v1];
		vv_d2[e.v2] = vv_d2[e.v1];

		auto es = e_map[e.v2];
		for (auto &k : fv_map[e.v2]) {
			nv_map[k] = e.v1;
			fv_map[e.v1].insert(k);
		}

		for (auto &edge_id : es) {
			auto &edge = edges[edge_id];

			if (edge.v1 == edge.v2) continue;

			if (e.v2 == edge.v1) {
				edge.v1 = e.v1;
			}
			else {
				edge.v2 = e.v1;
			}
			edge.update();

			if (edge.v1 == edge.v2) { continue; }

			e_map[e.v1].insert(edge_id);
			heap.configure(edge_id, ss[edge.v1] + ss[edge.v2] + edge_score(edge_id, edge));
			/*
			int offset = s_map[edge_id];
			scores[offset].second = ss[edge.v1] + ss[edge.v2];
			scores[offset].second += edge_score(edge_id, edge);;

			while (offset > del_cnt && scores[offset].second < scores[offset - 1].second) {
			s_map[scores[offset - 1].first] = offset;
			s_map[edge_id] = offset - 1;
			std::swap(scores[offset], scores[offset - 1]);
			offset--;
			}

			while (offset < sn - 1 && scores[offset].second > scores[offset + 1].second) {
			s_map[scores[offset + 1].first] = offset;
			s_map[edge_id] = offset + 1;
			std::swap(scores[offset], scores[offset + 1]);
			offset++;
			}
			*/
		}
		{
			auto &f1 = f_map[e.v1];
			auto &f2 = f_map[e.v2];

			std::vector<int> inter;
			std::set_intersection(f1.begin(), f1.end(), f2.begin(), f2.end(), std::back_inserter(inter));

			for (auto fid : f2)
			{
				f1.insert(fid);
			}

			f2.clear();
			for (auto const fid : inter)
			{
				auto &f = faces[fid];

				int v3;
				if ((f.v1 == e.v1 && f.v2 == e.v2) || (f.v2 == e.v1 && f.v1 == e.v2))
					v3 = f.v3;
				if ((f.v2 == e.v1 && f.v3 == e.v2) || (f.v3 == e.v1 && f.v2 == e.v2))
					v3 = f.v1;
				if ((f.v3 == e.v1 && f.v1 == e.v2) || (f.v1 == e.v1 && f.v3 == e.v2))
					v3 = f.v2;
				f_map[v3].erase(fid);
				f.v1 = f.v2 = f.v3 = -1;
				f1.erase(fid);
			}
			for (auto fid : f1)
			{
				auto &f = faces[fid];
				if (f.v1 == e.v2) f.v1 = e.v1;
				if (f.v2 == e.v2) f.v2 = e.v1;
				if (f.v3 == e.v2) f.v3 = e.v1;
			}
		}

		vb[e.v2] = true;

		//		edges.erase(q.first);
	}
	printf("final : %d / %d\n", remain_vn, vn);
	/*
	for(int i=0; i<obj.size(); i++)
	{
	objObject *objj = obj[i];
	objObject *objk = new objObject();
	for(int j=0; j<objj->f.size(); j++)
	{
	objFace *face = objj->f[j];
	for(int k=1; k+1<face->vv.size(); k++)
	{
	objFace *gace = new objFace();
	gace->vv.push_back(nv_map[face->vv[0+0]]);
	gace->vv.push_back(nv_map[face->vv[k+0]]);
	gace->vv.push_back(nv_map[face->vv[k+1]]);
	if(face->vn.size() > 0)
	{
	gace->vn.push_back(nv_map[face->vn[0+0]]);
	gace->vn.push_back(nv_map[face->vn[k+0]]);
	gace->vn.push_back(nv_map[face->vn[k+1]]);
	}
	if(face->vt.size() > 0)
	{
	gace->vt.push_back(nv_map[face->vt[0+0]]);
	gace->vt.push_back(nv_map[face->vt[k+0]]);
	gace->vt.push_back(nv_map[face->vt[k+1]]);
	}
	if(gace->vv[0] == gace->vv[1] || gace->vv[1] == gace->vv[2] || gace->vv[2] == gace->vv[0])
	{
	delete gace;
	continue;
	}
	objk->f.push_back(gace);
	}
	}
	delete objj;
	obj[i] = objk;
	}
	*/
	for (int i = 0; i<obj.size(); i++)
	{
		objObject *objj = obj[i];
		delete objj;

		objObject *objk = new objObject();
		for (auto f : faces)
		{
			//			if(f.second.v1 < 0)
			//			printf("%d : %d %d %d\n",f.first, f.second.v1, f.second.v2, f.second.v3);
			if (f.second.v1 >= 0)
			{
				if (f.second.v1 != f.second.v2 && f.second.v2 != f.second.v3)
				{
					objFace *face = new objFace();
					face->vv.push_back(f.second.v1);
					face->vv.push_back(f.second.v2);
					face->vv.push_back(f.second.v3);
					objk->f.push_back(face);
				}
			}
		}
		obj[i] = objk;
	}
}

void objModel::ready_qem()
{
	int i, j, k;

	vv_d2.resize(vv.size());
	vv_vectors.resize(vv.size());
	vv_quadric.resize(vv.size());
	for (i = 0; i<vv.size(); i++)
	{
		vv_d2[i] = 0;
		vv_vectors[i] = vector3f();
		vv_quadric[i] = matrix33f();
	}
	for (int i = 0; i<obj.size(); i++)
	{
		objObject *objj = obj[i];
		for (int j = 0; j<objj->f.size(); j++)
		{
			objFace *face = objj->f[j];
			for (int k = 1; k + 1<face->vv.size(); k++)
			{
				vector3f n = (vv[face->vv[k]] - vv[face->vv[0]]) % (vv[face->vv[k + 1]] - vv[face->vv[k]]);
				double area = std::abs(n.dist() / 2);
				area = 1;
				matrix33f q;
				double d;

				n.normalize();

				q.a[0][0] = n.x*n.x; q.a[0][1] = q.a[1][0] = n.x * n.y; q.a[0][2] = q.a[2][0] = n.x*n.z;
				q.a[1][1] = n.y*n.y; q.a[1][2] = q.a[2][1] = n.y * n.z;
				q.a[2][2] = n.z*n.z;

				d = -(n*vv[face->vv[0]]);
				//printf("%lf %lf %lf  %lf  %lf\n",VECTOR3F_PRINTF(n), area, d);
				vv_quadric[face->vv[0]] += (q * area);
				vv_vectors[face->vv[0]] += n * d * area;
				vv_d2[face->vv[0]] += d*d * area;

				vv_quadric[face->vv[k]] += (q * area);
				vv_vectors[face->vv[k]] += n * d * area;
				vv_d2[face->vv[k]] += d*d * area;

				vv_quadric[face->vv[k + 1]] += (q * area);
				vv_vectors[face->vv[k + 1]] += n * d * area;
				vv_d2[face->vv[k + 1]] += d*d * area;
			}
		}
	}
}


void objModel::resize()
{
	int i, j, k;
	double min_x, max_x;
	double min_y, max_y;
	double min_z, max_z;
	min_x = max_x = vv[0].x;
	min_y = max_y = vv[0].y;
	min_z = max_z = vv[0].z;
	for (i = 1; i<vv.size(); i++)
	{
		min_x = min(min_x, vv[i].x);
		min_y = min(min_y, vv[i].y);
		min_z = min(min_z, vv[i].z);
		max_x = max(max_x, vv[i].x);
		max_y = max(max_y, vv[i].y);
		max_z = max(max_z, vv[i].z);
	}
	double maxx = max(max(max_x - min_x, max_y - min_y), max_z - min_z);
	for (i = 0; i<vv.size(); i++)
	{
		vv[i].x = (vv[i].x - min_x) / maxx + (maxx - (max_x - min_x)) / maxx / 2;
		vv[i].y = (vv[i].y - min_y) / maxx + (maxx - (max_y - min_y)) / maxx / 2;
		vv[i].z = (vv[i].z - min_z) / maxx + (maxx - (max_z - min_z)) / maxx / 2;
	}
}
void objModel::normalize()
{
	vn.clear();

	int i, j, k, l;
	j = vv.size();
	l = obj.size();
	vector3f *normal_mean = new vector3f[j];
	int *normal_mean_num = new int[j];

	for (i = 0; i<j; i++)
	{
		normal_mean[i] = vector3f();
		normal_mean_num[i] = 0;
	}
	for (int ii = 0; ii<l; ii++)
	{
		k = obj[ii]->f.size();
		for (i = 0; i<k; i++)
		{
			objFace * face = obj[ii]->f[i];
			face->vn.clear();
			face->vn.resize(face->vv.size());
			for (int jj = 1; jj + 1<face->vv.size(); jj++)
			{
				vector3f n1;
				vector3f p01 = vv[face->vv[jj]] - vv[face->vv[0]];
				vector3f p12 = vv[face->vv[jj + 1]] - vv[face->vv[jj]];
				n1 = p01 % p12;
				n1.normalize();

				int id;
				id = face->vv[0];
				normal_mean[id] += n1;
				normal_mean_num[id] ++;
				face->vn[0] = id;
				id = face->vv[jj];
				normal_mean[id] += n1;
				normal_mean_num[id] ++;
				face->vn[jj] = id;
				id = face->vv[jj + 1];
				normal_mean[id] += n1;
				normal_mean_num[id] ++;
				face->vn[jj + 1] = id;
			}
		}
	}
	for (i = 0; i<j; i++)
	{
		if (normal_mean_num[i]>0)
			normal_mean[i] /= normal_mean_num[i];
		vn.push_back(normal_mean[i]);
	}

	delete[] normal_mean;
	delete[] normal_mean_num;
}
void objModel::getModel_3ds(const char *filename)
{
	int index = 0;
	FILE *l_file;
	if (fopen_s(&l_file, filename, "rb") != 0) {
		printf("error : 3DS load\n");
		exit(0);
	}
	int i;
	unsigned short l_chunk_id;
	unsigned int l_chunk_lenght;
	unsigned char l_char;
	unsigned short l_qty;
	unsigned short l_face_flags;

	objObject *cur_obj = new objObject();
	obj.push_back(cur_obj);

	fseek(l_file, 0l, SEEK_END);
	long l_filelength = ftell(l_file);
	fseek(l_file, 0l, SEEK_SET);
	while (ftell(l_file) < l_filelength) {
		fread(&l_chunk_id, 2, 1, l_file);
		fread(&l_chunk_lenght, 4, 1, l_file);
		switch (l_chunk_id)
		{
		case 0x4d4d:
			break;
		case 0x3d3d:
			break;
		case 0x4000:
			i = 0;
			do {
				fread(&l_char, 1, 1, l_file);
				i++;
			} while (l_char != '\0' && i<20);
			break;
		case 0x4100:

			break;
		case 0x4110:
			fread(&l_qty, sizeof(unsigned short), 1, l_file);
			for (i = 0; i<l_qty; i++) {
				double dx, dy, dz;
				fread(&dx, sizeof(double), 1, l_file);
				fread(&dy, sizeof(double), 1, l_file);
				fread(&dz, sizeof(double), 1, l_file);

				vv.push_back(vector3f(dx, dy, dz));
			}
			break;
		case 0x4120:
			fread(&l_qty, sizeof(unsigned short), 1, l_file);
			for (i = 0; i<l_qty; i++) {
				unsigned short face_id[3];
				fread(&face_id[0], sizeof(unsigned short), 1, l_file);
				fread(&face_id[1], sizeof(unsigned short), 1, l_file);
				fread(&face_id[2], sizeof(unsigned short), 1, l_file);
				fread(&l_face_flags, sizeof(unsigned short), 1, l_file);

				objFace *cur_face = new objFace();
				cur_face->vv.push_back(face_id[0]);
				cur_face->vv.push_back(face_id[1]);
				cur_face->vv.push_back(face_id[2]);
				cur_obj->f.push_back(cur_face);
			}
			break;
			break;
		case 0x4140:
		default:
			fseek(l_file, l_chunk_lenght - 6, SEEK_CUR);
		}
	}
	fclose(l_file);
}

void objModel::SimpleSquareModel(int n, int m, double sx, double sy, double sz,double length_n, double length_m,  vector3f axis_n, vector3f axis_m)
{
	int i, j, k;
	axis_n.normalize(); if(axis_n.dist() < E) axis_n = vector3f(1,0,0);
	axis_m.normalize(); if(axis_m.dist() < E) axis_m = vector3f(0,1,0);

	if(n<1)n=1;if(m<1)m=1;

	vv.clear();
	vn.clear();
	vt.clear();
	double dx = length_n / n;
	double dy = length_m / m;
	vector3f s = vector3f(sx,sy,sz);

	// push v
	for(i=0; i<=n; i++)
		for(j=0; j<=m; j++)
		{
			vv.push_back(s + axis_n * i * dx + axis_m * j * dy);
		}
	// push f

	for(i=0; i<obj.size(); i++) if(obj[i] != NULL)delete obj[i];
	obj.clear();

	objObject *objs = new objObject();
	for(i=0; i<n; i++)
		for(j=0; j<m; j++)
		{
			objFace *face1 = new objFace();
			objFace *face2 = new objFace();

			face1->vv.push_back((i+0)*(m+1) + (j+0));
			face1->vv.push_back((i+1)*(m+1) + (j+0));
			face1->vv.push_back((i+0)*(m+1) + (j+1));
			
			face2->vv.push_back((i+0)*(m+1) + (j+1));
			face2->vv.push_back((i+1)*(m+1) + (j+0));
			face2->vv.push_back((i+1)*(m+1) + (j+1));

			objs->f.push_back(face1);
			objs->f.push_back(face2);
		}
	obj.push_back(objs);
	
//	normalize();
}

#define WRITE_LINE(ppp) fprintf(fp, "%s\n",(ppp));
static void write_vectors_v(FILE *fp, std::vector<vector3f> v)
{
	char vv[1024];
	int i, j, l;

	l = v.size();
	for (i = 0; i<l; i++)
	{
		sprintf(vv, "v %lf %lf %lf", VECTOR3F_PRINTF(v[i]));
		WRITE_LINE(vv);
	}
}
static void write_vectors_vn(FILE *fp, std::vector<vector3f> v)
{
	char vn[1024];
	int i, j, l;

	l = v.size();
	for (i = 0; i<l; i++)
	{
		sprintf(vn, "vn %lf %lf %lf", VECTOR3F_PRINTF(v[i]));
		WRITE_LINE(vn);
	}
}
static void write_flag(FILE *fp, std::vector<objFace *> v)
{
	char ff[1024];
	char tf[1024];

	char vv[1024];
	char vt[1024];
	char vn[1024];
	for (std::vector<objFace *>::iterator i = v.begin(); i != v.end(); i++)
	{
		sprintf(ff, "f");
		for (int j = 0; j<(*i)->vv.size(); j++)
		{
			sprintf(vv, "%d", (*i)->vv[j] + 1);
			if ((*i)->vn.size())
			{
				if ((*i)->vt.size())
					sprintf(vt, "/%d", (*i)->vt[j] + 1);
				else
					sprintf(vt, "/");
				sprintf(vn, "/%d", (*i)->vn[j] + 1);
			}
			else
			{
				if ((*i)->vt.size())
					sprintf(vt, "/%d", (*i)->vt[j] + 1);
				else
					sprintf(vt, "");

				sprintf(vn, "");
			}
			sprintf(tf, " %s%s%s", vv, vt, vn);
			strcat(ff, tf);
		}
		WRITE_LINE(ff);
	}
}
static void write_obj(FILE *fp, std::vector<objObject *> v)
{
	int i, j, l;
	l = v.size();
	char *blank = "";

	char on[1024];
	char pn[1024];
	char qn[1024];
	for (i = 0; i<l; i++)
	{
		sprintf(qn, "#\n#\tObject %s\n#\n", v[i]->name);
		sprintf(on, "g %s", v[i]->name);
		sprintf(pn, "s %s", v[i]->smooth ? "1" : "off");
		WRITE_LINE(qn);
		WRITE_LINE(on);
		WRITE_LINE(pn);

		write_flag(fp, v[i]->f);
		sprintf(qn, "# %d flags", v[i]->f.size());
		WRITE_LINE(qn);
		WRITE_LINE(blank);
	}
}
static void write_model(FILE *fp, objModel *model)
{
	char *blank = "";

	char *temp1 = "# This obj file is made by hinohie`s program";
	char *temp2 = "# So here may feel with drug state";

	char vv_line[1024];

	WRITE_LINE(temp1);
	WRITE_LINE(temp2);
	WRITE_LINE(blank);

	write_vectors_v(fp, model->vv);
	sprintf(vv_line, "# %d vertex", model->vv.size());
	WRITE_LINE(vv_line);
	WRITE_LINE(blank);

	write_vectors_vn(fp, model->vn);
	sprintf(vv_line, "# %d vertex", model->vn.size());
	WRITE_LINE(vv_line);
	WRITE_LINE(blank);

	write_obj(fp, model->obj);
	sprintf(vv_line, "# %d objects", model->obj.size());
	WRITE_LINE(vv_line);
}

void objModel::writeModel(const char *filename)
{
	FILE *fp = fopen(filename, "wt");

	write_model(fp, this);

	fclose(fp);
}