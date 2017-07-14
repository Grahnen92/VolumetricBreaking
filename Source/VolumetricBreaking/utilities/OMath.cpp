#include "OMath.h"
#include "VolumetricBreaking.h"

namespace OMath {

	void findMaxMinExtent(TArray<FVector>& _vertexArray, FVector& _min, FVector& _max)
	{
		_min = _vertexArray[0];
		_max = _vertexArray[0];

		for (int i = 1; i < _vertexArray.Num(); i++) {
			//X
			if (_vertexArray[i].X > _max.X)
				_max.X = _vertexArray[i].X;
			else if (_vertexArray[i].X < _min.X)
				_min.X = _vertexArray[i].X;
			//Y
			if (_vertexArray[i].Y >_max.Y)
				_max.Y = _vertexArray[i].Y;
			else if (_vertexArray[i].Y < _min.Y)
				_min.Y = _vertexArray[i].Y;
			//Z
			if (_vertexArray[i].Z >_max.Z)
				_max.Z = _vertexArray[i].Z;
			else if (_vertexArray[i].Z < _min.Z)
				_min.Z = _vertexArray[i].Z;
			
		}
	}

	void findMaxMinExtent(std::vector<double>& _vertexArray, FVector& _min, FVector& _max)
	{
		_min.X = _vertexArray[0]; _min.Y = _vertexArray[1]; _min.Z = _vertexArray[2];
		_max.X = _vertexArray[0]; _max.Y = _vertexArray[1]; _max.Z = _vertexArray[2];

		for (int i = 3; i < _vertexArray.size(); i = i +3) {
			//X
			if (_vertexArray[i] > _max.X)
				_max.X = _vertexArray[i];
			else if (_vertexArray[i] < _min.X)
				_min.X = _vertexArray[i];
			//Y
			if (_vertexArray[i + 1] >_max.Y)
				_max.Y = _vertexArray[i + 1];
			else if (_vertexArray[i + 1] < _min.Y)
				_min.Y = _vertexArray[i + 1];
			//Z
			if (_vertexArray[i + 2] >_max.Z)
				_max.Z = _vertexArray[i + 2];
			else if (_vertexArray[i + 2] < _min.Z)
				_min.Z = _vertexArray[i + 2];
		}
	}


	FVector distToPolygon(const FVector &_point, const std::vector<int> &_indices, const std::vector<std::vector<double>> &vertices, std::vector<double> &normal)
	{
		FVector p0 = FVector(vertices[_indices[0]][0], vertices[_indices[0]][1], vertices[_indices[0]][2]);
		FVector n = -FVector(normal[0], normal[1], normal[2]);
		float d = FVector::DotProduct((p0 - _point), n) / n.SizeSquared();

		FVector intersection = _point + n*d;

		FVector closest_edge;


		return FVector::ZeroVector;
	}

	double distanceToLine(FVector _point, FVector lp1, FVector lp2)
	{
		FVector ldir = (lp2 - lp1);
		FVector p = _point - lp1;

		float t = FVector::DotProduct(p, ldir) / ldir.SizeSquared();
		if (t < 0.f)
			return (lp1 - _point).Size();
		else if (t > 0.f)
			return (lp2 - _point).Size();
		else
			return (p - ldir*t).Size();

	}
													//vector with 3*verts doubles		    //vector with 2*edges vert indicies  //vector with vert indicies for each face, each face is separated by the nr of verts of that face
	double distToVoronoiCell(const FVector &_point, const std::vector<double> &_cell_verts, const std::vector<int> &_cell_edges, const std::vector<int> &_cell_faces, const std::vector<int> &_cell_face_orders, const std::vector<double> &_cell_face_normals)
	{
		//double x, y, z;

		//find closest vertex
		
		float min_v_dist;
		FVector curr_vert_vec;

		int nr_of_verts = _cell_verts.size() / 3;
		curr_vert_vec.X = _cell_verts[0] - _point.X;
		curr_vert_vec.Y = _cell_verts[1] - _point.Y;
		curr_vert_vec.Z = _cell_verts[2] - _point.Z;
		min_v_dist = curr_vert_vec.Size();
		int c_v = 0;

		for (int i = 1; i < nr_of_verts; i++) {
			curr_vert_vec.X = _cell_verts[i*3] - _point.X;
			curr_vert_vec.Y = _cell_verts[i*3 +1] - _point.Y;
			curr_vert_vec.Z = _cell_verts[i*3 +2] - _point.Z;
			
			if (curr_vert_vec.Size() < min_v_dist){
				min_v_dist = curr_vert_vec.Size();
				c_v = i;
			}
		}

		//find closest edge of closest vertex --------------------------------------------------
	
		//find edges that include closest vert
		std::vector<int> re;
		for (int i = 0; i < _cell_edges.size(); i = i + 2)
			if (_cell_edges[i] == c_v || _cell_edges[i + 1] == c_v)
				re.push_back(i);
		
		FVector tmp_v1 = FVector(_cell_verts[_cell_edges[re[0]]], _cell_verts[_cell_edges[re[0]] + 1], _cell_verts[_cell_edges[re[0]] + 2]);
		FVector tmp_v2 = FVector(_cell_verts[_cell_edges[re[0]+1]], _cell_verts[_cell_edges[re[0]+1] + 1], _cell_verts[_cell_edges[re[0]+1] + 2]);
		//float min_e_dist = distanceToLine(_point, tmp_v1, tmp_v2);
		float min_e_dist = (FMath::ClosestPointOnSegment(_point, tmp_v1, tmp_v2) - _point).Size();
		int c_e = re[0];

		float curr_e_dist;
		for (int i = 1; i < re.size(); i = i +2){
			int vi1 = _cell_edges[re[i]];
			int vi2 = _cell_edges[re[i] + 1];
			FVector v1 = FVector(_cell_verts[vi1], _cell_verts[vi1 + 1], _cell_verts[vi1 + 2]);
			FVector v2 = FVector(_cell_verts[vi2], _cell_verts[vi2 + 1], _cell_verts[vi2 + 2]);

			//curr_e_dist = distanceToLine(_point, v1, v2);
			curr_e_dist = (FMath::ClosestPointOnSegment(_point, v1, v2) - _point).Size();
			if (curr_e_dist < min_e_dist){
				min_e_dist = curr_e_dist;
				c_e = re[i];
			}
		}

		//find the faces that closest edge belongs to

		//bool contains_p1 = false;
		//bool contains_p2 = false;
		std::vector<int> c_f;
		std::vector<int> c_fi;
		int face_it = 0;
		int face_order;

		int fcounter = 0;
		while (face_it < _cell_faces.size())
		{
			bool contains_p1 = false;
			bool contains_p2 = false;
			face_order = _cell_faces[face_it];
			for (int i = face_it + 1; i < face_it + face_order + 1; i++)
			{
				if (_cell_faces[i] == _cell_edges[c_e])
					contains_p1 = true;
				else if (_cell_faces[i] == _cell_edges[c_e + 1])
					contains_p2 = true;
			}

			if (contains_p1 && contains_p2)
			{
				c_f.push_back(face_it);
				c_fi.push_back(fcounter);
				if (c_f.size() == 2)
					break;
			}
			face_it = face_it + face_order + 1;
			fcounter++;
		}

		//TODO: implement own version distance to polygon and stop using unreals distnace to triangle
		//find closest triangle of closest faces --------------------------------------------------------------


		int tmp_cvs = _cell_faces[c_f[0] + 1]*3;
		int tmp_cvi = _cell_faces[c_f[0] + 2] * 3;
		int tmp_cvii = _cell_faces[c_f[0] + 3] * 3;
		FVector tmp_c_tp = FMath::ClosestPointOnTriangleToPoint(_point,
			FVector(_cell_verts[tmp_cvs], _cell_verts[tmp_cvs + 1], _cell_verts[tmp_cvs + 2]),
			FVector(_cell_verts[tmp_cvi], _cell_verts[tmp_cvi + 1], _cell_verts[tmp_cvi + 2]),
			FVector(_cell_verts[tmp_cvii], _cell_verts[tmp_cvii + 1], _cell_verts[tmp_cvii + 2])
		);

		float min_tri_dist = (tmp_c_tp - _point).Size();
		int closest_face =c_fi[0];
		float curr_tri_dist;

		//calculate the min_dist
		for (int i = 0; i < c_f.size(); i++)
		{
			face_order = _cell_faces[c_f[i]];
			int cvs = _cell_faces[c_f[i] + 1]*3;

			for (int j = c_f[i] + 1 + 1; j < c_f[i] + face_order; j++)
			{
				int cvi = _cell_faces[j] * 3;
				int cvii = _cell_faces[j + 1] * 3;

				FVector c_tp = FMath::ClosestPointOnTriangleToPoint(_point,
					FVector(_cell_verts[cvs], _cell_verts[cvs + 1], _cell_verts[cvs + 2]),
					FVector(_cell_verts[cvi], _cell_verts[cvi + 1], _cell_verts[cvi + 2]),
					FVector(_cell_verts[cvii], _cell_verts[cvii + 1], _cell_verts[cvii + 2])
				);

				curr_tri_dist = (c_tp - _point).Size();
				if (curr_tri_dist < min_tri_dist)
				{
					min_tri_dist = curr_tri_dist;
					closest_face = c_fi[i];
				}
			}
		}

		
		return FMath::Min3(min_v_dist, min_e_dist, min_tri_dist);
	}

	int closestPoint(FVector _point, const std::vector<FVector> &_points)
	{
		float tmp_dist;
		float closest_dist = (_points[0] - _point).Size();
		int closest = 0;

		for (int i = 1; i < _points.size(); i++)
		{
			tmp_dist = (_points[i] - _point).Size();
			if (tmp_dist < closest_dist)
			{
				closest_dist = tmp_dist;
				closest = i;
			}
				
		}
		return closest;
	}

	bool pointIsInsideCube(const FVector &_point, const FVector &_cube_origin, const FVector &_cube_dims)
	{
		FVector p = _point - _cube_origin;
			return p.X >= -_cube_dims.X && p.X <= _cube_dims.X &&
				   p.Y >= -_cube_dims.Y && p.Y <= _cube_dims.Y &&
				   p.Z >= -_cube_dims.Z && p.Z <= _cube_dims.Z;
	}
}