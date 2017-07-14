// Fill out your copyright notice in the Description page of Project Settings.

#include "VEvent.h"
#include "VolumetricBreaking.h"


VEvent::VEvent()
{
}

VEvent::VEvent(VSite* _s)
{
	s = _s;
	pos = _s->pos;
	site_event = true;
}

VEvent::VEvent( FVector2D _ce_point, VParabola* _par)
{
	par = _par;
	pos = _ce_point;
	site_event = false;
}

VEvent::~VEvent()
{
}
