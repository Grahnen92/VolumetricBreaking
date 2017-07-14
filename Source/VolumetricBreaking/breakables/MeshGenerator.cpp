// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

using namespace std;

#include "MeshGenerator.h"
#include "VolumetricBreaking.h"

//#include "C:/Program Files (x86)/Epic Games/projects/SwordSwing/ThirdParty/voro++/includes/voro++.hh"
//#include "voro++.cc"

#include <string>
#include <list>
#include <ctime>

#include "ScalarField.h"
#include "LevelSet.h"

//#include "utilities/OMath.h"

#include "voronoi/Voronoi.h"

#include "RawMesh.h"
#include "Components/StaticMeshComponent.h"

using namespace std;

FMatrix axisRotMatrix(float _a, FVector _axis) {
	FMatrix result;

	_axis.Normalize();
	float c = FMath::Cos(_a);
	float s = FMath::Sin(_a);

	float u = _axis.X;
	float v = _axis.Y;
	float w = _axis.Z;
	float u2 = u*u;
	float v2 = v*v;
	float w2 = w*w;

	FVector a1 = FVector(u2 + (1 - u2)*c, u*v*(1 - c) + w*s, u*w*(1 - c) - v*s);
	FVector a2 = FVector(u*v*(1 - c) - w*s, v2 + (1 - v2)*c, v*w*(1 - c) + u*s);
	FVector a3 = FVector(u*w*(1 - c) + v*s, v*w*(1 - c) - u*s, w2 + (1 - w2)*c);
	FVector a4 = FVector(0.f, 0.f, 0.f);
	result.SetAxes(&a1,&a2,&a3,&a4);

	return result;
}

// Sets default values
AMeshGenerator::AMeshGenerator()
{
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	baseModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Model"));
	baseModel->SetupAttachment(root);
	
	baseModel->SetSimulatePhysics(true);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;

	baseModel->OnComponentHit.AddDynamic(this, &AMeshGenerator::OnOriginalModelHit);
}

// Called when the game starts or when spawned
void AMeshGenerator::BeginPlay()
{
	Super::BeginPlay();

	//initiate timer
	std::clock_t start;
	double duration;

	start = std::clock();
	createModelLevelSet();
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	UE_LOG(LogTemp, Warning, TEXT("creating model level set took : %f seconds"), duration);
	
	start = std::clock();
	CreateFragmentLevelSet();
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	UE_LOG(LogTemp, Warning, TEXT("creating fragment level sets took : %f seconds"), duration);
}

// Called every frame
void AMeshGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	

	if (MyPlayer->IsPaused()) {
		MyPlayer->SetPause(false);
	}
}

void AMeshGenerator::OnOriginalModelHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	
	if (NormalImpulse.Size() > 10000.f)
	{
		//initiate timer
		std::clock_t start;
		double duration;
		start = std::clock();

		OtherComp->GetBodyInstance()->AddImpulse(-NormalImpulse, false);
		FVector test_mid = baseModel->GetCenterOfMass();

		//Transform impulsnormal and hit location to model space
		FVector localImpulseNormal = baseModel->ComponentToWorld.InverseTransformVectorNoScale(NormalImpulse.GetSafeNormal());
		FVector local_hit_location = baseModel->ComponentToWorld.InverseTransformPositionNoScale(Hit.Location);
		//create the rotation matrix that align the fragments to the impulsenormal
		float rot_angle = FMath::Acos(FVector::DotProduct(FVector::UpVector, localImpulseNormal));// *180.f / PI;
		FVector rot_axis = FVector::CrossProduct( FVector::UpVector, localImpulseNormal);
		FMatrix rot_mat;
		rot_mat = axisRotMatrix(rot_angle, rot_axis);

		//calculate the individual positions of the fragments and then merge them with the model level set and the triangulate the merged level set
		for (int i = 0; i < frag_ls.size(); i++)
		{
			FVector frag_offset = frag_ls[i]->getPos();
			CreateFragment(rot_mat, local_hit_location, frag_offset, i);
		}

		baseModel->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		baseModel->ToggleVisibility();
		APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
		MyPlayer->SetPause(true);

		//finish timer
		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		UE_LOG(LogTemp, Warning, TEXT("fracture took : %f seconds"), duration);
	}
}

void AMeshGenerator::resetDestructable() 
{
	baseModel->SetWorldTransform(FTransform(FVector(0.f, 0.f, 20.f)));
	baseModel->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
	baseModel->SetPhysicsLinearVelocity(FVector::ZeroVector);
	baseModel->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	baseModel->ToggleVisibility();
	
	for (auto fragment : mesh_frags)
	{
		fragment->UnregisterComponent();
		fragment->DestroyComponent();
	}
	mesh_frags.Empty();
}


void AMeshGenerator::CreateFragment(FMatrix _collision_rot, FVector _collision_loc, FVector _frag_offset, int _frag_index)
{

	int mf_i = mesh_frags.Num();
	std::string comp_name = "ProceduralMesh" + std::to_string(mf_i);
	mesh_frags.Add(ConstructObject<UProceduralMeshComponent>(UProceduralMeshComponent::StaticClass(), this, FName(&comp_name[0])));
	mesh_frags[mf_i]->RegisterComponent();
	mesh_frags[mf_i]->AttachTo(baseModel);
	mesh_frags[mf_i]->InitializeComponent();
	mesh_frags[mf_i]->bUseComplexAsSimpleCollision = false;

	mesh_frags[mf_i]->SetMaterial(0, base_material);

	LevelSet::mergeLevelSets(base_model_ls, frag_ls[_frag_index], _collision_rot, _collision_loc, _frag_offset);
	FVector tmp_zero = FVector::ZeroVector;
	//triangulation::marchingCubes(mesh_frags[mf_i], &merging_ls, tmp_zero);
	mc_tri.marchingCubes(mesh_frags[mf_i], frag_ls[_frag_index]->getScalarField(), frag_ls[_frag_index]->getIsoVal());
	mesh_frags[mf_i]->AddRelativeLocation(_collision_rot.TransformPosition(_frag_offset), false, nullptr, ETeleportType::TeleportPhysics);
	mesh_frags[mf_i]->AddRelativeRotation(_collision_rot.Rotator(), false, nullptr, ETeleportType::TeleportPhysics);
	mesh_frags[mf_i]->AddRelativeLocation(_collision_loc, false, nullptr, ETeleportType::TeleportPhysics);

	mesh_frags[mf_i]->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);

	mesh_frags[mf_i]->SetSimulatePhysics(true);
}

void AMeshGenerator::createModelLevelSet()
{
	//Create signed distance function and level set from a model ===============================================================================
	FStaticMeshSourceModel* sourceM = &baseModel->GetStaticMesh()->SourceModels[0];
	FRawMesh rawMesh;
	sourceM->RawMeshBulkData->LoadRawMesh(rawMesh);

	float max_dim = 2.0f*FMath::Max(FMath::Max(baseModel->Bounds.BoxExtent.X, baseModel->Bounds.BoxExtent.Y), baseModel->Bounds.BoxExtent.Z);
	FVector min_dims;
	FVector max_dims;
	OMath::findMaxMinExtent(rawMesh.VertexPositions, min_dims, max_dims);
	FVector extent = max_dims - min_dims;
	//extent = extent*baseModel->GetComponentScale();

	//<-- Used to create a scalar field that always have the same amount of scalar values but distributed 
	// at cubic intervals along boxes of different dimensions
	float k = std::cbrt((extent.Z*extent.Z) / (extent.X*extent.Y));
	float j = (extent.Y / extent.Z)*k;
	float i = (extent.X / extent.Z)*k;
	FVector res(std::ceilf(resolution*i), std::ceilf(resolution*j), std::ceilf(resolution*k));
	FVector point_interval = extent / res;
	//FVector increased_extent(extent.X + (extent.X / res.X) * 2, extent.Y + (extent.Y / res.Y) * 2, extent.Z + (extent.Z / res.Z) * 2);
	increased_extent = extent + 2 * point_interval;
	mid_point = (min_dims)+(extent / 2.0f);

	//<-- create the scalar field with the determined resolution and with a somewhat increased dimension so that scalar values exist outside and around the original model
	base_model_ls = new LevelSet(res, increased_extent);
	base_model_ls->setIsoVal(0.0f);
	base_model_ls->meshToLeveSet(&rawMesh, mid_point);
	base_material = baseModel->GetMaterial(0);
	//base_model_ls->drawScalars(GetWorld());
}

void AMeshGenerator::CreateFragmentLevelSet()
{
	FVector con_dims(increased_extent.Z, increased_extent.Z, increased_extent.Z);
	con = new voro::container(-con_dims.X, con_dims.X, -con_dims.Y, con_dims.Y, -con_dims.Z, con_dims.Z, 6, 6, 6, false, false, false, 8);
	
	v_particles.push_back(FVector(68.6f - 50.f, 32.5f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(52.3f - 50.f, 24.6f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(31.2f - 50.f, 32.3f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(26.3f - 50.f, 60.9f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(45.0f - 50.f, 75.3f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(72.8f - 50.f, 62.6f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(60.5f - 50.f, 77.8f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(80.8f - 50.f, 58.9f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(60.9f - 50.f, 26.4f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(55.4f - 50.f, 24.5f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(25.0f - 50.f, 24.3f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(21.5f - 50.f, 47.9f - 50.f, 0.f)*2.f);
	v_particles.push_back(FVector(52.4f - 50.f, 80.6f - 50.f, 0.f)*2.f);

	//put particles into container
	for (int i = 0; i < v_particles.size(); i++)
		con->put(i, v_particles[i].X, v_particles[i].Y, v_particles[i].Z);

	//Verts
	cell_verts.resize(v_particles.size());
	//edges
	cell_edges.resize(v_particles.size());
	//faces
	cell_faces.resize(v_particles.size());
	cell_face_orders.resize(v_particles.size());
	cell_face_normals.resize(v_particles.size());

	//voronoi cells
	v_cells.resize(v_particles.size());
	voro::voronoicell_neighbor tmp_cell;
	//Levelsets
	frag_ls.resize(v_particles.size());

	int loop_counter = 0;
	int current_id;
	voro::c_loop_all cl(*con);
	if (cl.start())
	{
		double x, y, z;
		//do if(con.compute_cell(tmp_cell, cl)){
		do if (con->compute_cell(tmp_cell, cl)) {
			cl.pos(x, y, z);
			//v_particles[loop_counter] = FVector(x, y, z);
			current_id = cl.pid();
			v_cells[current_id] = tmp_cell;
			v_cells[current_id].vertices(cell_verts[current_id]);
			v_cells[current_id].edges(cell_edges[current_id]);
			v_cells[current_id].face_vertices(cell_faces[current_id]);
			v_cells[current_id].face_orders(cell_face_orders[current_id]);
			v_cells[current_id].normals(cell_face_normals[current_id]);

			frag_ls[current_id] = new LevelSet;
			frag_ls[current_id]->voronoiCellSignedDist(&v_cells[current_id], con, current_id, v_particles, con_dims);
			loop_counter++;
		} while (cl.inc());
	}

	int mf_i = mesh_frags.Num();
	std::string comp_name = "ProceduralMesh" + std::to_string(0);
	mesh_frags.Add(ConstructObject<UProceduralMeshComponent>(UProceduralMeshComponent::StaticClass(), this, FName(&comp_name[0])));
	mesh_frags[0]->RegisterComponent();
	mesh_frags[0]->AttachTo(baseModel);
	mesh_frags[0]->InitializeComponent();
	mesh_frags[0]->bUseComplexAsSimpleCollision = false;
	mc_tri.marchingCubes(mesh_frags[0], frag_ls[0]->getScalarField(), frag_ls[0]->getIsoVal());
}

