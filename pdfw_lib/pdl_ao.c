//===========================================================================
//
// NAME:    pdl_ao.c:
//
// DESCRIPTION:
//
//          PowerDAQ Linux driver interface to Analog Output FW functions
//
// AUTHOR: Frederic Villeneuve 
//         Alex Ivchenko
//
//---------------------------------------------------------------------------
//      Copyright (C) 2000,2004 United Electronic Industries, Inc.
//      All rights reserved.
//---------------------------------------------------------------------------
// For more informations on using and distributing this software, please see
// the accompanying "LICENSE" file.
//
// this file is not to be compiled independently
// but to be included into pdfw_lib.c



////////////////////////////////////////////////////////////////////////
//
// ANALOG OUTPUT SECTION
//
////////////////////////////////////////////////////////////////////////

// Function:    int PdAOutSetCfg(PSYNCHRONIZED_CONTEXT pContext)
//
// Parameters:  int board -- ptr to adapter info struct
//              u32 dwAOutCfg      -- AOut Configuration word
//              u32 dwAOutPostTrig -- AOut Post-trigger Scan Count
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The set AOut configuration command sets the operating
//              configuration of the AOut subsystem.
//
// Notes:       See 6.3.5.1 for the AOut Configuration Word format.
//
//              This command is valid only when the AOut subsystem is in
//              the configuration state (acquisition disabled).
//
int pd_aout_set_config(int board, u32 config, u32 posttrig)
{
   int ret = 0; 

   pd_board[board].AoutSS.bRev3Mode = FALSE;
   
   // Issue PD_AOCFG command and check ack.
   if ( !pd_dsp_cmd_ret_ack(board, PD_AOCFG) )
      return 0;

   pd_dsp_write(board, config);
   pd_dsp_write(board, posttrig);
   
   ret = pd_dsp_read_ack(board);
   
   DPRINTK_IF(ret, "pd_aout_set_config failed\n");

   return ret;
}

// Function:    int PdAOutSetCvClk(PSYNCHRONIZED_CONTEXT pContext)
//
// Parameters:  int board -- ptr to adapter info struct
//              u32 dwClkDiv  -- AOut conversion clock divider
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The set internal AOut conversion start (pacer) clock
//              configures the DSP Timer to generate a clock signal
//              using the specified divider from a 11.0 MHz base clock
//              frequency.
//
// Notes:
//
int pd_aout_set_cv_clk(int board, u32 dwClkDiv)
{
   int ret;

   // Issue PD_AOCVCLK with param and check acknowledge.
   ret = pd_dsp_cmd_write_ack(board, PD_AOCVCLK, dwClkDiv);

   DPRINTK_IF(ret, "pd_aout_set_cv_clk failed\n");

   return ret;
}

//
// Function:    int PdAOutSetEvents(PSYNCHRONIZED_CONTEXT pContext)
//
// Parameters:  int board -- ptr to adapter info struct
//              u32 dwEvents  -- AOutIntrStat Event configuration word
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: Set selected AOut AOutIntrStat event bits enabling/disabling
//              and/or clearing individual firmware level events, thereby
//              re-enabling the event interrupts.
//
//              AOutIntrStat Bit Settings:
//
//                  AOB_xxxxIm bits:    0 = disable, 1 = enable interrupt
//                  AOB_xxxxSC bits:    0 = clear interrupt, 1 = no change
//
// Notes:       See pdfw_def.h for the AOutIntrStat event word format.
//
int pd_aout_set_events(int board, u32 dwEvents)
{
   int ret;

   DPRINTK_E("pd: pd_aout_set_events:%x\n", dwEvents);

   // Issue PD_AOEVENTSET with param and check acknowledge.
   ret = pd_dsp_cmd_write_ack(board, PD_AOSETEVNT, dwEvents);

   DPRINTK_IF(ret, "pd_aout_set_events failed\n");

   return ret;
}

//
// Function:    int PdAOutGetStatus(PSYNCHRONIZED_CONTEXT pContext)
//
// Parameters:  int board -- ptr to adapter info struct
//              u32 *pdwStatus    -- AOut Event/Status word
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The get AOut status command obtains the current status
//              and events, including error events, of the AOut subsystem.
//
// Notes:       See pdfw_def.h for the AOutIntrStat event word format.
//
int pd_aout_get_status(int board, u32* dwStatus)
{
    // Issue PD_AOSTATUS command and get return value.
    *dwStatus = pd_dsp_cmd_ret_value(board, PD_AOSTATUS);
    
    return 1;
}

//
// Function:    int PdAOutEnableConv(PSYNCHRONIZED_CONTEXT pContext)
//
// Parameters:  int board -- ptr to adapter info struct
//              u32 dwEnable  -- 0: disable, 1: enable AOut conversions
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The enable AOut conversions command enables or disables
//              AOut conversions irrespective of the AOut Conversion Clock
//              signal or Start Trigger. During configuration and following
//              an error condition the AOut conversion process is disabled
//              and must be re-enabled to perform subsequent conversions.
//
//              This command permits completing AOut configuration before
//              the subsystem responds to the Start trigger.
//
//              PD_AONCVEN <== 0:   AOut subsystem Start Trigger is disabled
//                                  and ignored. Conversion in progress
//                                  will not be interrupted but the start
//                                  trigger is disabled from retriggering
//                                  the subsystem again.
//
//              PD_AONCVEN <== 1:   AOut subsystem Start Trigger is enabled
//                                  and data acquisition will start on the
//                                  first valid AOut start trigger.
//
// Notes:
//
int pd_aout_set_enable_conversion(int board, u32 dwEnable)
{
   int ret;

   // Issue PD_AOCVEN with param and check acknowledge.
   ret = pd_dsp_cmd_write_ack(board, PD_AOCVEN, dwEnable);

   DPRINTK_IF(ret, "pd_aout_set_enable_conversion failed\n");

   return ret;
}

//
// Function:    int PdAOutSwStartTrig(int board)
//
// Parameters:  int board -- ptr to adapter info struct
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The SW AOut start trigger command triggers the AOut Start
//              event to start value output.
//
// Notes:
//
int pd_aout_sw_start_trigger(int board)
{
   int ret;

   // Issue PD_AOSTARTTRIG command and check ack.
   ret =  pd_dsp_cmd_ret_ack(board, PD_AOSTARTTRIG);

   DPRINTK_IF(ret, "pd_aout_sw_start_trigger failed\n");

   return ret;
}

// Function:    int PdAOutSwStopTrig(int board)
//
// Parameters:  int board -- ptr to adapter info struct
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The SW AOut stop trigger command triggers the AOut Stop
//              event to stop value output.
//
// Notes:
//
int pd_aout_sw_stop_trigger(int board)
{
   int ret;

   // Issue PD_AOSTOPTRIG command and check ack.
   ret = pd_dsp_cmd_ret_ack(board, PD_AOSTOPTRIG);

   DPRINTK_IF(ret, "pd_aout_sw_stop_trigger failed\n");

   return ret;
}

//
// Function:    int PdAOutSwCvStart(int board)
//
// Parameters:  int board -- ptr to adapter info struct
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The SW AOut conversion start command triggers the DAC
//              Conversion Start signal.
//
// Notes:
//
int pd_aout_sw_cv_start(int board)
{
   int ret;

   // Issue PD_AOSWCVSTART command and check ack.
   ret = pd_dsp_cmd_ret_ack(board, PD_AOSWCVSTART);

   DPRINTK_IF(ret, "pd_aout_sw_cv_start failed\n");

   return ret;
}

//
// Function:    int PdAOutClearData(int board)
//
// Parameters:  int board -- ptr to adapter info struct
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The clear all AOut data command clears the DAC latch and
//              all AOut data storage buffers.
//
// Notes:
//
int pd_aout_clear_data(int board)
{
   int ret;

   // Issue PD_AOCLRDATA command and check ack.
   ret = pd_dsp_cmd_ret_ack(board, PD_AOCLRDATA);

   DPRINTK_IF(ret, "pd_aout_clear_data failed\n");

   return ret;
}

//
// Function:    int PdAOutReset(int board)
//
// Parameters:  int board -- ptr to adapter info struct
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The reset AOut command resets the AOut subsystem to the
//              default startup configuration.  All operations in progress
//              are stopped and all configurations and buffers are cleared.
//              Any transfers in progress are stopped as terminated. All
//              AOut firmware interrupts are masked.
//
// Notes:
//
int pd_aout_reset(int board)
{
   int ret;

   pd_board[board].AoutSS.bRev3Mode = FALSE;

   // Issue PD_AORESET command and check ack.
   ret = pd_dsp_cmd_ret_ack(board, PD_AORESET);

   DPRINTK_IF(ret, "pd_aout_reset failed\n");

   return ret;
}

//
// Function:    int PdAOutPutValue(PSYNCHRONIZED_CONTEXT pContext)
//
// Parameters:  int board -- ptr to adapter info struct
//              u32 dwCh      -- channel to output value
//              u32 dwValue   -- value to output
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The AOut put single value command writes a single value
//              to be converted and output by the specified DAC.
//
// Notes:
//
int pd_aout_put_value(int board, u32 dwValue)
{
   int ret;

   // AOut Put Single Value:
   // Issue PD_AOPUTVALUE with param and check acknowledge.
   ret = pd_dsp_cmd_write_ack(board, PD_AOPUTVALUE, dwValue);

   DPRINTK_IF(ret, "pd_aout_put_value failed\n");

   return ret;
}

//
// Function:    int PdAOutPutBlock(PSYNCHRONIZED_CONTEXT pContext)
//
// Parameters:  int board -- ptr to adapter info struct
//              u32 dwNumValues   -- number of values in buf to output
//              u32 *pdwBuf       -- buffer containing values to output
//              u32 *pdwCount     -- number of values written
//
// Returns:     int status  -- 1:  command succeeded
//                                 0: command failed
//
// Description: The AOut Put Block command writes a block of values
//              containing one or multiple values for one or both DACs
//              to be stored in a DSP buffer and output to the specified
//              DACs.
//
// Notes:
//
int pd_aout_put_block(int board, u32 dwNumValues, u32* pdwBuf, u32* pdwCount)
{
    u32  i, dwReply, dwWords, value;

    // Issue PD_AOPUTBLOCK command and read remaining buffer size.
    dwReply = pd_dsp_cmd_ret_value(board, PD_AOPUTBLOCK);
    dwWords = (dwReply < dwNumValues)?dwReply:dwNumValues;

    // Write number of words in block to be written.
    pd_dsp_write(board, dwWords);

    // Write block of dwWords to transmit register.
    //FIXME: this code shall be rewritten with copy_toio when FW will be available
    for ( i = 0; i < dwWords; i++ )
    {
        // get data from user buffer
        value = *(pdwBuf + i);
        pd_dsp_write(board, value);
    }

    *pdwCount = dwWords;

    return pd_dsp_read_ack(board);
}







