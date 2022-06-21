// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BlankGameStateBase.generated.h"

class UProfileSubsystem;
/**
 * 
 */
UCLASS()
class BLANK_API ABlankGameStateBase : public AGameStateBase
{
	GENERATED_BODY()


public:

	ABlankGameStateBase();

	/** Holds the Cpu Active Time from last sample. */
	UPROPERTY(BlueprintReadOnly)
	float CpuOccupancyRate;

	/** Holds the Current Memory Occupancy rate. */
	UPROPERTY(BlueprintReadOnly)
	float MemoryOccupancyRate;

	/** Holds the Current Memory Usage. */
	UPROPERTY(BlueprintReadOnly)
	int64 MemoryUsage;

	/** Holds the Cpu Active Time from last sample. */
	UPROPERTY(BlueprintReadOnly)
	int64 CpuActiveTime;

	/** Holds the Cpu Total Time from last sample. */
	UPROPERTY(BlueprintReadOnly)
	int64 CpuTotalTime;

	/** Holds the Process Id from current game process. */
	UPROPERTY(BlueprintReadOnly)
	int32 ProcessId=0;

	/** Holds the GPU time spent ticking engine. */
	UPROPERTY(BlueprintReadOnly)
	float GpuTime=0;
	
	UPROPERTY(BlueprintReadOnly)
	float GpuFrameTime=0;

	/** Holds the ProfileSubsystem for us */
	UPROPERTY()
	UProfileSubsystem* ProfileSubsystem;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	/**
	 * Update the Hardware Property from the ProfileSubsystem.
	 *
	 * This function is called on Tick().
	 */
	void UpdateHardwareProperty();
	
};
