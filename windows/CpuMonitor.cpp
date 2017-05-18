#include "CpuMonitor.h"
#define INITGUID
#include<winternl.h>
#include<Windows.h>
#include <stdio.h>
#include <wbemidl.h>
#include <wmistr.h>
#include <evntrace.h>

namespace agent
{
// {12BF20F2-0B1C-47e8-90B3-13C81C7AFD9A}
static const GUID CpuUsageEvent =
{ 0x12bf20f2, 0xb1c, 0x47e8,{ 0x90, 0xb3, 0x13, 0xc8, 0x1c, 0x7a, 0xfd, 0x9a } };

#define LOGFILE_PATH "<FULLPATHTOLOGFILE.etl>"

static void WINAPI ProcessEvent(PEVENT_TRACE pEvent)
{
    ULONG64 CPUProcessUnits = 0;
    ULONG CPUUnits = 0;
    double CPUTime = 0;


    // Skips the event if it is the event trace header. Log files contain this event
    // but real-time sessions do not. The event contains the same information as 
    // the EVENT_TRACE_LOGFILE.LogfileHeader member that you can access when you open 
    // the trace. 

    if (IsEqualGUID(pEvent->Header.Guid, EventTraceGuid) &&
        pEvent->Header.Class.Type == EVENT_TRACE_TYPE_INFO)
    {
        ; // Skip this event.
    }
    else
    {
        if (IsEqualGUID(CpuUsageEvent, pEvent->Header.Guid))
        {
            // This example assumes that the start and end events are paired.
            // If this is the start event type, retrieve the start time values from the 
            // event; otherwise, retrieve the end time values from the event.

            if (pEvent->Header.Class.Type == EVENT_TRACE_TYPE_START)
            {
                // If the session is a private session, use the ProcessorTime
                // value to calculate the CPU time; otherwise, use the 
                // KernelTime and UserTime values.

                //if (g_bUserMode) // Private session
                //{
                //    g_StartProcessTime = pEvent->Header.ProcessorTime;
                //}
                //else  // Kernel session
                //{
                //    g_StartKernelTime = pEvent->Header.KernelTime;
                //    g_StartUserTime = pEvent->Header.UserTime;
                //}
            }
            else if (pEvent->Header.Class.Type == EVENT_TRACE_TYPE_END)
            {
                //if (g_bUserMode) // Private session
                //{
                //    // Calculate CPU time units used.

                //    CPUProcessUnits = pEvent->Header.ProcessorTime - g_StartProcessTime;
                //    wprintf(L"CPU time units used, %Lu.\n", CPUProcessUnits);

                //    // Processor time is in CPU ticks. Convert ticks to seconds.
                //    // 1000000000 = nanoseconds in one minute.

                //    CPUTime = (double)(CPUProcessUnits) / 1000000000;
                //    wprintf(L"Process CPU usage in seconds, %Lf.\n", CPUTime);
                //}
                //else  // Kernel session
                //{
                //    // Calculate the kernel mode CPU time units used for the set of instructions.

                //    CPUUnits = pEvent->Header.KernelTime - g_StartKernelTime;
                //    wprintf(L"CPU time units used (kernel), %d.\n", CPUUnits);

                //    // Calculate the kernel mode CPU time in seconds for the set of instructions.
                //    // 100 = 100 nanoseconds, 1000000000 = nanoseconds in one minute

                //    CPUTime = (double)(g_TimerResolution * CPUUnits * 100) / 1000000000;
                //    wprintf(L"Kernel mode CPU usage in seconds, %Lf.\n", CPUTime);

                //    // Calculate user mode CPU time units used for the set of instructions.

                //    CPUUnits = pEvent->Header.UserTime - g_StartUserTime;
                //    wprintf(L"\nCPU time units used (user), %d.\n", CPUUnits);

                //    // Calculate the user mode CPU time in seconds for the set of instructions.
                //    // 100 = 100 nanoseconds, 1000000000 = nanoseconds in one minute

                //    CPUTime = (double)(g_TimerResolution * CPUUnits * 100) / 1000000000;
                //    wprintf(L"User mode CPU usage in seconds, %Lf.\n", CPUTime);
                //}
            }
        }
    }
}



CPUMonitor::CPUMonitor():
    last_total_time_(0),
    last_idle_time_(0)
{
    Init();
}

CPUMonitor::~CPUMonitor()
{
}

static unsigned long long FileTime2Utc(const FILETIME &ft)
{
    LARGE_INTEGER li;
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    return li.QuadPart;
}

double CPUMonitor::StatisticUsage()
{
    StatisticUsageInternal();
    SleepEx(100, false);
    return StatisticUsageInternal();
}

void CPUMonitor::Init()
{
    ULONG status = ERROR_SUCCESS;
    EVENT_TRACE_LOGFILE trace;
    TRACE_LOGFILE_HEADER* pHeader = &trace.LogfileHeader;
    TRACEHANDLE hTrace = 0;

    // Identify the log file from which you want to consume events
    // and the callbacks used to process the events and buffers.

    ZeroMemory(&trace, sizeof(EVENT_TRACE_LOGFILE));
    trace.LogFileName = LOGFILE_PATH;
    trace.EventCallback = (PEVENT_CALLBACK)(ProcessEvent);

    hTrace = OpenTrace(&trace);
    if ((TRACEHANDLE)INVALID_HANDLE_VALUE == hTrace)
    {
        wprintf(L"OpenTrace failed with %lu\n", GetLastError());
        //goto cleanup;
    }

    //g_bUserMode = pHeader->LogFileMode & EVENT_TRACE_PRIVATE_LOGGER_MODE;

    //if (pHeader->TimerResolution > 0)
    //{
    //    g_TimerResolution = pHeader->TimerResolution / 10000;
    //}

    status = ProcessTrace(&hTrace, 1, 0, 0);
    if (status != ERROR_SUCCESS && status != ERROR_CANCELLED)
    {
        wprintf(L"ProcessTrace failed with %lu\n", status);
        //goto cleanup;
    }

//cleanup:
//
//    if ((TRACEHANDLE)INVALID_HANDLE_VALUE != hTrace)
//    {
//        status = CloseTrace(hTrace);
//    }
}

void CPUMonitor::Uninit()
{
}

double CPUMonitor::StatisticUsageInternal()
{
    SYSTEMTIME sysTime;

    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime))
    {
        return 0;
    }

    unsigned long long current_idle_time = FileTime2Utc(idleTime);
    unsigned long long current_total_time = FileTime2Utc(kernelTime) + FileTime2Utc(userTime);

    if (last_total_time_ == 0 || last_idle_time_ == 0)
    {
        last_total_time_ = current_total_time;
        last_idle_time_ = current_idle_time;
        return 0;
    }

    unsigned long long idle_duration = current_idle_time - last_idle_time_;
    unsigned long long total_duration = current_total_time - last_total_time_;

    last_total_time_ = current_total_time;
    last_idle_time_ = current_idle_time;

    if (total_duration <= 0)
    {
        return 0;
    }

    return (total_duration - idle_duration)*100.0 / static_cast<double>(total_duration);
}

} // namespace agent