#pragma once

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <memory>

#include "Core/String.h"
// #include "ledstripeffect.h"

// Stack size for the taskmgr's idle threads
#define DEFAULT_STACK_SIZE 2048 + 512

#define IDLE_STACK_SIZE    2048
#define DRAWING_STACK_SIZE 4096
#define AUDIO_STACK_SIZE   4096
#define JSON_STACK_SIZE    4096
#define SOCKET_STACK_SIZE  4096
#define NET_STACK_SIZE     8192
#define DEBUG_STACK_SIZE   8192 // Needs a lot of stack for output if UpdateClockFromWeb is called from debugger
#define REMOTE_STACK_SIZE  4096

class IdleTask
{
  private:
    float         _idleRatio = 0;
    unsigned long _lastMeasurement;

    const int kMillisPerLoop = 1;
    const int kMillisPerCalc = 1000;

    unsigned long counter = 0;

  public:
    void ProcessIdleTime()
    {
        _lastMeasurement = millis();
        counter          = 0;

        // We need to whack the watchdog so we delay in small bites until we've used up all the time
        while( true )
        {
            int delta = millis() - _lastMeasurement;
            if( delta >= kMillisPerCalc )
            {
                // Serial.printf("Core %u Spent %lu in delay during a window of %d for a ratio of %f\n", xPortGetCoreID(), counter,
                // delta, (float)counter/delta);
                _idleRatio       = ( (float)counter / delta );
                _lastMeasurement = millis();
                counter          = 0;
            }
            else
            {
                esp_task_wdt_reset();
                delayMicroseconds( kMillisPerLoop * 1000 );
                counter += kMillisPerLoop;
            }
        }
    }

    IdleTask()
        : _lastMeasurement( millis() )
    {
    }

    // GetCPUUsage
    //
    // Returns 100 less the amount of idle time that we were able to squander.

    float GetCPUUsage() const
    {
        // If the measurement failed to even get a chance to run, this core is maxed and there was no idle time

        if( millis() - _lastMeasurement > kMillisPerCalc )
            return 100.0f;

        // Otherwise, whatever cycles we were able to burn in the idle loop counts as "would have been idle" time
        return 100.0f - 100 * _idleRatio;
    }

    // Stub entry point for calling into it without a THIS pointer
    static void IdleTaskEntry( void *that )
    {
        IdleTask *pTask = (IdleTask *)that;
        pTask->ProcessIdleTime();
    }
};

// TaskManager
//
// TaskManager runs two tasks at just over idle priority that do nothing but try to burn CPU, and they
// keep track of how much they can burn.   It's assumed that everything else runs at a higher priority
// and thus they "starve" the idle tasks when doing work.
#define DELETE_TASK( handle ) \
    if( handle != nullptr )   \
    vTaskDelete( handle )
class TaskManager
{
    TaskHandle_t _hIdle0 = nullptr;
    TaskHandle_t _hIdle1 = nullptr;

    IdleTask _taskIdle0;
    IdleTask _taskIdle1;

    TaskHandle_t _taskScreen = nullptr;

  public:
    float GetCPUUsagePercent( int iCore = -1 ) const
    {
        if( iCore < 0 )
            return ( _taskIdle0.GetCPUUsage() + _taskIdle1.GetCPUUsage() ) / 2;
        else if( iCore == 0 )
            return _taskIdle0.GetCPUUsage();
        else if( iCore == 1 )
            return _taskIdle1.GetCPUUsage();
        else
            throw new std::runtime_error( "Invalid core passed to GetCPUUsagePercentCPU" );
    }

    TaskManager()
    {
    }

    ~TaskManager()
    {
        DELETE_TASK( _taskScreen );
    }

    // CheckHeap
    //
    // Quick and dirty debug test to make sure the heap has not been corrupted

    static inline void CheckHeap()
    {
        if( false == heap_caps_check_integrity_all( true ) )
        {
            throw std::runtime_error( "Heap FAILED checks!" );
        }
    }

    void Start()
    {
        Serial.printf( "Replacing Idle Tasks with TaskManager...\n" );

        // The idle tasks get created with a priority just ABOVE idle so that they steal idle time but nothing else.  They then
        // measure how much time is "wasted" at that lower priority and deem it to have been free CPU.
        // We need to turn off the watchdogs because our idle measurement tasks burn all of the idle time just
        // to see how much there is (it's how they measure free CPU).  Thus, we starve the system's normal idle tasks
        // and have to feed the watchdog on our own.
        esp_task_wdt_delete( xTaskGetIdleTaskHandleForCPU( 0 ) );
        xTaskCreatePinnedToCore( _taskIdle0.IdleTaskEntry, "Idle0", IDLE_STACK_SIZE, &_taskIdle0, tskIDLE_PRIORITY + 1, &_hIdle0, 0 );
        esp_task_wdt_add( _hIdle0 );

        esp_task_wdt_delete( xTaskGetIdleTaskHandleForCPU( 1 ) );
        xTaskCreatePinnedToCore( _taskIdle1.IdleTaskEntry, "Idle1", IDLE_STACK_SIZE, &_taskIdle1, tskIDLE_PRIORITY + 1, &_hIdle1, 1 );
        esp_task_wdt_add( _hIdle1 );
    }

    void StartThread( TaskFunction_t entry, const char *name, int stackSize, UBaseType_t priority, TaskHandle_t *handle,
                      UBaseType_t coreId )
    {
        Serial.print( str_sprintf( ">> Launching Thread [%s].  Mem: %u, LargestBlk: %u, PSRAM Free: %u/%u, ", name, ESP.getFreeHeap(),
                                   ESP.getMaxAllocHeap(), ESP.getFreePsram(), ESP.getPsramSize() ) );
        xTaskCreatePinnedToCore( entry, name, stackSize, nullptr, priority, handle, coreId );
        CheckHeap();
    }
};