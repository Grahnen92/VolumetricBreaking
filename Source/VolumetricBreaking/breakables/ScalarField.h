// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#ifndef SCALARFIELD_H
#define SCALARFIELD_H

#include "VolumetricBreaking.h"
#include <memory>
#include "math.h"
#include <vector>
#include "ProceduralMeshComponent.h"

#include "voronoi/Voronoi.h"
#include "C:/Program Files (x86)/Epic Games/projects/VolumetricBreaking/ThirdParty/voro++/includes/voro++.hh"

#include "utilities/OMath.h"
#include "RawMesh.h"
#include <limits>
#include <iostream>

#include "DrawDebugHelpers.h"

/**
 * 
 */


template <typename scalar_value>
class VOLUMETRICBREAKING_API ScalarField
{

public:
	ScalarField()
	{
		res = FVector(0.f, 0.f, 0.f);
		dim = FVector(0.f, 0.f, 0.f);
	}
	ScalarField(int _cubed_res)
	{
		res = FVector(_cubed_res, _cubed_res, _cubed_res);
		dim = FVector(1.f, 1.f, 1.f);
		allocateData();
	}

	ScalarField(int _cubed_res, float _cubed_dim)
	{
		res = FVector(_cubed_res, _cubed_res, _cubed_res);
		dim = FVector(_cubed_dim, _cubed_dim, _cubed_dim);
		allocateData();
	}
	ScalarField(FVector _res, float _cubed_dim)
	{
		res = _res;
		dim = FVector(_cubed_dim, _cubed_dim, _cubed_dim);
		allocateData();
	}
	ScalarField(int _cubed_res, FVector _dim)
	{
		res = FVector(_cubed_res, _cubed_res, _cubed_res);
		dim = _dim;
		allocateData();
	}
	ScalarField(FVector _res, FVector _dim)
	{
		res = _res;
		dim = _dim;
		allocateData();
	}

	~ScalarField()
	{
		deAllocateData();
	}

	FVector getRes()
	{
		return res;
	}
	FVector* getResPtr()
	{
		return &res;
	}
	void setRes(FVector _res)
	{
		res = _res;
		allocateData();
	}
	FVector getDims()
	{
		return dim;
	}
	FVector* getDimsPtr()
	{
		return &dim;
	}
	void setDims(const FVector &_dims)
	{
		dim = _dims;
		//allocateData();
	}
	void setCubedDims( int cubed_dim)
	{
		dim = _dims;
		//allocateData();
	}
	scalar_value getIsoValue()
	{
		return iso_value;
	}
	void setIsoValue(scalar_value _iv)
	{
		iso_value = _iv;
	}

	void setLargestValue(scalar_value _v)
	{
		largest_value = _v;
	}

	scalar_value getLargestValue()
	{
		return largest_value;
	}

	scalar_value* getLargestValuePtr()
	{
		return &largest_value;
	}

	scalar_value*** getDataPtr()
	{
		return data;
	}
	scalar_value** operator[](int i)
	{
		return data[i];
	}
	scalar_value getValue(int _x, int _y, int _z)
	{
		return data[_x][_y][_z];
	}

	unsigned char*** getSectionPtr()
	{
		return section;
	}

	void setNrOfSections(int _nr_of_section)
	{
		nr_of_sections = _nr_of_section;
	}

	unsigned char getNrOfSections()
	{
		return nr_of_sections;
	}

	void setAllValues(scalar_value _v)
	{
		for (int i = 0; i < res.X; i++)
		{
			for (int j = 0; j < res.Y; j++)
			{
				for (int k = 0; k < res.Z; k++) {
					if (i == 0 || j == 0 || k == 0 || i == res.X - 1 || j == res.Y - 1 || k == res.Z - 1) {
						data[i][j][k] = 0.0f;
					}
					else {
						data[i][j][k] = 255.0f;
					}
				}
			}
		}
	}

	void setHalfOfAllValues(scalar_value _v)
	{
		for (int i = 0; i < res.X; i++)
		{
			for (int j = 0; j < res.Y; j++)
			{
				for (int k = 0; k < res.Z; k++) {
					if (i == 0 || j == 0 || k == 0 || i == res.X - 1 || j == res.Y - 1 || k == res.Z - 1) {
						data[i][j][k] = 0.0f;
					}
					else if (k > res.Z / 2) {
						data[i][j][k] = 255.0f;
					}
					else
					{
						data[i][j][k] = 0.0f;
					}
				}
			}
		}
	}

	void setAsCosFun(scalar_value _v)
	{
		for (int i = 0; i < res.X; i++)
		{
			for (int j = 0; j < res.Y; j++)
			{
				for (int k = 0; k < res.Z; k++) {
					if (i == 0 || j == 0 || k == 0 || i == res.X - 1 || j == res.Y - 1 || k == res.Z - 1) {
						data[i][j][k] = 0.0f;
					}
					else if (k > std::cos(i)*res.X / 2) {
						data[i][j][k] = 255.0f;
					}
					else
					{
						data[i][j][k] = 0.0f;
					}
				}
			}
		}
	}

	void setAsWedge(scalar_value _v)
	{
		for (int i = 0; i < res.X; i++)
		{
			for (int j = 0; j < res.Y; j++)
			{
				for (int k = 0; k < res.Z; k++) {
					x = ((i - (res.X / 2)) / res.X)*dim.X + (dim.X / res.X)*0.5f + _mid_point.X;
					y = ((j - (res.Y / 2)) / res.Y)*dim.Y + (dim.Y / res.Y)*0.5f + _mid_point.Y;
					z = ((k - (res.Z / 2)) / res.Z)*dim.Z + (dim.Z / res.Z)*0.5f + _mid_point.Z;
				}
			}
		}
	}
	

	void drawBounds(UWorld * _world, const FVector& _offset = FVector(0.f, 0.f, 0.f)) {

		FVector v1 = FVector(dim.X, dim.Y, dim.Z)*0.5f + _offset;
		FVector v2 = FVector(-dim.X, dim.Y, dim.Z)*0.5f + _offset;
		FVector v3 = FVector(-dim.X, -dim.Y, dim.Z)*0.5f + _offset;
		FVector v4 = FVector(dim.X, -dim.Y, dim.Z)*0.5f + _offset;

		FVector v5 = FVector(dim.X, dim.Y, -dim.Z)*0.5f + _offset;
		FVector v6 = FVector(-dim.X, dim.Y, -dim.Z)*0.5f + _offset;
		FVector v7 = FVector(-dim.X, -dim.Y, -dim.Z)*0.5f + _offset;
		FVector v8 = FVector(dim.X, -dim.Y, -dim.Z)*0.5f + _offset;

		DrawDebugLine(_world, v1, v2, FColor(255, 255, 255), true,0.0,10,1.f);
		DrawDebugLine(_world, v2, v3, FColor(255, 255, 255), true, 0.0, 10, 1.f);
		DrawDebugLine(_world, v3, v4, FColor(255, 255, 255), true, 0.0, 10, 1.f);
		DrawDebugLine(_world, v4, v1, FColor(255, 255, 255), true, 0.0, 10, 1.f);

		DrawDebugLine(_world, v1, v5, FColor(255, 255, 255), true, 0.0, 10, 1.f);
		DrawDebugLine(_world, v2, v6, FColor(255, 255, 255), true, 0.0, 10, 1.f);
		DrawDebugLine(_world, v3, v7, FColor(255, 255, 255), true, 0.0, 10, 1.f);
		DrawDebugLine(_world, v4, v8, FColor(255, 255, 255), true, 0.0, 10, 1.f);

		DrawDebugLine(_world, v5, v6, FColor(255, 255, 255), true, 0.0, 10, 1.f);
		DrawDebugLine(_world, v6, v7, FColor(255, 255, 255), true, 0.0, 10, 1.f);
		DrawDebugLine(_world, v7, v8, FColor(255, 255, 255), true, 0.0, 10, 1.f);
		DrawDebugLine(_world, v8, v5, FColor(255, 255, 255), true, 0.0, 10, 1.f);

	}

	void drawScalars(UWorld * _world, const FVector& _offset = FVector(0.f, 0.f, 0.f)) {
		float x, y, z;
		float ox = (dim.X / res.X)*0.5f;
		float oy = (dim.Y / res.Y)*0.5f;
		float oz = (dim.Z / res.Z)*0.5f;
		float resxm1d2 = ((res.X - 1) / 2.f);
		float resym1d2 = ((res.Y - 1) / 2.f);
		float reszm1d2 = ((res.Z - 1) / 2.f);
		float resxm1 = (res.X - 1);
		float resym1 = (res.Y - 1);
		float reszm1 = (res.Z - 1);
		for (int i = 0; i < res.X; i++)
		{
			for (int j = 0; j < res.Y; j++)
			{
				for (int k = 0; k < res.Z; k++) {
					x = ((i - resxm1d2) / resxm1)*dim.X + _offset.X;
					y = ((j - resym1d2) / resym1)*dim.Y + _offset.Y;
					z = ((k - reszm1d2) / reszm1)*dim.Z + _offset.Z;
					FColor col;
					

					float scaled_val = FMath::Abs(data[i][j][k]) / largest_value;
					/*col = FColor(255 * scaled_val, 0, 255 * (1 - scaled_val));*/
					if(data[i][j][k]> 0.f)
						col = FColor(255* scaled_val, 0, 0);
					else
						col = FColor(0, 0, 255*scaled_val);


					DrawDebugPoint(
						_world,
						FVector(x,y,z),
						10,//size
						col,
						true, //persistent (never goes away)
						0.0 //point leaves a trail on moving object
					);
				}
			}
		}

	}

	void drawScalarSections(UWorld * _world, const FVector& _offset = FVector(0.f, 0.f, 0.f)) {
		float x, y, z;
		float ox = (dim.X / res.X)*0.5f;
		float oy = (dim.Y / res.Y)*0.5f;
		float oz = (dim.Z / res.Z)*0.5f;
		float resxm1d2 = ((res.X - 1) / 2.f);
		float resym1d2 = ((res.Y - 1) / 2.f);
		float reszm1d2 = ((res.Z - 1) / 2.f);
		float resxm1 = (res.X - 1);
		float resym1 = (res.Y - 1);
		float reszm1 = (res.Z - 1);
		for (int i = 0; i < res.X; i++)
		{
			for (int j = 0; j < res.Y; j++)
			{
				for (int k = 0; k < res.Z; k++) {
					x = ((i - resxm1d2) / resxm1)*dim.X + _offset.X;
					y = ((j - resym1d2) / resym1)*dim.Y + _offset.Y;
					z = ((k - reszm1d2) / reszm1)*dim.Z + _offset.Z;
					FColor col;


					float scaled_val = FMath::Abs(data[i][j][k]) / largest_value;
					/*col = FColor(255 * scaled_val, 0, 255 * (1 - scaled_val));*/
					if (section[i][j][k] == (char)0)
						col = FColor(255 * scaled_val, 0, 0);
					else if(section[i][j][k] == (char)1)
						col = FColor(0, 0, 255 * scaled_val);
					else if(section[i][j][k] == (char)2)
						col = FColor(0, 255 * scaled_val, 0);


					DrawDebugPoint(
						_world,
						FVector(x, y, z),
						2,//size
						col,
						true, //persistent (never goes away)
						0.0 //point leaves a trail on moving object
					);
				}
			}
		}

	}
	void allocateData() {
		if (data)
		{
			UE_LOG(LogTemp, Warning, TEXT("data already allocated, returning."));
			std::cout << "data already allocated, returning.";
			return;
		}
		//data = scalar_u_ptr_3d<scalar_value>(new scalar_u_ptr_2d<scalar_value>[res.X]);
		data = new scalar_value**[res.X];
		for (int i = 0; i < res.X; i++)
		{
			//data[i] = scalar_u_ptr_2d<scalar_value>(new scalar_u_ptr_1d<scalar_value>[res.Y]);
			data[i] = new scalar_value*[res.Y];
			for (int j = 0; j < res.Y; j++)
			{
				//data[i][j] = scalar_u_ptr_1d<scalar_value>(new scalar_value[res.Z]);
				data[i][j] = new scalar_value[res.Z];
			}
		}
	}

	void deAllocateData() {

		if (!data)
		{
			UE_LOG(LogTemp, Warning, TEXT("data already deallocated, returning."));
			std::cout << "data already deallocated, returning.";
			return;
		}

		for (int i = 0; i < res.X; i++)
		{
			for (int j = 0; j < res.Y; j++)
			{
				delete[] data[i][j];
			}
			delete[] data[i];
		}
		delete[] data;

		data = nullptr;
	}

	void reAllocateData() {
		deAllocateData();
		allocateData();
	}

	void allocateSectionData() {
		if (section)
		{
			UE_LOG(LogTemp, Warning, TEXT("sectiondata already allocated, returning."));
			std::cout << "sectiondata already allocated, returning.";
			return;
		}
		//data = scalar_u_ptr_3d<scalar_value>(new scalar_u_ptr_2d<scalar_value>[res.X]);
		section = new unsigned char**[res.X];
		for (int i = 0; i < res.X; i++)
		{
			//data[i] = scalar_u_ptr_2d<scalar_value>(new scalar_u_ptr_1d<scalar_value>[res.Y]);
			section[i] = new unsigned char*[res.Y];
			for (int j = 0; j < res.Y; j++)
			{
				//data[i][j] = scalar_u_ptr_1d<scalar_value>(new scalar_value[res.Z]);
				section[i][j] = new unsigned char[res.Z];
			}
		}
	}

	void deAllocateSectionData() {

		if (!section)
		{
			UE_LOG(LogTemp, Warning, TEXT("sectiondata already deallocated, returning."));
			std::cout << "sectiondata already deallocated, returning.";
			return;
		}

		for (int i = 0; i < res.X; i++)
		{
			for (int j = 0; j < res.Y; j++)
			{
				delete[] section[i][j];
			}
			delete[] section[i];
		}
		delete[] section;

		data = nullptr;
	}

	void reAllocateSectionData() {
		deAllocateSectionData();
		allocateSectionData();
	}

private:
	//scalar_u_ptr_3d<scalar_value> data;
	scalar_value*** data = nullptr;
	FVector res;
	FVector dim;
	scalar_value iso_value;
	scalar_value largest_value;

	unsigned char*** section = nullptr;
	unsigned char nr_of_sections;

};

//namespace ScalarField
//{
//	void mergeLevelSets(ScalarField* _sf1, ScalarField* _sf2, FVector _offset, ScalarField* _sf_out)
//	{
//
//	}
//}
#endif