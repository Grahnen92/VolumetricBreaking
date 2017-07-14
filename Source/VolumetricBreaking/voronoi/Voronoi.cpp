// Fill out your copyright notice in the Description page of Project Settings.

#include "Voronoi.h"
#include "VolumetricBreaking.h"


#include <algorithm>
#include <iostream>

Voronoi::Voronoi()
{
}

Voronoi::~Voronoi()
{
}

void Voronoi::CalculateDiagram(TArray<FVector2D>* _sites)
{
	sites.reserve(_sites->Num());
	
	for (int i = 0; i < _sites->Num(); i++)
	{
		sites.push_back(VSite((*_sites)[i]));
		event_queue.push(new VEvent(&sites[i]));
	}
	
	root = new VParabola(event_queue.top()->s, nullptr);
	directrix_y = event_queue.top()->pos.Y;
	event_queue.pop();


	while (!event_queue.empty())
	{
		VEvent* e = event_queue.top();
		event_queue.pop();

		directrix_y = e->pos.Y;
		auto deleted_event = std::find(deleted_events.begin(), deleted_events.end(), e);
		if (deleted_event != deleted_events.end())
		{
			delete e;
			deleted_events.erase(deleted_event);
			continue;
		}
		if (e->site_event)
		{
			addParabola(e->s);
		}
		else
		{
			handleCircleEvent(e);
		}

		delete(e);
		
	}

	for (auto &site : sites)
	{
		finishEdges(&site);
	}
}

void Voronoi::addParabola(VSite * _s)
{
	VParabola* par_to_split;

	par_to_split = findParabolaAtX(_s->pos.X);
	par_to_split->is_leaf = false;

	if (par_to_split->c_e)
	{
		deleted_events.push_back(par_to_split->c_e);
		par_to_split->c_e = nullptr;
	}

	//Create the edge that results from this split
	VHalfEdge* left_edge = new VHalfEdge(FVector2D(_s->pos.X, par_to_split->getYAt(_s->pos.X, directrix_y)), par_to_split->s, _s);
	//edges.push_back(VHalfEdge(FVector2D(_s->pos.X, par_to_split->getYAt(_s->pos.X, directrix_y)), par_to_split->s, _s));
	//VHalfEdge* left_edge = &edges.back();
	_s->edges.push_back(left_edge);
	VHalfEdge* right_edge = new VHalfEdge(FVector2D(_s->pos.X, par_to_split->getYAt(_s->pos.X, directrix_y)), _s, par_to_split->s);
	//edges.push_back(VHalfEdge(FVector2D(_s->pos.X, par_to_split->getYAt(_s->pos.X, directrix_y)), _s, par_to_split->s));
	//VHalfEdge* right_edge = &edges.back();
	par_to_split->e = right_edge;
	par_to_split->s->edges.push_back(right_edge);

	left_edge->twin = right_edge;
	right_edge->twin = left_edge;

	//create and assign children of the parabolaNode that is being split
	VParabola* left_split = new VParabola(par_to_split->s, par_to_split);
	par_to_split->left_child = left_split;
		//this is an edgeNode that holds the created edge and is parent to the new parabola and the right split of parent parabola
	VParabola* edge_node = new VParabola(left_edge, par_to_split);
	par_to_split->right_child = edge_node;

	//create and assign children of the edgeNode
	VParabola* new_par = new VParabola(_s, edge_node);
	edge_node->left_child = new_par;
	VParabola* right_split = new VParabola(par_to_split->s, edge_node);
	edge_node->right_child = right_split;
	
	checkCircle(right_split);
	checkCircle(left_split);

}

VParabola* Voronoi::findParabolaAtX(float _x)
{
	VParabola * par_it = root;
	float x = 0.0;

	while (!par_it->is_leaf) 
	{
		x = par_it->getXOfIntersection( directrix_y);
		if (x <_x) 
			par_it = par_it->right_child;
		else 
			par_it = par_it->left_child;
	}
	return par_it;
}

void Voronoi::checkCircle(VParabola* _p1)
{
	VParabola* left_parent = _p1->getClosestLeftParent();
	VParabola* right_parent = _p1->getClosestRightParent();
	if (!left_parent || !right_parent)
	{
		std::cout << "either no left or right parent, wont disappear" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("either no left or right parent, wont disappear"));
		return;
	}
		

	VParabola * left_leave = left_parent->getClosestLeftLeave();
	VParabola * right_leave = right_parent->getClosestRightLeave();
	if (!left_leave || !right_leave || left_leave->s == right_leave->s)
	{
		std::cout << "did not find left or right parabola or left and right were the same" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("did not find left or right parabola or left and right were the same"));
		return;
	}

	if (_p1->s->isNeighbour(left_leave->s) && _p1->s->isNeighbour(right_leave->s) && left_leave->s->isNeighbour(right_leave->s))
	{
		std::cout << "circle event has already been handled" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("circle event has already been handled"));
		return;
	}

	FVector2D i_point;
	if (!getEdgeIntersection(left_parent->e, right_parent->e, i_point))
		return;

	FVector2D radial_vec = right_leave->s->pos - i_point;
	float event_y = i_point.Y - radial_vec.Size();
	if (event_y >= directrix_y)// || FMath::IsNearlyEqual(event_y, directrix_y, 0.001f))
	{
		std::cout << "circle is completely above the directrix and is therefore not a legit event" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("circle is completely above the directrix and is therefore not a legit event"));
		return;
	}
	//else if (_p1->s->isNeighbour(left_leave->s) && _p1->s->isNeighbour(right_leave->s) && left_leave->s->isNeighbour(right_leave->s))
	//{
	//	std::cout << "circle event has already been handled" << std::endl;
	//	UE_LOG(LogTemp, Warning, TEXT("circle event has already been handled"));
	//	return;
	//}

	VEvent* new_circle_event = new VEvent(FVector2D(i_point.X, event_y), _p1);
	vertices.push_back(new_circle_event->pos);
	_p1->c_e = new_circle_event;
	event_queue.push(new_circle_event);


	_p1->s->addNeighbour(left_leave->s);
	_p1->s->addNeighbour(right_leave->s);

	left_leave->s->addNeighbour(_p1->s);
	left_leave->s->addNeighbour(right_leave->s);

	right_leave->s->addNeighbour(left_leave->s);
	right_leave->s->addNeighbour(_p1->s);
}

bool Voronoi::getEdgeIntersection(VHalfEdge* _he1, VHalfEdge* _he2, FVector2D& _out)
{
	FVector2D l1_end = (_he1->tmp_start + _he1->direction);
	float x1 = _he1->tmp_start.X;
	float y1 = _he1->tmp_start.Y;
	float x2 = l1_end.X;
	float y2 = l1_end.Y;

	FVector2D l2_end = (_he2->tmp_start + _he2->direction);
	float x3 = _he2->tmp_start.X;
	float y3 = _he2->tmp_start.Y;
	float x4 = l2_end.X;
	float y4 = l2_end.Y;

	float denom = (x1 - x2)*(y3 - y4) - ( y1 - y2)*(x3 - x4);
	if (denom == 0.f)
	{
		std::cout << "trying to find interesection between parallel lines" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("trying to find interesection between parallel lines"));
		return false;
	}

	float a = (x1*y2 - y1*x2);
	float b = (x3*y4 - y3*x4);

	_out.X = (a*(x3 - x4) - (x1 - x2)*b) / denom;
	_out.Y = (a*(y3 - y4) - (y1 - y2)*b) / denom;

	if (_out.X < 0 || _out.X > dims.X)
	{
		std::cout << "event is outside of voronoi dims and will not be considered" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("event is outside of voronoi dims and will not be considered"));
		return false;
	}

	/*FVector2D check = (_out - _he1->start) / _he1->direction;
	if (check.X < 0.f || check.Y < 0.f)
		return false;

	check = (_out - _he2->start) / _he2->direction;
	if (check.X < 0.f || check.Y < 0.f)
		return false;*/

	return true;
}

void Voronoi::handleCircleEvent(VEvent* _circle_event)
{
	VParabola* par_to_remove = _circle_event->par;

	VParabola* left_parent = par_to_remove->getClosestLeftParent();
	VParabola* right_parent = par_to_remove->getClosestRightParent();

	VParabola* left_leave = left_parent->getClosestLeftLeave();
	VParabola* right_leave = right_parent->getClosestRightLeave();
	
	if (left_leave == right_leave)
	{
		std::cout << "p1 and p2 are the same, this should never happen" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("p1 and p2 are the same, this should never happen"));
	}

	if (left_leave->c_e)
	{
		deleted_events.push_back(left_leave->c_e);
		left_leave->c_e = nullptr;
	}
	if (right_leave->c_e)
	{
		deleted_events.push_back(right_leave->c_e);
		right_leave->c_e = nullptr;
	}

	FVector2D end_point = FVector2D(_circle_event->pos.X, par_to_remove->getYAt(_circle_event->pos.X, directrix_y));
	vertices.push_back(end_point);

	//determine if this is a start or endpoint of the edge and then assign the point
	left_parent->e->setStartOrEnd(end_point);
	right_parent->e->setStartOrEnd(end_point);
	
	//find the highest parent and assign the new edge to it
	VParabola * highest = nullptr;
	VParabola * par_it = par_to_remove;
	while (par_it != root)
	{
		par_it = par_it->parent;
		if (par_it == left_parent) highest = left_parent;
		if (par_it == right_parent) highest = right_parent;
	}
	if (highest == nullptr)
	{
		std::cout << "crash about to happen because highest was not assigned" << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("crash about to happen because highest was not assigned"));
	}
	//TODO: examine
	highest->e = new VHalfEdge(end_point, left_leave->s, right_leave->s);
	//edges.push_back(VHalfEdge(end_point, left_leave->s, right_leave->s));
	//highest->e = &edges.back();
	right_leave->s->edges.push_back(highest->e);

	left_leave->s->edges.push_back(new VHalfEdge(end_point, right_leave->s, left_leave->s));
	//edges.push_back(VHalfEdge(end_point, right_leave->s, left_leave->s));
	//left_leave->s->edges.push_back(&edges.back());
	
	highest->e->twin = left_leave->s->edges.back();
	left_leave->s->edges.back()->twin = highest->e;
	highest->e->setStartOrEnd(end_point);
	//edges.push_back(highest->e);

	//reassing parabolas to correct order
	VParabola* removed_parent = par_to_remove->parent->parent;
	if (par_to_remove->parent->left_child == par_to_remove)
	{
		if (removed_parent->left_child == par_to_remove->parent) removed_parent->setLeft(par_to_remove->parent->right_child);
		if (removed_parent->right_child == par_to_remove->parent) removed_parent->setRight(par_to_remove->parent->right_child);
	}
	else
	{
		if (removed_parent->left_child == par_to_remove->parent) removed_parent->setLeft(par_to_remove->parent->left_child);
		if (removed_parent->right_child == par_to_remove->parent) removed_parent->setRight(par_to_remove->parent->left_child);
	}
	
	delete par_to_remove->parent;
	delete par_to_remove;

	checkCircle(left_leave);
	checkCircle(right_leave);
}

void Voronoi::finishEdges(VSite* _site)
{
	for (auto edge : _site->edges)
	{
		//check if edge is already complete
		if (edge->start_found && edge->twin->start_found)
			continue;

		//complete edge
		VHalfEdge* e;
		if (edge->start_found)
			e = edge;
		else
			e = edge->twin;

		//check if we intersect x border before y
		// if it does assign the intersection as the final point and continue to next loop iteration
		if (e->direction.X > 0.f) {
			float t = (dims.X - e->start.X) / e->direction.X;
			float y = e->start.Y + e->direction.Y*t;
			if (y > 0.f && y < dims.Y) {
				e->end = FVector2D(dims.X, y);
				e->twin->start = FVector2D(dims.X, y);
				continue;
			}
		}
		else {
			float t = (0.f - e->start.X) / e->direction.X;
			float y = e->start.Y + e->direction.Y*t;
			if (y > 0.f && y < dims.Y) {
				e->end = FVector2D(0.f, y);
				e->twin->start = FVector2D(0.f, y);
				continue;
			}
		}

		//The line will intersect y border before x border
		//assign the intersection as the final point
		if (e->direction.Y > 0.f) {
			float t = (dims.Y - e->start.Y) / e->direction.Y;
			float x = e->start.X + e->direction.X*t;
			e->end = FVector2D(x, dims.Y);
			e->twin->start = FVector2D(x, dims.Y);
		}
		else {
			float t = (0.f - e->start.Y) / e->direction.Y;
			float x = e->start.X + e->direction.X*t;
			e->end = FVector2D(x, 0.f);
			e->twin->start = FVector2D(x, 0.f);
		}
	}
}
