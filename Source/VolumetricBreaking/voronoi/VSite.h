// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VHalfEdge.h"
#include <list>
#include <vector>

/**
 * 
 */
class VOLUMETRICBREAKING_API VSite
{
public:
	VSite();
	VSite(FVector2D& _coords);
	~VSite();

	FVector2D pos;

	std::vector<VHalfEdge*> edges;
	std::vector<VSite*> neighbours;

	bool isNeighbour(VSite* _pn);
	void addNeighbour(VSite* _pn);
private:
	
	
};
