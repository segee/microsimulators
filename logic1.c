/**********************************************************************
*                             Logic1.c                                *
***********************************************************************
*   Contains the alu and sequencing logic for board 1.
***********************************************************************/

#include "sim.h"
#include "micro1.h"

/**********************************************************************
*                       logic() 
***********************************************************************
*   Perform the logic functions
**********************************************************************/

int logic()
{
   int mpc_load;

/*    determine the load status   */

   switch((mw.bop>>1)&3)
   {
      case 0: mpc_load=(sw[mw.cond]==mw.pol);  break;
      case 1: mpc_load = TRUE;                 break;
      case 2: mpc_load = FALSE;                break;
      case 3: mpc_load = TRUE;                 break;
   }
   
   return(mpc_load);
}


