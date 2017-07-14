// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "MCTriangulator.h"
#include "C:/Program Files (x86)/Epic Games/projects/VolumetricBreaking/ThirdParty/voro++/includes/voro++.hh"
#include "MeshGenerator.generated.h"

UCLASS()
class VOLUMETRICBREAKING_API AMeshGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeshGenerator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Category = "Mesh", BlueprintReadWrite, VisibleAnywhere)
	class USceneComponent * root;

	UPROPERTY(Category = "Mesh", BlueprintReadWrite, VisibleAnywhere)
	TArray< class UProceduralMeshComponent*> mesh_frags;


	UPROPERTY(Category = "Mesh", BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* baseModel;
	UMaterialInterface* base_material;

	class LevelSet* base_model_ls;
	FVector mid_point;
	FVector increased_extent;
	

	UPROPERTY(Category = "Mesh", BlueprintReadWrite, EditAnywhere)
	int32 resolution = 60;
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** called when projectile hits something */
	UFUNCTION()
	void OnOriginalModelHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void resetDestructable();

private:
	voro::container* con;
	std::vector<FVector> v_particles;
	std::vector<std::vector<double>> cell_verts;
	//edges
	std::vector<std::vector<int>> cell_edges;
	//faces
	std::vector<std::vector<int>> cell_faces;
	std::vector<std::vector<int>> cell_face_orders;
	std::vector<std::vector<double>> cell_face_normals;

	void createModelLevelSet();

	//voronoi cells
	std::vector<voro::voronoicell_neighbor> v_cells;
	void CreateFragmentLevelSet();
	
	void CreateFragment(FMatrix _collision_rot, FVector _collision_loc, FVector _frag_offset, int _frag_index);


	std::vector<class LevelSet*> frag_ls;

	MCTriangulator mc_tri;
	
};
