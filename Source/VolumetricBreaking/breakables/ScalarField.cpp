// Fill out your copyright notice in the Description page of Project Settings.
#include "ScalarField.h"
#include "VolumetricBreaking.h"


//template <typename scalar_value>
//ScalarField<scalar_value>::ScalarField()
//{
//	dimension = FVector(100.f, 100.f, 100.f);
//
//	data = new std::unique_ptr<std::unique_ptr<scalar_value>>[dimensions.X];
//	for (int i = 0; i < dimensions.X; i++) 
//	{
//		data[i] = new std::unique_ptr<scalar_value>[dimensions.Y];
//		for (int j = 0; j < dimensions.Y; j++)
//		{
//			data[i][j] = new scalar_value[dimensions.Z];
//			for (int k = 0; k < dimensions.Z; k++) {
//				data[i][j][k] = 1.0f;
//			}
//		}
//	}
//}
//
//template <typename scalar_value>
//ScalarField<scalar_value>::~ScalarField()
//{
//}
