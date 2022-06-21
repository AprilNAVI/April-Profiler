// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <fstream>
#include <vector>
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProfileSubsystem.generated.h"

static const int MAX_GPUFRAMEQUERIES = 4;

/****************************************************************************************
* TODO list
* √1、读取/proc/stat写入CurrentEntity
* √2、读取/proc/self/stat写入FPidStat
* √3、从FPidStat解析pid
* √4、从FPidStat解析内存信息
*	5、从CurrentEntity读取各个core的信息
*	5、计算GPU耗时
* 
****************************************************************************************/

using namespace std;

struct FPidStat
{
	string pid="0";
	
	string comm;
	
	string state;
	
	string ppid;
	
	string pgrp;
	
	string session;
	
	string tty_nr;
	
    string tpgid;
    
	string flags;
	
	string minflt;
	
	string cminflt;
	
	string majflt;
	
	string cmajflt;
	
	string utime;
	
	string stime;
	
	string cutime;
	
	string cstime;
	
	string priority;
	
	string nice;
	
	string non;
	
	string itrealvalue;
	
	string starttime;
	
	string vsize;
	
	string rss="0";
};

struct CPUData
{
	string cpu;
	
	size_t times[10];
};

enum CPUStates
{
	S_USER = 0,
	S_NICE,
	S_SYSTEM,
	S_IDLE,
	S_IOWAIT,
	S_IRQ,
	S_SOFTIRQ,
	S_STEAL,
	S_GUEST,
	S_GUEST_NICE
};
/**
 * 
 */
UCLASS()
class BLANK_API UProfileSubsystem : public UWorldSubsystem,public FTickableGameObject
{
	GENERATED_BODY()
public:
	

	FTimerHandle ProfileTimer;

	FTimerHandle MemoryTimer;

	/** Holds the Current Memory Occupancy rate. */
	FPidStat PidStat;

	/** Holds the All Cpu‘s Data from last sample. */
	
	std::vector<CPUData> CurrentEntity;

	/** Holds the Current Memory Occupancy rate. */
	float CurrentMemoryOccupancyRate=0.f;
	
	/** Holds the Current Cpu Occupancy Rate. */
	float CurrentCpuOccupancyRate=0.f;

	/** Holds the Current Logic Time cost. */
	float CurrentLogicFrameTime=0.f;

	/** Holds the Current Real Time cost. */
	float CurrentRealFrameTime=0.f;

	/** Holds the Current Render Time cost. */
	float CurrentRenderFrameTime=0.f;

	/** Holds the Current Memory Usage. */
	int64 CurrentMemoryUsage=0;

	/** Holds the Total Cpu Time from Last sample. */
	int64 CurrentTotalCpuTime=0;

	/** Holds the Active Cpu Time from Last sample. */
	int64 CurrentActiveCpuTime=0;

	

	/*
	int32 NestedFrameCount;
	
	uint32 GPUFrameTime32=0;

	uint32 ExternalGPUTime;

	bool bIsGpuProfilerInitialized;

	bool bIsSupported=false;
	// Frequency for the timing values, in number of ticks per seconds, or 0 if the feature isn't supported. 
	TStaticArray<uint64, MAX_NUM_GPUS> GTimingFrequency;

	FOpenGLBufferedGPUTiming FrameTiming;

	class FOpenGLDynamicRHI* OpenGLRHI;

	FOpenGLDisjointTimeStampQuery DisjointGPUFrameTimeQuery[MAX_GPUFRAMEQUERIES];

	int CurrentGPUFrameQueryIndex;
*/

public:
/*
 *UProfileSubsystem(class FOpenGLDynamicRHI* InOpenGLRHI);
 *
	void GpuProfilerInit(class FOpenGLDynamicRHI* InOpenGLRHI);

	void StartFrame(class FOpenGLDynamicRHI* InRHI);

	void EndFrame();

	void CleanUp();

	void SetTimingFrequency(uint64 TimingFrequency, uint32 GPUIndex = 0)
		{
			GTimingFrequency[GPUIndex] = TimingFrequency;
		}
	*/
	/*! @fn int32 GetCurrenProcessId();
	*  @brief 根据proc/self/stat文件读入 Pid
	*  @return			返回Pid
	*/
	int32 GetCurrenProcessId();

	/*! @fn void ConvertFileToFPidStat(FPidStat& AnyPidStat,const char* Filename);
	*  @brief 将proc/self/stat文件读入 FPidStat
	*  @param[out] AnyPidStat 要读入的FPidStat
	*  @param[in] Filename	 文件目录地址
	*/
	static void ConvertFileToFPidStat(FPidStat& AnyPidStat,const char* Filename);

	/*! @fn void ReadStatsCPU(vector<CPUData> & entries, const char* Filename);
	*  @brief 将proc/stat文件读入 vector<CPUData>
	*  @param[out] entries 要写入的vector<CPUData>
	*  @param[in] Filename	 文件目录地址
	*/
	static void ReadStatsCPU(vector<CPUData> & entries, const char* Filename);
	
	/*! @fn void GetCpuUsageSwapVerSion();
	*  @brief 定义一个新的vector<CPUData>和当前的vector<CPUData>进行比较，并将新的写入
	*/	
	UFUNCTION()
	void GetCpuUsageSwapVerSion();

	/*! @fn void GetMemoryUsage();
	*  @brief 从FPidStat读入内存相关的信息并改写类中成员MemoryUsage和MemoryOccupancyRate的值
	*/		
	void GetMemoryUsage();

	/*! @fn static void ComputeStats(const std::vector<CPUData> & entries1,
	* const std::vector<CPUData> & entries2,float& Usage,int64& ActiveTime,int64& TotalTime);
	*  @param[in] entries1 要计算的vector<CPUData>
	*  @param[in] entries2 要计算的vector<CPUData>
	*  @param[out] Usage 最后改写返回的总Cpu占用率
	*  @param[out] ActiveTime  自从上次采样经过的cpu活动时间
	*  @param[out] TotalTime  自从上次采样经过的cpu总时间
	*  @brief 从FPidStat读入内存相关的信息并改写类中成员MemoryUsage和MemoryOccupancyRate的值
	*/		
	static void ComputeStats(const std::vector<CPUData> & entries1, const std::vector<CPUData> & entries2,float& Usage,int64& ActiveTime,int64& TotalTime);

	/*! @fn static int64 GetIdleTime(const CPUData & entity);
	*  @param[in] entity 要计算的cpu数据
	*  @brief 读入CPU数据计算CPU的闲置时间
	*  @return	返回Cpu闲置时间
	*/	
	static int64 GetIdleTime(const CPUData & entity);
	
	/*! @fn static int64 GetActiveTime(const CPUData & entity);
	*  @param[in] entity 要计算的cpu数据
	*  @brief 读入CPU数据计算CPU的活跃时间
	*  @return	返回Cpu活跃时间
	*/	
	static int64 GetActiveTime(const CPUData & entity);

	/*! @fn void StartGetCpuUsage();
	*  @brief 开始4秒采样一次CPU数据
	*/		
	void StartGetCpuUsage();
	
	/*! @fn void StartProfilering();
	*  @brief 开始性能分析
	*/	
	void StartProfilering();
	
	/*! @fn void StartGetMemoryUsage();
	*  @brief 开始4秒采样一次内存数据
	*/	
	void StartGetMemoryUsage();
	
	virtual TStatId GetStatId() const override{return Super::GetStatID();}

	virtual bool IsTickable() const override{return true;}
	
	virtual void Tick(float DeltaTime) override ;

};


