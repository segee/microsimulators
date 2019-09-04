/********************************************************************
*                          Micro1.c                                 *
*********************************************************************
* Contains the board 1 data structures and microword access routines.
********************************************************************/

#include "sim.h"
#include "micro.c"

int BOARD = 1;
int sw[4] = {0,0,0,0};        /* board1 input switches */
int last_index = 0;           /* latch for MPC value   */
unsigned char *memory;        /* not used in board 1   */

struct micro_word
{
   myint micro_ad;
   myint bop;
   myint pol;
   myint cond;
   myint data;
};

struct micro_word mw =        /* current micro_word */
{
   0,0,0,0,0
};

struct micro_word tgm,stm;    /* trigger and store mw specs */

/*----------------------------------------------------------------------------
*                           trace data array
*---------------------------------------------------------------------------*/

struct trace_data
{
   myint    mad;                /* address of executing microword             */
   myint    tick;               /* system clock (in cycles)                   */
   myint    insw[4];            /* input switches                             */
   myint    mpc;                /* micro_program counter                      */
};

struct trace_data tr[TMAX];   /* trace_data array                           */
struct trace_data tg;         /* trigger_on state specs                     */
struct trace_data st;         /* store_on state specs                       */

/******************************************************
 *       load_mw()   loads the microword from cs
 ******************************************************/

int load_mw(cindex)
unsigned cindex;
{
	unsigned windex;
	windex=cindex*CS_INT_WIDTH;
   mw.micro_ad = getbits(control_store+windex,0,8);
   mw.bop = getbits(control_store+windex,8,3);
   mw.pol = getbits(control_store+windex,12,1);
   mw.cond = getbits(control_store+windex,13,2);
   mw.data = getbits(control_store+windex,17,4);
}

/******************************************************
 *       store_mw()   stores the microword into cs
 ******************************************************/

int store_mw(cindex)
unsigned cindex;
{
   unsigned windex;
   windex=cindex*CS_INT_WIDTH;
   putbits(control_store+windex,0,8,(unsigned) mw.micro_ad);
   putbits(control_store+windex,8,3,(unsigned) mw.bop);
   putbits(control_store+windex,12,1,(unsigned) mw.pol);
   putbits(control_store+windex,13,2,(unsigned) mw.cond);
   putbits(control_store+windex,17,4,(unsigned) mw.data);
}

/*******************************************************
 *      display_mw()   displays the current microword
 *******************************************************/

int display_mw()
{
  myprintf("Microword:  DATA  COND  POL  BOP  MICRO_AD\n");
  myprintf("%15lx%6lx%5lx%5lx      %02lx\n",
                   mw.data,mw.cond,mw.pol,mw.bop,mw.micro_ad);
}

/*********************************************************
 *       edit_mw_menu() puts up a menu of edit choices 
 ********************************************************/

int edit_mw_menu()
{
   int mwfield;

   display_mw();
   myprintf("\n");
   myprintf("                 === EDIT MICROWORD MENU ===      \n\n");
   myprintf("           1] Micro_Ad                   4] Cond    \n");
   myprintf("           2] Bop                        5] Data    \n");
   myprintf("           3] Pol                                   \n");
   myprintf("\n");
   myprintf("                        0] Exit (Done)              \n");
   myprintf("\n");
   myprintf("Your selection [0-5]? "); 
   mwfield = get_dec();
   if (logging) fprintf(logfile,"%d\n",mwfield);
   while(mwfield < 0 || mwfield > 5)
   {
      myprintf("Please choose a number between 0 and 5: "); 
      mwfield = get_dec();
      if (logging) fprintf(logfile,"%d\n",mwfield);
   }
   return(mwfield);
}

/*********************************************************
 *       edit_mw()  changes the microword
 ********************************************************/

int edit_mw()
{
   while (TRUE)       /* infinite loop */
   {
      switch(edit_mw_menu()) 
      {
	case 0:
		return(0);
		break;
	case 1:
                request("MICRO_AD = ",&(mw.micro_ad),0xffL);
		break;
        case 2:
                request("BOP = ",&(mw.bop),0x07L);
		break;
        case 3:
                request("POL = ",&(mw.pol),1L);
		break;
        case 4:
                request("COND = ",&(mw.cond),3L);
		break;
        case 5:
                request("DATA = ",&(mw.data),0x0fL);
		break;
        default:
		break;
      }
   }
}

