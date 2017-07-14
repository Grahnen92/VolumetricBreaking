#pragma once

#ifndef OMATH_H
#define OMATH_H


//#include "voro++/src/voro++.hh"
//#include "voro++/src/voro++.cc"

#include <vector>


struct FVector;
#include "C:/Program Files (x86)/Epic Games/projects/VolumetricBreaking/ThirdParty/voro++/includes/voro++.hh"
//class voronoicell_neighbor;

namespace OMath {
	
	void findMaxMinExtent(TArray<FVector>& _vertexArray, FVector& _min, FVector& _max);
	void findMaxMinExtent(std::vector<double>& _vertexArray, FVector& _min, FVector& _max);

	FVector distToPolygon(const FVector &_point, const std::vector<int> &_indices, const std::vector<std::vector<double>> &vertices, std::vector<double> &normal);

	double distanceToLine(FVector _point, FVector lp1, FVector lp2);

	double distToVoronoiCell(const FVector &_point, const std::vector<double> &_cell_verts, const std::vector<int> &_cell_edges, const std::vector<int> &_cell_faces, const std::vector<int> &_cell_face_orders, const std::vector<double> &_cell_face_normals);

	int closestPoint(FVector _point, const std::vector<FVector> &_points);

	bool pointIsInsideCube(const FVector &_point, const FVector &_cube_origin, const FVector &_cube_dims);
}
#endif