/********************************************************************
*                          Trace.c                                  *
*********************************************************************
*   Generic variables and functions for the trace?.c source files.
********************************************************************/

                        /* pointers into the trace_ data array:    */
   int tndx=0;          /*     next available slot                 */
   int trigptr=0;       /*     the state that triggered the trace  */
   int tracing=FALSE;   /* trace status                            */
   int triggered=FALSE; /* trigger status                          */
   int step_count=0;    /* number of states stored since started   */

/********************************************************************
*                        inc_tndx()                                 *
*********************************************************************
*  Increment the pointer into the circular trace array.
********************************************************************/

int inc_tndx()
{
   if((++tndx)==TMAX)
      tndx=0;
   if((++step_count)==TMAX)
   {
      myprintf("\nTrace complete. "); lnl;
      stop_trace();
      return(TRUE);
   }
   return(FALSE);
}

/********************************************************************
*                        clear_trace()                              *
*********************************************************************
*  Clears the trace_data array without changing specs.
********************************************************************/

int clear_trace()
{
   step_count=0;
   triggered = FALSE;
   trigptr = 0;
   tndx = 0;
}


/********************************************************************
*                        start_trace()                              *
*********************************************************************
*  Clear trace data and start tracing.
********************************************************************/

int start_trace()
{
   clear_trace();
   tracing = TRUE;
   myprintf("Trace: ACTIVE\n");
}

/********************************************************************
*                        stop_trace()                               *
*********************************************************************
*  Stop tracing; preserve trace data pointers.
********************************************************************/

int stop_trace()
{
   tracing = FALSE;
   myprintf("Trace: INACTIVE\n");
}

/********************************************************************
*                         savestate()                               *
*********************************************************************
*   Returns true if current state matches store specs.
********************************************************************/

int savestate()
{
   return(matchstate(&st,&stm));
}

/********************************************************************
*                         trigstate()                               *
*********************************************************************
*   Returns true if current state matches trigger specs.
********************************************************************/

int trigstate()
{
   if(!triggered)
      return(matchstate(&tg,&tgm));
   else
      return(FALSE);
}

/********************************************************************
*                          trace()                                  *
*********************************************************************
*  Process current state for tracing function.
********************************************************************/

int trace()
{
   if(tracing)
   {
      if(trigstate())
      {
         trigptr=tndx;
         triggered = TRUE;
      }
      if (triggered) return(store_trace());
   }
   return(FALSE);
}

/********************************************************************
*                          set_trigger()                            *
*********************************************************************
*   Configure the trigger_on state.
********************************************************************/

int set_trigger()
{
   if(tracing)
   {
      myprintf("Tracing is ACTIVE!\n");
      myprintf("Trigger specs cannot be changed while tracing!\n\n");
   }
   else
      set_spec(&tg,&tgm,"TRIGGER");
}

/********************************************************************
*                          set_store()                              *
*********************************************************************
*   Configure the store_on state.
********************************************************************/

int set_store()
{
   if(tracing)
   {
      myprintf("Tracing is ACTIVE!\n");
      myprintf("Store specs cannot be changed while tracing!\n\n");
   }
   else
      set_spec(&st,&stm,"STORE");
}

/********************************************************************
*                        show_config()                              *
*********************************************************************
*  Display the trigger and store configurations.
********************************************************************/

int show_config()
{
   int j=1;
   myprintf("TRIGGER ON:\n");
   show_spec(&tg,&tgm,&j);
   if(j>MAXLINES) more();
   myprintf("\nSTORE ON:\n");
   j+=2;
   show_spec(&st,&stm,&j);
   if(j>MAXLINES) more();
   if(tracing)
      myprintf("\nTrace: ACTIVE \n");
   else
      myprintf("\nTrace: INACTIVE \n");
}

