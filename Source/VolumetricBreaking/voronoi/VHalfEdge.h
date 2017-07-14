// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
/**
 * 
 */
class VSite;

class VOLUMETRICBREAKING_API VHalfEdge
{
public:
	VHalfEdge();
	VHalfEdge(FVector2D _tmp_start);
	VHalfEdge(FVector2D _tmp_start, VSite* _left, VSite* _right);
	VHalfEdge(VSite* _left, VSite* _right);
	VHalfEdge(FVector2D _tmp_start, VHalfEdge* _twin);
	~VHalfEdge();

	void setStartOrEnd(FVector2D _p);

	FVector2D tmp_start;

	bool start_found = false;
	FVector2D start;
	FVector2D end;

	VSite* left;
	VSite* right;

	FVector2D direction;

	VHalfEdge* twin;

};
