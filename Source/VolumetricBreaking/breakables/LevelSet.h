// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#ifndef LEVELSET_H
#define LEVELSET_H
#include "ScalarField.h"

/**
 * 
 */
class VOLUMETRICBREAKING_API LevelSet
{
public:
	LevelSet();
	LevelSet(const FVector &_res, const FVector &_dims, float _iso_val = 0.f);
	LevelSet(int _cubed_res, float _cubed_dim, float _iso_val = 0.f);
	~LevelSet();

	void setRes(int x, int y, int z);
	void setRes(const FVector &_res);
	void getRes(int &x, int &y, int &z);
	void getRes(FVector &_res);
	FVector getRes();

	void setDims(const FVector &_dims);
	void getDims(FVector &_dims);
	FVector getDims();

	void setPos(const FVector &_pos);
	void getPos(FVector &_pos);
	FVector getPos();

	void setIsoVal(float _val);
	float getIsoVal();

	ScalarField<float>* getScalarField();

	void sphereSignedDistance(FVector _world_pos);
	
	void cubeSignedDistance(FVector _world_pos);

	void voronoiCellSignedDist(voro::voronoicell_neighbor* _v_cell, voro::container* _vcon, const int& _cell_id, const std::vector<FVector> &_v_particles, FVector &_vd_dims, const  FVector& _world_pos = FVector(0.f, 0.f, 0.f));

	void voronoiDiagramSignedDist(std::vector<voro::voronoicell_neighbor> *_v_cells, const std::vector<FVector> &_v_particles, FVector &_vd_dims, const  FVector& _world_pos = FVector(0.f, 0.f, 0.f));

	void meshToLeveSet(FRawMesh* _rm, FVector& _world_pos);

	//_rel_transform is the transform that puts the origin of sf2 at a certain point relative to the origin of sf1
	static void mergeLevelSets(LevelSet* _ls1, LevelSet* _ls2, FMatrix _rel_rotation, FVector rel_position, FVector frag_offset);
	static void mergeLevelSets(LevelSet* _ls1, LevelSet* _ls2, FMatrix _rel_rotation, FVector rel_position, FVector frag_offset, LevelSet* _ls_out);
		
	void drawBounds(UWorld * _world);

	void drawScalars(UWorld * _world);

	void drawScalarSections(UWorld * _world);

private:
	ScalarField<float> sf;
	float iso_val;
	FVector w_pos;

	FVector* dims;
	FVector* res;

	void reallocateData();

	float distanceToMesh(FRawMesh* _rm, FVector _p);

	// Transforms the positition to the integer space of  the data 3D array. Note that the returned 
	// vector still contains floats and these are not rounded to integer values.
	// This particular function will also bound the values, meaning that if the provided position
	// is outside of the data integer space it will set the transformed vector to the closest point 
	// within the data space.
	FVector transform2BoundedDataPos(FVector& _pos);

	float getTLIValue(FVector _pos);

};
#endif