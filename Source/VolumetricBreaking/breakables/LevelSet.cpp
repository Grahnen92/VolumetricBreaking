// Fill out your copyright notice in the Description page of Project Settings.
#include "LevelSet.h"
#include "VolumetricBreaking.h"


LevelSet::LevelSet()
{
	iso_val = 0.f;

	dims = sf.getDimsPtr();
	res = sf.getResPtr();
}

LevelSet::LevelSet(const FVector &_res, const FVector &_dims, float _iso_val) 
	: sf(_res, _dims)
{
	dims = sf.getDimsPtr();
	res = sf.getResPtr();

	iso_val = _iso_val;

	/**dims = _dims;
	*res = _res;

	sf.allocateData();*/
}

LevelSet::LevelSet(int _cubed_res, float _cubed_dim, float _iso_val)
	: sf(_cubed_res, _cubed_dim)
{
	dims = sf.getDimsPtr();
	res = sf.getResPtr();

	iso_val = _iso_val;
	

	sf.allocateData();
}

LevelSet::~LevelSet()
{
}

//this will reallocate all data of the scalarfield
void LevelSet::setRes(int x, int y, int z)
{
	//sf.setRes(FVector(x, y, z));
	res->X = x; res->Y = y; res->Z = z;
}
//this will reallocate all data of the scalarfield
void LevelSet::setRes(const FVector &_res) {
	//sf.setRes(_res);
	*res = _res;
}
void LevelSet::getRes(int &x, int &y, int &z) {
	//FVector res = sf.getRes();
	//x = res.X; y = res.Y; z = res.Z;
	x = res->X; y = res->Y; z = res->Z;
}
void  LevelSet::getRes(FVector &_res)
{
	//_res = sf.getRes();
	_res = *res;
}
FVector LevelSet::getRes()
{
	//return sf.getRes();
	return *res;
}

//settings dims has no effect until the level set is (re)filled with data
void LevelSet::setDims(const FVector &_dims) {
	//sf.setDims(_dims);
	*dims = _dims;
}
void LevelSet::getDims(FVector &_dims)
{
	//_dims = sf.getDims();
	_dims = *dims;
}
FVector LevelSet::getDims() {
	//return sf.getDims();
	return *dims;
}

void  LevelSet::setPos(const FVector &_pos)
{
	w_pos = _pos;
}
void  LevelSet::getPos(FVector &_pos) {
	_pos = w_pos;
}
FVector LevelSet::getPos()
{
	return w_pos;
}

void LevelSet::setIsoVal(float _val)
{
	iso_val = _val;
}
float LevelSet::getIsoVal()
{
	return iso_val;
}

ScalarField<float>* LevelSet::getScalarField()
{
	return &sf;
}

void LevelSet::sphereSignedDistance(FVector _w_pos)
{
	w_pos = _w_pos;
	//TODO: this is unsafe if the resolution or dimension are not cubed
	/*FVector dim = sf.getDims();
	FVector res = sf.getRes();*/
	float*** data = sf.getDataPtr();
	float radius =dims->X / 2.0f - (dims->X / res->X);
	FVector origin(0.0f, 0.0f, 0.0f);
	FVector p1;
	FVector p2;
	FVector dist_vec;
	float dist;

	sf.setLargestValue(radius);

	float x, y, z;
	for (int i = 0; i < res->X; i++)
	{
		for (int j = 0; j < res->Y; j++)
		{
			for (int k = 0; k < res->Z; k++)
			{
				x = ((i - (res->X / 2)) / res->X)*dims->X + (dims->X /res->X)*0.5f;
				y = ((j - (res->Y / 2)) / res->Y)*dims->Y + (dims->Y /res->Y)*0.5f;
				z = ((k - (res->Z / 2)) /res->Z)*dims->Z + (dims->Z /res->Z)*0.5f;

				p1 = FVector(x, y, z);
				p2 = p1;
				p2.Normalize();
				p2 = p2*radius;

				dist_vec = p2 - p1;
				dist = dist_vec.Size();
				dist_vec.Normalize();

				//outside of sphere
				if (FVector::DotProduct(p2, dist_vec) < 0)
				{
					data[i][j][k] = -dist;
				}
				else //inside of sphere
				{
					data[i][j][k] = dist;
				}



			}
		}
	}
}

void LevelSet::cubeSignedDistance(FVector _w_pos)
{
	w_pos = _w_pos;

	//FVector dim = sf.getDims();
	//FVector res = sf.getRes();
	float*** data = sf.getDataPtr();

	//TODO: this is unsafe if the resolution or dimension are not cubed
	//cube_half_length
	float chl = dims->X / 2.0f - (dims->X / res->X);
	FVector origin(0.0f, 0.0f, 0.0f);
	FVector p1;
	FVector p2;
	FVector dist_vec;

	float *lv = sf.getLargestValuePtr();
	*lv = 0.f;

	float x, y, z;
	for (int i = 0; i < res->X; i++)
	{
		for (int j = 0; j < res->Y; j++)
		{
			for (int k = 0; k < res->Z; k++)
			{
				x = ((i - (res->X / 2)) / res->X)*dims->X + (dims->X / res->X)*0.5f;
				y = ((j - (res->Y / 2)) / res->Y)*dims->Y + (dims->Y / res->Y)*0.5f;
				z = ((k - (res->Z / 2)) / res->Z)*dims->Z + (dims->Z / res->Z)*0.5f;

				float x_dist_max = x - chl;
				float x_dist_min = x - (-chl);
				float x_dist;
				if (FMath::Abs(x_dist_max) < FMath::Abs(x_dist_min))
				{
					x_dist = x_dist_max;
				}
				else
				{
					x_dist = -x_dist_min;
				}
				x_dist = -x_dist;

				float y_dist_max = y - chl;
				float y_dist_min = y - (-chl);
				float y_dist;
				if (FMath::Abs(y_dist_max) < FMath::Abs(y_dist_min))
				{
					y_dist = y_dist_max;
				}
				else
				{
					y_dist = -y_dist_min;
				}
				y_dist = -y_dist;

				float z_dist_max = z - chl;
				float z_dist_min = z - (-chl);
				float z_dist;
				if (FMath::Abs(z_dist_max) < FMath::Abs(z_dist_min))
				{
					z_dist = z_dist_max;
				}
				else
				{
					z_dist = -z_dist_min;
				}
				z_dist = -z_dist;

				data[i][j][k] = FMath::Min3(x_dist, y_dist, z_dist);
				if (FMath::Abs(data[i][j][k]) > *lv)
				{
					*lv = FMath::Abs(data[i][j][k]);
				}
			}
		}
	}
}

void LevelSet::voronoiCellSignedDist(voro::voronoicell_neighbor* _v_cell, voro::container* _vcon, const int& _cell_id, const std::vector<FVector> &_v_particles, FVector &_vd_dims, const  FVector& _w_pos)
{
	//FVector dim = sf.getDims();
	//FVector res = sf.getRes();

	//Verts
	std::vector<double> cell_verts;
	//_v_cell->vertices(_w_pos.X, _w_pos.Y, _w_pos.Z, cell_verts);
	_v_cell->vertices(cell_verts);
	//edges
	std::vector<int> cell_edges;
	_v_cell->edges(cell_edges);
	//faces
	std::vector<int> cell_faces;
	_v_cell->face_vertices(cell_faces);
	std::vector<int> cell_face_orders;
	_v_cell->face_orders(cell_face_orders);
	std::vector<double> cell_face_normals;
	_v_cell->normals(cell_face_normals);

	double cx, cy, cz;
	_v_cell->centroid(cx, cy, cz);
	FVector centroid(cx, cy, cz);

	FVector min_extent;
	FVector max_extent;
	FVector tmp_dims;
	OMath::findMaxMinExtent(cell_verts, min_extent, max_extent);

	tmp_dims = (max_extent - min_extent);
	tmp_dims = FVector(FMath::Ceil(tmp_dims.X), FMath::Ceil(tmp_dims.Y), FMath::Ceil(tmp_dims.Z));

	//float max_dim = FMath::Max3(tmp_dims.X, tmp_dims.Y, tmp_dims.Z);
	//max_dim = FMath::Floor(max_dim);
	//setDims(FVector(max_dim, max_dim, max_dim));
	setDims(tmp_dims);
	//max_dim = max_dim*0.25f;
	setRes(FMath::Floor(tmp_dims.X*1.f), FMath::Floor(tmp_dims.Y*1.f), FMath::Floor(tmp_dims.Z*1.f));
	//setRes(max_dim, max_dim, max_dim);

	FVector c_offset = min_extent + (tmp_dims/2.f);
	_v_cell->vertices(-c_offset.X, -c_offset.Y, -c_offset.Z, cell_verts);


	reallocateData();

	float*** data = sf.getDataPtr();

	FVector nb_res = *res - FVector(2.f, 2.f, 2.f);

	float x, y, z;
	float ox = (dims->X / nb_res.X);
	float oy = (dims->Y / nb_res.Y);
	float oz = (dims->Z / nb_res.Z);
	*dims = FVector(ox, oy, oz) + *dims;
	ox = ox*0.5f; oy = oy*0.5f; oz = oz*0.5f;
	float resxm1 = (res->X - 1);
	float resym1 = (res->Y - 1);
	float reszm1 = (res->Z - 1);
	float resxm1d2 = (resxm1 / 2.f);
	float resym1d2 = (resym1 / 2.f);
	float reszm1d2 = (reszm1 / 2.f);

	
	w_pos = _v_particles[_cell_id] + c_offset;
	//w_pos = centroid;
	int tmp_cell_id;
	FVector curr_p;

	float *lv = sf.getLargestValuePtr();
	sf.setLargestValue(0.f);

	for (int i = 0; i < res->X; i++)
	{
		for (int j = 0; j < res->Y; j++)
		{
			for (int k = 0; k < res->Z; k++)
			{
				x = ((i - resxm1d2) / resxm1)*dims->X;// +c_offset.X;
				y = ((j - resym1d2) / resym1)*dims->Y;// +c_offset.Y;
				z = ((k - reszm1d2) / reszm1)*dims->Z;// +c_offset.Y;
				curr_p = FVector(x, y, z);

				data[i][j][k] = OMath::distToVoronoiCell(curr_p, cell_verts, cell_edges, cell_faces, cell_face_orders, cell_face_normals);
				if (FMath::Abs(data[i][j][k]) > *lv)
					*lv = data[i][j][k];

				curr_p = curr_p + w_pos;
				double rx, ry, rz;
				/*if (!_vcon->find_voronoi_cell(curr_p.X, curr_p.Y, curr_p.Z, rx, ry, rz, tmp_cell_id))
				data[i][j][k] = -data[i][j][k];
				else if (tmp_cell_id != _cell_id)
				data[i][j][k] = -data[i][j][k];*/
				if (!OMath::pointIsInsideCube(curr_p, FVector::ZeroVector, _vd_dims))
					data[i][j][k] = -data[i][j][k];
				//else if (OMath::closestPoint(curr_p, _v_particles) != _cell_id)
				else if (_vcon->find_voronoi_cell(curr_p.X, curr_p.Y, curr_p.Z, rx, ry, rz, tmp_cell_id))
				{
					if(tmp_cell_id != _cell_id)
						data[i][j][k] = -data[i][j][k];
				}
					

				
				//UE_LOG(LogTemp, Warning, TEXT("aweawe %d"), tmp_cell_id);

			}
		}
	}
}

void LevelSet::voronoiDiagramSignedDist(std::vector<voro::voronoicell_neighbor> *_v_cells, const std::vector<FVector> &_v_particles, FVector &_vd_dims, const  FVector& _w_pos)
{
	sf.allocateSectionData();
	sf.setNrOfSections(_v_cells->size());

	unsigned char*** section = sf.getSectionPtr();
	float*** data = sf.getDataPtr();

	float x, y, z;

	//resolution of non padding data
	FVector nb_res = *res - FVector(2.f, 2.f, 2.f);

	float ox = (dims->X / nb_res.X);
	float oy = (dims->Y / nb_res.Y);
	float oz = (dims->Z / nb_res.Z);
	*dims = FVector(ox, oy, oz) + *dims;
	ox = ox*0.5f; oy = oy*0.5f; oz = oz*0.5f;
	float resxm1 = (res->X - 1);
	float resym1 = (res->Y - 1);
	float reszm1 = (res->Z - 1);
	float resxm1d2 = (resxm1 / 2.f);
	float resym1d2 = (resym1 / 2.f);
	float reszm1d2 = (reszm1 / 2.f);


	//create padding data
	for (int i = 0; i < res->X; i++){
		for (int j = 0; j < res->Y; j++){
			data[i][j][0] = oz;
			data[i][j][(int)res->Z - 1] = oz;
		}
	}
	for (int i = 0; i < res->X; i++) {
		for (int j = 0; j < res->Z; j++) {
			data[i][0][j] = oy;
			data[i][(int)res->Y - 1][j] = oy;
		}
	}
	for (int i = 0; i < res->Y; i++) {
		for (int j = 0; j < res->Z; j++) {
			data[0][i][j] = ox;
			data[(int)res->X - 1][i][j] = ox;
		}
	}

	//Verts
	std::vector<std::vector<double>> cell_verts(_v_cells->size());
	//edges
	std::vector<std::vector<int>> cell_edges(_v_cells->size());
	//faces
	std::vector<std::vector<int>> cell_faces(_v_cells->size());
	std::vector<std::vector<int>> cell_face_orders(_v_cells->size());
	std::vector<std::vector<double>> cell_face_normals(_v_cells->size());


	for (int i = 0; i < _v_cells->size(); i++)
	{
		(*_v_cells)[i].vertices(_v_particles[i].X, _v_particles[i].Y, _v_particles[i].Z, cell_verts[i]);
		(*_v_cells)[i].edges(cell_edges[i]);
		(*_v_cells)[i].face_vertices(cell_faces[i]);
		(*_v_cells)[i].face_orders(cell_face_orders[i]);
		(*_v_cells)[i].normals(cell_face_normals[i]);
	}



	w_pos = _w_pos;
	//int tmp_cell_id;
	FVector curr_p;
	int cell_id;
	float *lv = sf.getLargestValuePtr();
	*lv = 0.f;

	for (int i = 1; i < res->X -1; i++)
	{
		for (int j = 1; j < res->Y - 1; j++)
		{
			for (int k = 1; k < res->Z - 1; k++)
			{
				x = ((i - resxm1d2) / resxm1)*dims->X;// +ox;// +w_pos.X;
				y = ((j - resym1d2) / resym1)*dims->Y;// +oy;// + w_pos.Y;
				z = ((k - reszm1d2) / reszm1)*dims->Z;// + oz;// +w_pos.Z;

				curr_p = FVector(x, y, z);
				cell_id = OMath::closestPoint(curr_p, _v_particles);
				section[i][j][k] = cell_id;

				data[i][j][k] = OMath::distToVoronoiCell(curr_p, cell_verts[cell_id], cell_edges[cell_id], cell_faces[cell_id], cell_face_orders[cell_id], cell_face_normals[cell_id]);
				if (FMath::Abs(data[i][j][k]) > *lv)
					*lv = data[i][j][k];
				//UE_LOG(LogTemp, Warning, TEXT("aweawe %d"), tmp_cell_id);

			}
		}
	}
}

void LevelSet::meshToLeveSet(FRawMesh* _rm, FVector& _w_pos)
{
	/*FVector dim = sf.getDims();
	FVector res = sf.getRes();*/
	float*** data = sf.getDataPtr();

	w_pos = _w_pos;
	float x, y, z;
	float ox = (dims->X / res->X)*0.5f;
	float oy = (dims->Y / res->Y)*0.5f;
	float oz = (dims->Z / res->Z)*0.5f;
	float resxm1d2 = ((res->X - 1) / 2.f);
	float resym1d2 = ((res->Y - 1) / 2.f);
	float reszm1d2 = ((res->Z - 1) / 2.f);
	float resxm1 = (res->X - 1);
	float resym1 = (res->Y - 1);
	float reszm1 = (res->Z - 1);

	sf.setLargestValue(FMath::Max3(dims->X, dims->Y, dims->Z )*0.25f );

	/*scalar_value max = -std::numeric_limits<scalar_value>::infinity();
	scalar_value min = -max;*/

	for (int i = 0; i < res->X; i++)
	{
		for (int j = 0; j < res->Y; j++)
		{
			for (int k = 0; k < res->Z; k++) {
				//if (i == 0 || j == 0 || k == 0 || i == res.X - 1 || j == res.Y - 1 || k == res.Z - 1) 
				//{
				//	//data[i][j][k] = -1000.0f;
				//}
				//else
				//{
				x = ((i - resxm1d2) / resxm1)*dims->X + w_pos.X;
				y = ((j - resym1d2) / resym1)*dims->Y + w_pos.Y;
				z = ((k - reszm1d2) / reszm1)*dims->Z + w_pos.Z;
				//z = ((k ) / res.Z)*dim.Z + (dim.Z / res.Z)*0.5f;
				data[i][j][k] = -distanceToMesh(_rm, FVector(x, y, z));


				/*if (data[i][j][k] > max)
				{
				max = data[i][j][k];
				}
				else if (data[i][j][k] < min)
				{
				min = data[i][j][k];
				}*/

				/*if(data[i][j][k] > 0)
				{
				data[i][j][k] = 0.0f;
				}
				else
				{
				data[i][j][k] = 255.0f;
				}*/
				//}
			}
		}
	}
}

//_rel_transform is the transform that puts the origin of sf2 at a certain point relative to the origin of sf1
void LevelSet::mergeLevelSets(LevelSet* _ls1, LevelSet* _ls2, FMatrix _rel_rotation, FVector rel_position, FVector frag_w_pos)
{

	FVector res1;
	_ls1->getRes(res1);
	FVector dim1;
	_ls1->getDims(dim1);
	FVector pos1;
	_ls1->getPos(pos1);
	float*** data1 = _ls1->sf.getDataPtr();


	FVector xyz1;
	//int i1, j1, k1;
	float resxm1d2_1 = ((res1.X - 1) / 2.f);
	float resym1d2_1 = ((res1.Y - 1) / 2.f);
	float reszm1d2_1 = ((res1.Z - 1) / 2.f);
	float resxm1_1 = (res1.X - 1);
	float resym1_1 = (res1.Y - 1);
	float reszm1_1 = (res1.Z - 1);
	float interp_val_1;

	FVector res2;
	_ls2->getRes(res2);
	FVector dim2;
	_ls2->getDims(dim2);
	FVector pos2;
	_ls2->getPos(pos2);
	float*** data2 = _ls2->sf.getDataPtr();

	FVector xyz2;
	float resxm1d2_2 = ((res2.X - 1) / 2.f);
	float resym1d2_2 = ((res2.Y - 1) / 2.f);
	float reszm1d2_2 = ((res2.Z - 1) / 2.f);
	float resxm1_2 = (res2.X - 1);
	float resym1_2 = (res2.Y - 1);
	float reszm1_2 = (res2.Z - 1);


	float x, y, z;
	for (int i2 = 0; i2 < res2.X; i2++)
	{
		for (int j2 = 0; j2 < res2.Y; j2++)
		{
			for (int k2 = 0; k2 < res2.Z; k2++)
			{
				//inside fragment
				if (data2[i2][j2][k2] >= _ls2->iso_val)
				{
					x = ((i2 - resxm1d2_2) / resxm1_2)*dim2.X;// +pos2.X;
					y = ((j2 - resym1d2_2) / resym1_2)*dim2.Y;// + pos2.Y;
					z = ((k2 - reszm1d2_2) / reszm1_2)*dim2.Z;// + pos2.Z;



					xyz2 = FVector(x, y, z) + frag_w_pos;

					xyz1 = _rel_rotation.TransformPosition(xyz2) + rel_position;

					interp_val_1 = _ls1->getTLIValue(xyz1);
					//inside original model
					if (interp_val_1 >= _ls1->iso_val)
					{
						data2[i2][j2][k2] = std::fmin(interp_val_1, data2[i2][j2][k2]);
					}
					else
					{
						data2[i2][j2][k2] = interp_val_1;
					}

				}
			}
		}
	}
}

float LevelSet::getTLIValue(FVector _pos)
{
	float*** data = sf.getDataPtr();

	FVector data_pos = transform2BoundedDataPos(_pos);
	//integer positions
	int x0 = FMath::Floor(data_pos.X);
	int x1 = FMath::Ceil(data_pos.X);
	int y0 = FMath::Floor(data_pos.Y);
	int y1 = FMath::Ceil(data_pos.Y);
	int z0 = FMath::Floor(data_pos.Z);
	int z1 = FMath::Ceil(data_pos.Z);
	//differentials
	float xd = data_pos.X - x0;
	float yd = data_pos.Y - y0;
	float zd = data_pos.Z - z0;
	//interpolate along x
	float v00 = data[x0][y0][z0] * (1 - xd) + data[x1][y0][z0] * xd;
	float v01 = data[x0][y0][z1] * (1 - xd) + data[x1][y0][z1] * xd;
	float v10 = data[x0][y1][z0] * (1 - xd) + data[x1][y1][z0] * xd;
	float v11 = data[x0][y1][z1] * (1 - xd) + data[x1][y1][z1] * xd;
	//interpolate along y
	float v0 = v00*(1 - yd) + v10*yd;
	float v1 = v01*(1 - yd) + v11*yd;
	//interpolate along z
	float v = v0*(1 - zd) + v1*zd;

	return v;
}

float LevelSet::distanceToMesh(FRawMesh* _rm, FVector _p)
{
	FVector closest_point_on_tri;
	FVector closest_point_on_mesh;

	float tmp_dist;
	float min_dist;

	int closest_tri_index;

	closest_point_on_tri = FMath::ClosestPointOnTriangleToPoint(_p,
		_rm->VertexPositions[_rm->WedgeIndices[0]],
		_rm->VertexPositions[_rm->WedgeIndices[0 + 1]],
		_rm->VertexPositions[_rm->WedgeIndices[0 + 2]]);

	min_dist = FVector::DistSquared(closest_point_on_tri, _p);
	closest_point_on_mesh = closest_point_on_tri;
	closest_tri_index = 0;


	for (int i = 3; i < _rm->WedgeIndices.Num(); i = i + 3)
	{
		closest_point_on_tri = FMath::ClosestPointOnTriangleToPoint(_p,
			_rm->VertexPositions[_rm->WedgeIndices[i]],
			_rm->VertexPositions[_rm->WedgeIndices[i + 1]],
			_rm->VertexPositions[_rm->WedgeIndices[i + 2]]);

		
		tmp_dist = FVector::DistSquared(closest_point_on_tri, _p);
		if (tmp_dist < min_dist)
		{
			//if (tmp_dist == 0.f)
			//	tmp_dist = tmp_dist;

			min_dist = tmp_dist;
			closest_point_on_mesh = closest_point_on_tri;
			closest_tri_index = i;
		}
	}


	FVector closest_vector = closest_point_on_mesh - _p;
	FVector e1 = _rm->VertexPositions[_rm->WedgeIndices[closest_tri_index + 1]] - _rm->VertexPositions[_rm->WedgeIndices[closest_tri_index]];
	FVector e2 = _rm->VertexPositions[_rm->WedgeIndices[closest_tri_index + 2]] - _rm->VertexPositions[_rm->WedgeIndices[closest_tri_index]];
	closest_vector.Normalize();
	if (FVector::DotProduct(closest_vector, FVector::CrossProduct(e1, e2)) < 0)
	{
		return -min_dist;
	}
	else
	{
		return min_dist;
	}

}

// Transforms the positition to the integer space of  the data 3D array. Note that the returned 
// vector still contains floats and these are not rounded to integer values.
// This particular function will also bound the values, meaning that if the provided position
// is outside of the data integer space it will set the transformed vector to the closest point 
// within the data space.
FVector LevelSet::transform2BoundedDataPos(FVector& _pos)
{
	//X
	float x = ((_pos.X - w_pos.X) / dims->X)*(res->X - 1) + (res->X - 1) / 2.0f;
	if (x < 0)
		x = 0;
	else if (x >(res->X - 1))
		x = (res->X - 1);
	//Y
	float y = ((_pos.Y - w_pos.Y) / dims->Y)*(res->Y - 1) + (res->Y - 1) / 2.0f;
	if (y < 0)
		y = 0;
	else if (y >(res->Y - 1))
		y = (res->Y - 1);
	//Z
	float z = ((_pos.Z - w_pos.Z) / dims->Z)*(res->Z - 1) + (res->Z - 1) / 2.0f;
	if (z < 0)
		z = 0;
	else if (z >(res->Z - 1))
		z = (res->Z - 1);

	return FVector(x, y, z);
}

void LevelSet::reallocateData() {
	sf.reAllocateData();
}

void LevelSet::drawBounds(UWorld * _world) {
	sf.drawBounds(_world, w_pos);
}

void LevelSet::drawScalars(UWorld * _world) {
	sf.drawScalars(_world, w_pos);
}

void LevelSet::drawScalarSections(UWorld * _world) {
	sf.drawScalarSections(_world, w_pos);
}