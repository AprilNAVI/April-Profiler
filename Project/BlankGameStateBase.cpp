// Fill out your copyright notice in the Description page of Project Settings.


#include "BlankGameStateBase.h"

#include "ProfileSubsystem.h"

ABlankGameStateBase::ABlankGameStateBase()
{
	CpuOccupancyRate=2.f;
	MemoryOccupancyRate=2.f;
	MemoryUsage=2;
	CpuActiveTime=2;
	CpuTotalTime=2;
	GpuTime=2;
	ProcessId=2;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	//PrimaryActorTick.TickInterval=1.f;
}

void ABlankGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateHardwareProperty();
	GpuTime=(float)GGPUFrameTime;
	GpuFrameTime=(float)AprilGPUFrameTime;
	
}

void ABlankGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	ProfileSubsystem=GetWorld()->GetSubsystem<UProfileSubsystem>();
	ProcessId=ProfileSubsystem->GetCurrenProcessId();
	ProfileSubsystem->StartProfilering();
	
}

void ABlankGameStateBase::UpdateHardwareProperty()
{
	MemoryOccupancyRate=ProfileSubsystem->CurrentMemoryOccupancyRate;
	MemoryUsage=ProfileSubsystem->CurrentMemoryUsage;
	CpuOccupancyRate=ProfileSubsystem->CurrentCpuOccupancyRate;
	CpuActiveTime=ProfileSubsystem->CurrentActiveCpuTime;
	CpuTotalTime=ProfileSubsystem->CurrentTotalCpuTime;
	
}
