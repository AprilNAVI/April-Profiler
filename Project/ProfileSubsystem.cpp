// Fill out your copyright notice in the Description page of Project Settings.


#include "ProfileSubsystem.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <sstream> 
#include <thread>


using namespace std;

/*
UProfileSubsystem::UProfileSubsystem(class FOpenGLDynamicRHI* InOpenGLRHI)
:   FrameTiming(InOpenGLRHI, 4)
,	CurrentGPUFrameQueryIndex(0)
,	OpenGLRHI(InOpenGLRHI)
,	NestedFrameCount(0)
,	bIsGpuProfilerInitialized(false)
,	ExternalGPUTime(0)
{
}

void UProfileSubsystem::GpuProfilerInit(class FOpenGLDynamicRHI* InOpenGLRHI)
{
	FrameTiming.InitResources();
	for (int32 Index = 0; Index < MAX_GPUFRAMEQUERIES; ++Index)
	{
		DisjointGPUFrameTimeQuery[Index].Init(OpenGLRHI);
	}
}

void UProfileSubsystem::StartFrame(class FOpenGLDynamicRHI* InRHI)
{
	if (!bIsGpuProfilerInitialized)
	{
		GpuProfilerInit(InRHI);
	}

	if (NestedFrameCount++>0)
	{
		// guard against nested Begin/EndFrame calls.
		return;
	}
		if (GNumAlternateFrameRenderingGroups == 1)
    	{
    		if (FrameTiming.IsSupported())
    		{
    			FrameTiming.StartTiming();
    		}
    		if (FOpenGLDisjointTimeStampQuery::IsSupported())
    		{
    			CurrentGPUFrameQueryIndex = (CurrentGPUFrameQueryIndex + 1) % MAX_GPUFRAMEQUERIES;
    			DisjointGPUFrameTimeQuery[CurrentGPUFrameQueryIndex].StartTracking();
    		}
    	}
	
}



void UProfileSubsystem::EndFrame()
{
	if (--NestedFrameCount != 0)
	{
		// ignore endframes calls from nested beginframe calls.
		return;
	}
	// Skip timing events when using SLI, they will not be accurate anyway
	if (GNumAlternateFrameRenderingGroups == 1)
	{
		if (FrameTiming.IsSupported())
		{
			FrameTiming.EndTiming();
		}
		if (FOpenGLDisjointTimeStampQuery::IsSupported())
		{
			DisjointGPUFrameTimeQuery[CurrentGPUFrameQueryIndex].EndTracking();
		}
	}	
	// Skip timing events when using SLI, as they will block the GPU and we want maximum throughput
	// Stat unit GPU time is not accurate anyway with SLI
	if (FrameTiming.IsSupported() && GNumAlternateFrameRenderingGroups == 1)
	{
		uint64 GPUTiming = FrameTiming.GetTiming();
		uint64 GPUFreq = FrameTiming.GetTimingFrequency();
		GGPUFrameTime = FMath::TruncToInt( double(GPUTiming) / double(GPUFreq) / FPlatformTime::GetSecondsPerCycle() );
	}	
	else if (FOpenGLDisjointTimeStampQuery::IsSupported() && GNumAlternateFrameRenderingGroups == 1)
	{
		static uint32 GLastGPUFrameTime = 0;
		uint64 GPUTiming = 0;
		uint64 GPUFreq = FOpenGLDisjointTimeStampQuery::GetTimingFrequency();
		int OldestQueryIndex = (CurrentGPUFrameQueryIndex + 1) % MAX_GPUFRAMEQUERIES;
		if ( DisjointGPUFrameTimeQuery[OldestQueryIndex].IsResultValid() && DisjointGPUFrameTimeQuery[OldestQueryIndex].GetResult(&GPUTiming) )
		{
			GGPUFrameTime = FMath::TruncToInt( double(GPUTiming) / double(GPUFreq) / FPlatformTime::GetSecondsPerCycle() );
			GLastGPUFrameTime = GGPUFrameTime;
		}
		else
		{
			// Keep the timing of the last frame if the query was disjoint (e.g. GPU frequency changed and the result is undefined)
			GGPUFrameTime = GLastGPUFrameTime;
		}
	}
	else
	{
		GGPUFrameTime = ExternalGPUTime;
	}	
}



void UProfileSubsystem::CleanUp()
{
	if (bIsGpuProfilerInitialized)
	{
		for (int32 Index = 0; Index < MAX_GPUFRAMEQUERIES; ++Index)
		{
			DisjointGPUFrameTimeQuery[Index].ReleaseResources();
		}

		FrameTiming.ReleaseResources();
		NestedFrameCount = 0;
		bIsGpuProfilerInitialized = false;
	}
}
*/
int32 UProfileSubsystem::GetCurrenProcessId()
{
	ConvertFileToFPidStat(PidStat,"/proc/self/stat");
	int32 ProcessId=std::stoi(this->PidStat.pid.c_str());
	UE_LOG(LogTemp,Log,TEXT("ProcessId:%d"),ProcessId);
	return ProcessId;
}

void UProfileSubsystem::ConvertFileToFPidStat(FPidStat& AnyPidStat, const char* Filename)
{
	ifstream inFile;
	
	inFile.open(Filename, ios::in);
	
	if (!inFile)
	{
		UE_LOG(LogTemp,Warning,TEXT("PidStat Read Failed!"))
		return;
	}
	inFile >> AnyPidStat.pid >> AnyPidStat.comm >> AnyPidStat.state >> AnyPidStat.ppid >> AnyPidStat.pgrp
		   >>AnyPidStat.session >> AnyPidStat.tty_nr>> AnyPidStat.tpgid >> AnyPidStat.flags >> AnyPidStat.minflt 
		   >>AnyPidStat.cminflt >> AnyPidStat.majflt >> AnyPidStat.cmajflt >> AnyPidStat.utime >> AnyPidStat.stime 
		   >> AnyPidStat.cutime >> AnyPidStat.cstime >> AnyPidStat.priority >> AnyPidStat.nice>> AnyPidStat.non
	>> AnyPidStat.itrealvalue >> AnyPidStat.starttime >> AnyPidStat.vsize >> AnyPidStat.rss;
	
	inFile.close();
}

void UProfileSubsystem::ReadStatsCPU(vector<CPUData>& entries, const char* Filename)
{
	ifstream inFile;
	if (!inFile)
	{
		UE_LOG(LogTemp,Warning,TEXT("Stat Read Failed!"))
		GEngine->AddOnScreenDebugMessage(-1,2.f,FColor::Blue,TEXT("Stat Read Failed!"));
		return;
	}
	inFile.open(Filename, ios::in);

	string line;
	const std::string STR_CPU("cpu");
	const std::size_t LEN_STR_CPU = STR_CPU.size();
	const std::string STR_TOT("tot");
	while(std::getline(inFile, line))
	{
		if(!line.compare(0, LEN_STR_CPU, STR_CPU))
		{
			std::istringstream ss(line);

			// store entry
			entries.emplace_back(CPUData());
			CPUData & entry = entries.back();

			// read cpu label
			ss >> entry.cpu;

			if(entry.cpu.size() > LEN_STR_CPU)
				entry.cpu.erase(0, LEN_STR_CPU);
			else
				entry.cpu = STR_TOT;

			// read times
			for(int i = 0; i < 10; ++i)
			{
				ss >> entry.times[i];
				UE_LOG(LogTemp,Warning,TEXT("%d"),entry.times[i])
				GEngine->AddOnScreenDebugMessage(-1,2.f,FColor::Blue,FString::FromInt(static_cast<int>(entry.times[i])));
			}

		}
	}
	
}

void UProfileSubsystem::GetCpuUsageSwapVerSion()
{
	vector<CPUData> NewEntity;
	ReadStatsCPU(NewEntity,"/proc/stat");
	ComputeStats(CurrentEntity, NewEntity,CurrentCpuOccupancyRate,CurrentActiveCpuTime,CurrentTotalCpuTime);
	CurrentEntity=NewEntity;
	
	
}

void UProfileSubsystem::GetMemoryUsage()
{
	ConvertFileToFPidStat(this->PidStat,"/proc/self/stat");
	int64 rss64=stoi(PidStat.rss);
	CurrentMemoryUsage= rss64 * 4;
	CurrentMemoryOccupancyRate=CurrentMemoryUsage*100.f/7602948;
	UE_LOG(LogTemp,Log,TEXT("Memory Usage:%f"),CurrentMemoryOccupancyRate);
	UE_LOG(LogTemp,Log,TEXT("Memory Amount:%d"),CurrentMemoryUsage);
	//D:\\ue4project\\ActionRPG\\Content\\pidstat1.txt
	//proc/self/stat
}

void UProfileSubsystem::ComputeStats(const std::vector<CPUData>& entries1, const std::vector<CPUData>& entries2,float& Usage,int64& ActiveTime,int64& TotalTime)
{
	const int NUM_ENTRIES = entries1.size();
	float ALL_TOT_ALTIME=0.f;
	float ALL_ACTIVE_TIME=0.f;
	for(int i = 0; i < NUM_ENTRIES; ++i)
	{
		const CPUData & e1 = entries1[i];
		const CPUData & e2 = entries2[i];


		const float ACTIVE_TIME	= static_cast<float>(GetActiveTime(e2) - GetActiveTime(e1));
		const float IDLE_TIME	= static_cast<float>(GetIdleTime(e2) - GetIdleTime(e1));
		const float TOTAL_TIME	= ACTIVE_TIME + IDLE_TIME;
		ALL_TOT_ALTIME+=TOTAL_TIME;
		ALL_ACTIVE_TIME+=ACTIVE_TIME;
	}
	TotalTime=ALL_TOT_ALTIME;
	ActiveTime=ALL_ACTIVE_TIME;
	Usage=ALL_ACTIVE_TIME/ALL_TOT_ALTIME*100.f;
	UE_LOG(LogTemp,Log,TEXT("CpuUsage:%f"),Usage);
	UE_LOG(LogTemp,Log,TEXT("ALL_ACTIVE_TIME:%f"),ALL_ACTIVE_TIME);
	UE_LOG(LogTemp,Log,TEXT("ALL_TOT_ALTIME:%f"),ALL_TOT_ALTIME);
}

int64 UProfileSubsystem::GetIdleTime(const CPUData& entity)
{
	return	entity.times[S_IDLE] + 
	entity.times[S_IOWAIT];
}

int64 UProfileSubsystem::GetActiveTime(const CPUData& entity)
{
	return	entity.times[S_USER] +
	entity.times[S_NICE] +
	entity.times[S_SYSTEM] +
	entity.times[S_IRQ] +
	entity.times[S_SOFTIRQ];
}

void UProfileSubsystem::StartGetCpuUsage()
{
	ReadStatsCPU(CurrentEntity,"/proc/stat");
	GetWorld()->GetTimerManager().SetTimer(ProfileTimer,this,&UProfileSubsystem::GetCpuUsageSwapVerSion,4.f,true);
}

void UProfileSubsystem::StartProfilering()
{
	StartGetMemoryUsage();
	StartGetCpuUsage();
}

void UProfileSubsystem::StartGetMemoryUsage()
{
	UE_LOG(LogTemp,Log,TEXT("1111111111111"));
	GetWorld()->GetTimerManager().SetTimer(MemoryTimer,this,&UProfileSubsystem::GetMemoryUsage,4.f,true,-1);
	
}

void UProfileSubsystem::Tick(float DeltaTime)
{
	
}

