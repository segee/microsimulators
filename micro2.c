/********************************************************************
*                          Micro2.c                                 *
*********************************************************************
* Contains the board 2 data structures and microword access routines.
********************************************************************/

#include "sim.h"
#include "micro.c"

int BOARD = 2;
int last_index = 0;
unsigned char *memory;          /* not used in board 2          */

/*---------------------------------------------------------------
*                     inputs
*---------------------------------------------------------------*/
myint op_code = 0;              /* op code input switches       */
int go_button = 1;              /* go button status             */
myint ia_reg = 0xffL;           /* data input register a        */
myint ib_reg = 0xffL;           /* data input register b        */

/*---------------------------------------------------------------
*                    outputs
*---------------------------------------------------------------*/
myint f_reg = 0xffL;            /* result register f            */

/*---------------------------------------------------------------
*              temporary registers and busses
*---------------------------------------------------------------*/
myint ta_reg = 0xffL;           /* temporary register a         */
myint tb_reg = 0xffL;           /* temporary register b         */
myint a_bus,b_bus,f_bus;        /* alu busses                   */

/*---------------------------------------------------------------
*              status bits  (asserted=1)
*---------------------------------------------------------------*/
int z_bit = 0;                  /* zero-detect                  */
int n_bit = 1;                  /* negative result              */
int c4_bit = 0;                 /* half-carry bit               */
int c8_bit = 0;                 /* full carry bit               */
int zp_bit = 1;                 /* zero-detect (positive logic) */


struct micro_word
{
   myint micro_ad;
   myint count;
   myint bop;
   myint a_source;
   myint b_source;
   myint alu_func;
   myint cin;
   myint alu_dest;
};

struct micro_word mw =        /* current micro_word */
{
   0,0,0,0,0,0,0,0
};

struct micro_word tgm,stm;    /* trigger & store mw specs   */

/*---------------------------------------------------------------
*                    trace data array and specs
*---------------------------------------------------------------*/

struct trace_data
{
   myint mad;                 /* address of executing micro_word    */
   myint tick;                /* system clock (in cycles)           */
   myint insw[3];             /* input switches: ia, ib, opcode     */
   myint reg[3];              /* ia,ib,ta,tb,f-registers            */
   myint bus[3];              /* a,b,f-busses                       */
   myint mpc;                 /* micro_program counter              */
   int   bit[6];              /* status bits and go button state    */
   int   rep;                 /* data representation state          */
};

struct trace_data tr[TMAX];   /* trace data array                   */
struct trace_data tg,st;      /* trigger_on & store_on state specs  */


/***********************************************************************
 *       load_mw()   loads the microword from cs
 **********************************************************************/

int load_mw(cindex)
unsigned cindex;
{
   unsigned windex;
   windex=cindex*CS_INT_WIDTH;
   mw.micro_ad = getbits(control_store+windex,0,8);
   mw.count = getbits(control_store+windex,8,1);
   mw.bop = getbits(control_store+windex,9,4);
   mw.a_source = getbits(control_store+windex,13,1);
   mw.b_source = getbits(control_store+windex,14,1);
   mw.alu_func = getbits(control_store+windex,15,5);
   mw.cin = getbits(control_store+windex,20,1);
   mw.alu_dest = getbits(control_store+windex,21,3);
}

/***********************************************************************
 *       store_mw()   stores the microword into cs
 **********************************************************************/

int store_mw(cindex)
unsigned cindex;
{
   unsigned windex;
   windex=cindex*CS_INT_WIDTH;
   putbits(control_store+windex,0,8,(unsigned) mw.micro_ad);
   putbits(control_store+windex,8,1,(unsigned) mw.count);
   putbits(control_store+windex,9,4,(unsigned) mw.bop);
   putbits(control_store+windex,13,1,(unsigned) mw.a_source);
   putbits(control_store+windex,14,1,(unsigned) mw.b_source);
   putbits(control_store+windex,15,5,(unsigned) mw.alu_func);
   putbits(control_store+windex,20,1,(unsigned) mw.cin);
   putbits(control_store+windex,21,3,(unsigned) mw.alu_dest);
}

/***********************************************************************
 *      display_mw()   displays the current microword
 **********************************************************************/

int display_mw()
{
    char *head="\n AluDest Cin AluFunc BSource ASource Bop Count MicroAd\n";

    myprintf("%s",head);
    myprintf("    %01lx     %01lx     %02lx      %01lx       %01lx     %01lx",
        mw.alu_dest,mw.cin,mw.alu_func,mw.b_source,mw.a_source,mw.bop);
    myprintf("    %01lx      %02lx\n\n", mw.count,mw.micro_ad);
}

/*********************************************************
 *       edit_mw_menu() puts up a menu of edit choices 
 ********************************************************/

int edit_mw_menu()
{
   int mwfield;

   display_mw();
   myprintf("\n");
   myprintf("                 === EDIT MICROWORD MENU ===         \n\n");
   myprintf("           1] Micro_Ad                   5] B_Source   \n");
   myprintf("           2] Count                      6] ALU_Func   \n");
   myprintf("           3] Bop                        7] Cin        \n");
   myprintf("           4] A_Source                   8] ALU_Dest   \n");
   myprintf("\n");
   myprintf("                        0] Exit (Done)                 \n");
   myprintf("\n");
   myprintf("Your selection [0-8]? ");
   mwfield = get_dec();
   if (logging) fprintf(logfile,"%d\n",mwfield);
   while(mwfield < 0 || mwfield > 8)
   {
      myprintf("Please choose a number between 0 and 8: ");
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
                request("COUNT = ",&(mw.count),1L);
		break;
        case 3:
                request("BOP = ",&(mw.bop),0x0fL);
		break;
        case 4:
                request("A_SOURCE = ",&(mw.a_source),1L);
		break;
        case 5:
                request("B_SOURCE = ",&(mw.b_source),1L);
		break;
        case 6:
                request("ALU_FUNC = ",&(mw.alu_func),0x1fL);
		break;
        case 7:
                request("CIN = ",&(mw.cin),1L);
		break;
        case 8:
                request("ALU_DEST = ",&(mw.alu_dest),7L);
		break;
        default:
		break;
      }
   }
}
