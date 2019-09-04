/********************************************************************
*                          Micro3.c                                 *
*********************************************************************
* Contains the board 3 data structures and microword access routines.
********************************************************************/

#include "sim.h"
#include "micro.c"

int BOARD = 3;

/*-------------------------------------------------------------
*                    board externals
*-------------------------------------------------------------*/
unsigned char *memory;       /* 8-bit ram and rom for board 3 */
int leds;                    /* LED output (register)         */

/*-------------------------------------------------------------
*                     other globals
*-------------------------------------------------------------*/

int last_index = 0;           /* latch for current_index      */
int cmicro = 1;               /* latch for alu C8 bit         */
int cin_bit = 1;              /* active low alu carry in bit  */
int c4_bit = 1;               /* half_carry from alu          */
int c8_bit = 1;               /* full_carry from alu          */
int z_bit = 0;                /* zero-detect bit              */
int n_bit = 0;                /* negative detect bit          */

/*-------------------------------------------------------------
*                    registers and busses
*-------------------------------------------------------------*/
myint data_in  = 0;           /* data in register             */
myint data_bus;               /* data bus                     */
myint address_bus;            /* address bus (16 bits)        */

myint acc  = 0;               /* accumulator                  */
myint sr   = 0;               /* status register              */
myint sph  = 0;               /* stack pointer high           */
myint spl  = 0;               /* stack pointer low            */
myint marh = 0;               /* memory address register high */
myint marl = 0;               /* memory address register low  */
myint pch  = 0;               /* program counter high         */
myint pcl  = 0;               /* program counter low          */

myint a_bus,b_bus,f_bus;      /* alu busses                   */

/*-------------------------------------------------------------
*                    micro word structure
*-------------------------------------------------------------*/
struct micro_word
{
   myint micro_ad;
   myint constant;
   myint bop;
   myint count;
   myint op_con_ctl;
   myint alu_source;
   myint alu_dest;
   myint ad_bus;
   myint alu_func;
   myint cin;
   myint inc_pc;
   myint en_sr;
   myint r_w;
   myint vma;
};

struct micro_word mw =            /* current micro_word */
{
   0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

struct micro_word tgm,stm;        /* trigger & store mw specs */

/*-------------------------------------------------------------
*                     trace data array
*-------------------------------------------------------------*/
struct trace_data
{
   myint          mad;      /* address of executing microword */
   myint          tick;     /* system clock (in cycles)       */
   myint          reg[9];   /* acc,sr,sph/l,marh/l,pch/l,data */
   myint          bus[5];   /* a, b, f, data, and address bus */
   myint          mpc;      /* micro_program counter          */
   myint          cmicro;   /* cmicro latch                   */
   myint          leds;     /* led state                      */
};

struct trace_data tr[TMAX]; /* trace_data array               */
struct trace_data tg;       /* trigger_on state spec          */
struct trace_data st;       /* store_on state spec            */


/********************************************************************
 *       load_mw()   loads the microword from cs
 *******************************************************************/

int load_mw(cindex)
unsigned cindex;
{
   unsigned windex;
   windex=cindex*CS_INT_WIDTH;
   mw.micro_ad = getbits(control_store+windex,0,12);
   mw.constant = getbits(control_store+windex,4,8);
   mw.bop = getbits(control_store+windex,12,4);
   mw.count = getbits(control_store+windex,16,1);
   mw.op_con_ctl = getbits(control_store+windex,17,2);
   mw.alu_source = getbits(control_store+windex,19,4);
   mw.alu_dest = getbits(control_store+windex,23,4);
   mw.ad_bus = getbits(control_store+windex,27,2);
   mw.alu_func = getbits(control_store+windex,29,5);
   mw.cin = getbits(control_store+windex,34,2);
   mw.inc_pc = getbits(control_store+windex,36,1);
   mw.en_sr = getbits(control_store+windex,37,1);
   mw.r_w = getbits(control_store+windex,38,1);
   mw.vma = getbits(control_store+windex,39,1);
}

/********************************************************************
 *       store_mw()   stores the microword into cs
 *******************************************************************/

int store_mw(cindex)
unsigned cindex;
{
   unsigned windex;
   windex=cindex*CS_INT_WIDTH;
   putbits(control_store+windex,0,12,(unsigned) mw.micro_ad);
   putbits(control_store+windex,12,4,(unsigned) mw.bop);
   putbits(control_store+windex,16,1,(unsigned) mw.count);
   putbits(control_store+windex,17,2,(unsigned) mw.op_con_ctl);
   putbits(control_store+windex,19,4,(unsigned) mw.alu_source);
   putbits(control_store+windex,23,4,(unsigned) mw.alu_dest);
   putbits(control_store+windex,27,2,(unsigned) mw.ad_bus);
   putbits(control_store+windex,29,5,(unsigned) mw.alu_func);
   putbits(control_store+windex,34,2,(unsigned) mw.cin);
   putbits(control_store+windex,36,1,(unsigned) mw.inc_pc);
   putbits(control_store+windex,37,1,(unsigned) mw.en_sr);
   putbits(control_store+windex,38,1,(unsigned) mw.r_w);
   putbits(control_store+windex,39,1,(unsigned) mw.vma);
}

/********************************************************************
 *      display_mw()   displays the current microword
 *******************************************************************/

int display_mw()
{
   char *head1=" Vma R_W EnSR IncPC Cin AluFunc AdBus AluDest";
   char *head2=" AluSrc OpConCtl Count Bop MicAd\n\n";

   myprintf("Microword:\n");
   myprintf("%s",head1);
   myprintf("%s",head2);

   myprintf("  %01lx   %01lx    %01lx    %01lx    %01lx     %02lx     %01lx   ",
        mw.vma,mw.r_w,mw.en_sr,mw.inc_pc,mw.cin,mw.alu_func,mw.ad_bus);
   myprintf("   %01lx       %01lx       %01lx      %01lx    %01lx   %03lx\n\n",
     mw.alu_dest,mw.alu_source,mw.op_con_ctl,mw.count,mw.bop,mw.micro_ad);

}

/********************************************************************
 *       edit_mw_menu() puts up a menu of edit choices 
 *******************************************************************/

int edit_mw_menu()
{
   int mwfield;
   
   display_mw();
   myprintf("\n");
   myprintf("                 === EDIT MICROWORD MENU ===          \n\n");
   myprintf("   1] Micro_Ad         6] ALU_Source       11] Inc_PC   \n");
   myprintf("   2] Constant         7] ALU_Dest         12] EN_SR    \n");
   myprintf("   3] Bop              8] AD_Bus           13] R_W      \n");
   myprintf("   4] Count            9] ALU_Func         14] VMA      \n");
   myprintf("   5] Op_Con_Ctl      10] Cin                           \n");
   myprintf("\n");
   myprintf("                       0] Exit (Done)                   \n");
   myprintf("\n");
   myprintf("Your selection [0-14]? ");
   mwfield = get_dec();
   if (logging) fprintf(logfile,"%d\n",mwfield);
   while(mwfield < 0 || mwfield > 14)
   {
      myprintf("Please choose a number between 0 and 14: ");
      mwfield = get_dec();
      if (logging) fprintf(logfile,"%d\n",mwfield);
   }
   return(mwfield);
}

/********************************************************************
 *       edit_mw()  changes the microword
 *******************************************************************/

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
                request("MICRO_AD = ",&(mw.micro_ad),0xfffL);
		mw.constant=(mw.micro_ad>>4)&0xffL;
		break;
        case 2:
                request("CONSTANT = ",&(mw.constant),0xffL);
                mw.micro_ad &= 0x0fL;
                mw.micro_ad |= mw.constant<<4;
		break;
        case 3:
                request("BOP = ",&(mw.bop),1L);
		break;
        case 4:
                request("COUNT = ",&(mw.count),1L);
		break;
        case 5:
                request("OP_CON_CTL = ",&(mw.op_con_ctl),3L);
		break;
        case 6:
                request("ALU_SOURCE = ",&(mw.alu_source),0x0fL);
		break;
        case 7:
                request("ALU_DEST = ",&(mw.alu_dest),0x0fL);
		break;
        case 8:
                request("AD_BUS = ",&(mw.ad_bus),3L);
		break;
        case 9:
                request("ALU_FUNC = ",&(mw.alu_func),0x1fL);
		break;
        case 10:
                request("CIN = ",&(mw.cin),3L);
		break;
        case 11:
                request("INC_PC = ",&(mw.inc_pc),1L);
		break;
        case 12:
                request("EN_SR = ",&(mw.en_sr),1L);
		break;
        case 13:
                request("R_W = ",&(mw.r_w),1L);
		break;
        case 14:
                request("VMA = ",&(mw.vma),1L);
		break;
        default:
		break;
      }
   }
}
