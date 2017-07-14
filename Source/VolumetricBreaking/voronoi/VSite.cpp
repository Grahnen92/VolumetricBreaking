// Fill out your copyright notice in the Description page of Project Settings.

#include "VSite.h"
#include "VolumetricBreaking.h"


VSite::VSite()
{
}

VSite::VSite(FVector2D& _coords)
{
	pos = _coords;
}


VSite::~VSite()
{
}


bool VSite::isNeighbour(VSite* _pn)
{
	for (const auto &neighbour : neighbours)
		if (neighbour == _pn)
			return true;

	return false;
}

void  VSite::addNeighbour(VSite* _pn)
{
	if (!isNeighbour(_pn))
		neighbours.push_back(_pn);
}