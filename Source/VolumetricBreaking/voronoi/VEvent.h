// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VParabola.h"
#include "VSite.h"

/**
 * 
 */
class VOLUMETRICBREAKING_API VEvent
{
public:
	VEvent();
	VEvent(VSite* _s);
	VEvent(FVector2D _ce_point, VParabola* _par);
	~VEvent();

	bool operator <(const VEvent& rhs)
	{
		return pos.Y < rhs.pos.Y;
	}

	bool operator >(const VEvent& rhs)
	{
		return rhs.pos.Y < pos.Y;
	}
	
	FVector2D pos;

	bool site_event;
	//only used if this is a site event
	VSite* s;

	//only used if this is a circle event
	VParabola* par;

	struct CompareEvent : public std::binary_function<VEvent*, VEvent*, bool>
	{
		bool operator()(const VEvent* l, const VEvent* r) const { return (l->pos.Y < r->pos.Y); }
	};
	
};
