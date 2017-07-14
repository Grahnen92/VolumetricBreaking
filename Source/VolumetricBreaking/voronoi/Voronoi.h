// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VSite.h"
#include "VHalfEdge.h"
#include "VParabola.h"
#include "VEvent.h"

#include <queue>
#include <set>
#include <list>
#include <vector>

/**
 * 
 */
class VOLUMETRICBREAKING_API Voronoi
{
public:
	Voronoi();
	~Voronoi();

	void CalculateDiagram(TArray<FVector2D>* _sites);

	//all coordinates within the diagram go from 0-x and 0-y
	void setDims(float x, float y) 
	{
		dims.X = x; dims.Y = y;
	}

	FVector getDims()
	{
		return FVector(dims.X, dims.Y, 0.f);
	}

	std::vector<VSite>* getSites()
	{
		return &sites;
	}

private:

	void addParabola(VSite * _s);
	VParabola* findParabolaAtX(float _x);

	void checkCircle(VParabola* _p1);
	bool getEdgeIntersection(VHalfEdge* _he1, VHalfEdge* _he2, FVector2D& _out);

	void handleCircleEvent(VEvent* _circle_event);

	void finishEdges(VSite* _site);
	void withinDims(FVector2D& _point);

	std::vector<VSite> sites;
	std::vector<FVector2D> vertices;
	std::vector<VHalfEdge> edges;

	VParabola* root;

	//queue sorted decreasingly by the Y coordinate of the events
	std::priority_queue<VEvent*, std::vector<VEvent*>, VEvent::CompareEvent > event_queue;
	//directrix moving downwards with the eventorder
	float directrix_y;

	//vector of deleted events
	std::vector<VEvent*> deleted_events;
	FVector2D dims;
};
