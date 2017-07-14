// Fill out your copyright notice in the Description page of Project Settings.

#include "VHalfEdge.h"
#include "VolumetricBreaking.h"
#include "VSite.h"

#include <iostream>

VHalfEdge::VHalfEdge()
{
}

VHalfEdge::VHalfEdge(FVector2D _tmp_start)
{
	tmp_start = _tmp_start;
	start = _tmp_start;
}

VHalfEdge::VHalfEdge(FVector2D _tmp_start, VSite* _left, VSite* _right)
{
	tmp_start = _tmp_start;
	start = _tmp_start;
	left = _left;
	right = _right;

	FVector2D perpendicular_dir = (_left->pos - _right->pos).GetSafeNormal();
	direction = FVector2D(-perpendicular_dir.Y, perpendicular_dir.X);

	if (perpendicular_dir.IsNearlyZero())
	{
		std::cout << "left and right site are identical" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("left and right site are identical"));

	}
}

VHalfEdge::VHalfEdge(VSite* _left, VSite* _right)
{
	left = _left;
	right = _right;
	FVector2D perpendicular_dir = (_left->pos - _right->pos).GetSafeNormal();
	direction = FVector2D(-perpendicular_dir.Y, perpendicular_dir.X);

	if (perpendicular_dir.IsNearlyZero())
	{
		std::cout << "left and right site are identical" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("left and right site are identical"));

	}
}
VHalfEdge::VHalfEdge(FVector2D _tmp_start, VHalfEdge* _twin)
{
	tmp_start = _tmp_start;
	start = _tmp_start;
	twin = _twin;
}

VHalfEdge::~VHalfEdge()
{
}

void VHalfEdge::setStartOrEnd(FVector2D _p)
{
	if (start_found) {
		end = _p;
		twin->start = _p;
		twin->start_found = true;
	}
	else if (twin->start_found) {
		start = _p;
		start_found = true;
		twin->end = _p;
	}
	else {
		//
		FVector2D r = right->pos;
		FVector2D l = left->pos;
		//if (FVector2D::DotProduct(_point - start, direction) > 0.f) {
		if ( ((l.X - r.X)*(_p.Y - r.Y) - (l.Y - r.Y)*(_p.X - r.X)) > 0.f) {
			end = _p;
			twin->start = _p;
			twin->start_found = true;
		}
		else
		{
			start = _p;
			start_found = true;
			twin->end = _p;
		}
	}
}

