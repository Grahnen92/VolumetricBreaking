// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#ifndef MCTRIANGULATOR_H
#define MCTRIANGULATOR_H
#include <vector>
#include "ProceduralMeshComponent.h"
#include "ScalarField.h"
#include <string>
//template <typename scalar_value>
/**
 * 
 */
//TODO make subclasses for each type of triangulation

class VOLUMETRICBREAKING_API MCTriangulator
{
public:

	MCTriangulator();
	~MCTriangulator();

	void marchingCubes(UProceduralMeshComponent* _mesh, ScalarField<float>* _sf, float _iso_val = 0.f, const FVector& _mid_point = FVector(0.0f, 0.0f, 0.0f));
	void sectionedMarchingCubes(UProceduralMeshComponent* _mesh, ScalarField<float>* _sf, float _iso_val = 0.f, const FVector& _mid_point = FVector(0.0f, 0.0f, 0.0f));
	//void marchingCubes(UProceduralMeshComponent* _mesh, LevelSet* _ls, const FVector& _mid_point = FVector(0.0f, 0.0f, 0.0f));
	
private:

	//MC cell iterator data structures =======================
	int cubeIndex;

	float xyz[8][3];
	int vertList[12];
	float val[8];
	bool cellIsoBool[8];
	double dVal;

	unsigned char sections[8];
	std::vector<unsigned char> cell_sections;

	int x, y, z;
	int layerIndex = 0;

	//variables dependant on the scalarfield being triangulated
	float*** data = nullptr;
	unsigned char*** section = nullptr;

	float iso_val;

	FVector dim;

	float resxm1;
	float resxm1d2;
	float resym1;
	float resym1d2;
	float reszm1;
	float reszm1d2;

	/*int edgeTable[256];
	int triTable[256][16];
	void initTables(int* _edgeTable, int** _triTable);*/
	const static int edgeTable[];// [256];
	const static int triTable[][16];// [256][16];

	struct mcCacheCell {
		bool isoBool;
		float cornerPoint[3];
		int vertexIndex[3];
	};

	mcCacheCell*** isoCache;//[2][resy][resz]
	std::vector<int> y0Cache;
	int z0Cache;

	//arrays to store generated data =============================

	std::vector<TArray<FVector>> vertexArrays;
	std::vector<TArray<int32>> triangleArrays;
	std::vector<TArray<FVector>> normalArrays;
	std::vector<TArray<FProcMeshTangent>> tangentArrays;

	std::vector<int> vertexCaps;
	std::vector<int> triangleCaps;

	//functions used during the algorithm
	void calcNormals();
	void calcSectionedNormals();

	void prepSectionedCell(unsigned char _section);
	void calculateSectionedCellPositions();
	void calculateSectionedCell(unsigned char _section);

	void calcSixthCorner();
	void setCubeIndex();
	void bindTriangleIndicies();
	void bindSectionedTriangleIndicies(unsigned char _section);
};
#endif