//==============================================================================
//
// NAME:    DSPUCtEv.c
//
// DESCRIPTION:   PowerDAQ DSP Counter/Timer Events Test application
//
// NOTES:
//
// AUTHOR:  Frederic Villeneuve
//
// DATE:    21-NOV-2001
//
// HISTORY:
//
//      Rev 0.1,     21-NOV-2001,  F.V.,   Initial version.
//
//---------------------------------------------------------------------------
//
//      Copyright (C) 2001 United Electronic Industries, Inc.
//      All rights reserved.
//      United Electronic Industries Confidential Information.
//
//===========================================================================
//  DSP Counter/Timer module has three independent 24-bit count-up counters 
// and one 21-bit count-down prescaler counter.
//  
// Each counter is a set of four 24-bit registers:
//
//	Load register, which contains initial value of the counter when it start/
//                 restart counting. Load register has a write only access.
//	Control/Status register (read/write) which defines a counter counting mode, 
//                 reload mode (when the counter value reloaded each time, 
//                 after it reach compare value), input/output inversion flag 
//                 and interrupt flag/status bits.
//	Count register (read only), which can be used to read actual value of the 
//                 counter (useful in the measurement modes).
//	Compare register (read/write) is used to determine a output waveform shape.
//
// Prescaler can be used as a clock source for any counter which allows to 
//  generate very low frequencies, 
//   Prescaler load register contains initial value of the prescaler counter, 
//                 which reloads each time when prescaler counter reach zero. 
//                 Also different sources can be specified as a prescaler 
//                 clock. (Internal CLK/2, Timer0, Timer1, Timer2)
//	Count register (read only), which can be used to read actual value of the 
//                 prescaler.
//
//  To get more details on counter/timer functionality please refer to the 
//  Motorola DSP 56301 user manual (Motorola P/N DSP 56301UM/AD).
//
//  Please note that TIOx pins are bi-directional and it is strongly 
//  recommended to use at 100-200 OHm series resistor to connect external 
//  signals to those pins.
//===========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include "win_sdk_types.h"
#include "powerdaq.h"
#include "powerdaq32.h"
#include "powerdaq_Uct.h"

#include "ParseParams.h"

int TestMeasPeriod(int handle, int timer);


int TestMeasPeriod(int handle, int timer)
{
   int retVal = 0;
   unsigned long count;
   
   retVal = PdDspUctConfig(handle, 
                           timer,
                           Clock,
                           MeasPeriod,
                           1,                   //divider
                           0,                //stop counting after 2000 events
                           0,                //ticks2
                           0);                  //inverted
   if(retVal < 0)
   {
      printf("TestCountEvent : PdDspUctConfig error %d\n", retVal);
      return retVal;
   }

   retVal = PdDspUctStart(handle, timer);
   if(retVal < 0)
   {
      printf("TestCountEvent : PdDspUctStart error %d\n", retVal);
      return retVal;
   }

   retVal = PdDspUctWaitForEvent(handle, timer, MeasPeriod, 10000);
   if(retVal < 0)
   {
      printf("TestCountEvent : PdDspUctWaitForCompare error %d\n", retVal);
      return retVal;
   }

   retVal = PdDspUctRead(handle, timer, &count);
   if(retVal < 0)
   {
      printf("TestCountEvent : PdDspUctRead error %d\n", retVal);
      return retVal;
   }

   printf("TestCountEvent: %lu events were counted, frequency is %f Hz\n", count, (33000000.0) / count);

   retVal = PdDspUctStop(handle, timer);
   if(retVal < 0)
   {
      printf("TestCountEvent : PdDspUctStop error %d\n", retVal);
      return retVal;
   }

   return retVal;
}


/****************************************************************************

****************************************************************************/
int main(int argc, char *argv[])
{
    int  retVal = 0;
    int  handle;
    PD_PARAMS params = {0, 1, {0}, 1000.0, 0};
   
    ParseParameters(argc, argv, &params);

        
    // Print hello world
    printf("UEI PowerDAQ Test Application - DSP Counter/Timer Events Test. (C)UEI, 2001\n");

    // Open the first PowerDAQ PCI adapter.
    handle = PdAcquireSubsystem(params.board, CounterTimer, 1);
    if (handle < 0) 
       printf("PdAdapterAcquireSubsystem error %d\n", retVal);


    TestMeasPeriod(handle, 0);
    

    // return subsystem
    retVal = PdAcquireSubsystem(handle, CounterTimer, 0);
    if (retVal < 0) 
       printf("PdAdapterAcquireSubsystem error %d\n", retVal);


    return(0);
}

