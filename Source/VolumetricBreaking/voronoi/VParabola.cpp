// Fill out your copyright notice in the Description page of Project Settings.

#include "VParabola.h"
#include "VolumetricBreaking.h"

#include <iostream>
//VParabola::VParabola()
//{
//	//s = nullptr;
//	//is_leaf = false;
//	//e = nullptr;
//	
//}

VParabola::VParabola(VSite* _s, VParabola* _parent)
{
	s = _s;
	is_leaf = true;
	parent = _parent;
	e = nullptr;
	c_e = nullptr;
}

VParabola::VParabola(VHalfEdge* _e, VParabola* _parent)
{
	s = nullptr;
	is_leaf = false;
	parent = _parent;
	e = _e;
	c_e = nullptr;
}

VParabola::VParabola(VSite* _s, VHalfEdge* _e, VParabola* _parent)
{
	s = _s;
	is_leaf = false;
	parent = _parent;
	e = _e;
	c_e = nullptr;
}

VParabola::~VParabola()
{
}

void  VParabola::setLeft(VParabola*  _left)
{
	left_child = _left; _left->parent = this;
}
void  VParabola::setRight(VParabola*  _right)
{
	right_child = _right; _right->parent = this;
}

float VParabola::getYAt(float x, float directrix_y)
{
	float F = (s->pos.Y - directrix_y)/2.f;
	float xv = s->pos.X;
	float yv = s->pos.Y - F;

	float y = (x*x / (4.f*F)) - (x*xv / (2.f*F)) + xv*xv / (4.f*F) + yv;
	return y;
}

float VParabola::getXOfIntersection(float current_directrix_y)
{
	VParabola* left = getClosestLeftLeave();
	VParabola* right = getClosestRightLeave();
	float cdy = current_directrix_y;

	FVector2D s1 = left->s->pos;
	FVector2D s2 = right->s->pos;

	float f1 = (s1.Y - cdy) / 2.f;
	float f2 = (s2.Y - cdy) / 2.f;

	float a = ((1.f / (4.f*f1)) - (1.f / (4.f*f2)));
	float b = ((s2.X / (2.f*f2)) - (s1.X / (2.f*f1)));
	float c = ((s2.X*s2.X) / (4.f*f2)) + s2.Y - ((s1.X*s1.X) / (4.f*f1)) - s1.Y;

	float b_2a = b / (a*2.f);
	float sqr = FMath::Sqrt(b_2a*b_2a + (c / a));

	float x1 = -b_2a + sqr;
	float x2 = -b_2a - sqr;

	if (s1.Y < s2.Y)
		return FMath::Max(x1, x2);
	else
		return FMath::Min(x1, x2);
}

VParabola* VParabola::getClosestLeftLeave()
{
	//TODO: remove debug code
	if (!left_child)
	{
		std::cout << "trying to acces child of leaf";
		return nullptr;
	}
	VParabola* par_it = left_child;

	while (!par_it->is_leaf)
	{
		//TODO: remove debug code
		if (!par_it->right_child)
		{
			std::cout << "non-leaf doesn't have children, something is wrong";
			return nullptr;
		}
		par_it = par_it->right_child;
	}

	return par_it;
}
VParabola* VParabola::getClosestRightLeave() 
{
	//TODO: remove debug code
	if (!right_child)
	{
		std::cout << "trying to acces child of leaf";
		return nullptr;
	}
	VParabola* par_it = right_child;

	while (!par_it->is_leaf)
	{
		//TODO: remove debug code
		if (!par_it->left_child)
		{
			std::cout << "non-leaf doesn't have children, something is wrong";
			return nullptr;
		}
		par_it = par_it->left_child;
	}

	return par_it;
}

VParabola* VParabola::getClosestLeftParent()
{
	VParabola * par = this->parent;
	VParabola * pLast = this;
	while (par->left_child == pLast)
	{
		if (!par->parent) return nullptr;
		pLast = par;
		par = par->parent;
	}
	return par;
}
VParabola* VParabola::getClosestRightParent()
{
	VParabola * par = this->parent;
	VParabola * pLast = this;
	while (par->right_child == pLast)
	{
		if (!par->parent) return nullptr;
		pLast = par; par = par->parent;
	}
	return par;
}
