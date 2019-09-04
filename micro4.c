/********************************************************************
*                          Micro4.c                                 *
*********************************************************************
* Contains the board 4 data structures and microword access routines.
********************************************************************/

#include "sim.h"
#include "micro.c"

int BOARD = 4;
int last_index = 0;           /* latch for current_index      */

/*-------------------------------------------------------------------
*                    board externals
*------------------------------------------------------------------*/
unsigned int *memory;       /* 16-bit ram and rom for board 4      */
unsigned pitin = 0;         /* pit switch inputs  (port B)         */
unsigned pitout = 0;        /* pit led outputs    (port A)         */
int ram_delay = 4;          /* memory access times in clock cycles */
int rom_delay = 4;
int pit_delay = 4;
int prescaler;
struct pits
{
   unsigned char reg[32];   /* pgcr - tsr .. pit registers         */
};
struct pits pit;

/*------------------------------------------------------------------
*                 control signals and state bits
*------------------------------------------------------------------*/

unsigned ready;                    /* ready (dtack) [low]          */
unsigned busreq;                   /* bus request [low]            */
unsigned bgrant;                   /* bus grant [high]             */
unsigned vma;                      /* valid memory access [low]    */
unsigned rw;                       /* read [high] write [low]      */
unsigned iack;                     /* interrupt acknowledge [low]  */
unsigned intreq;                   /* interrupt request [low]      */
unsigned hung;                     /* flag                         */
unsigned cc;                       /* ms condition bit [low]       */
unsigned cin;                      /* carry input to the alu       */
unsigned full;                     /* sequencer stack full flag    */
unsigned ssf;                      /* alu shift function line      */
unsigned sio0,sio8;                /* alu shift control lines      */
unsigned asel,bsel,csel;           /* alu register select lines    */
unsigned zbit,nbit,vbit,cbit;      /* alu status lines             */
unsigned v_latch;                  /* divide overflow flip-flop    */

/*------------------------------------------------------------------
*                    registers and busses
*------------------------------------------------------------------*/
myunsigned ir;                   /* instruction register           */
myunsigned mar;                  /* memory address register        */
myunsigned din;                  /* data in register               */
myunsigned dout;                 /* data out register              */
myunsigned microstatus;          /* microstatus register           */
myunsigned macrostatus;          /* macrostatus register           */
unsigned mpc;                    /* microaddress (cs address)      */
myunsigned data_bus;             /* data bus                       */
myunsigned address_bus;          /* address bus                    */
myunsigned y_bus;                /* alu y-bus                      */
myunsigned a_bus;                /* alu da-bus                     */
myunsigned b_bus;                /* alu db-bus                     */
myunsigned r_bus;                /* alu internal r-bus             */
myunsigned s_bus;                /* alu internal s-bus             */
myunsigned mq;                   /* alu internal MQ register       */
myunsigned r[16];                /* alu internal general registers */

/*------------------------------------------------------------------
*                   microsequencer structure
*------------------------------------------------------------------*/
struct micro_sequencer
{
   myunsigned rc;                /* register-counter               */
   myunsigned sp;                /* stack pointer                  */
   myunsigned stack[9];          /* stack     (12 bits x 9)        */
   myunsigned mpc;               /* microprogram counter-register  */
   myunsigned data;              /* ms data input   (12 bits)      */
   myunsigned inst;              /* ms instruction  (4 bits)       */
};

struct micro_sequencer ms =
{
   0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/*------------------------------------------------------------------
*                    micro word structure
*------------------------------------------------------------------*/
struct micro_word
{
   myint micro_ad;
   myint seq;
   myint rld;
   myint cond;
   myint sr_func;
   myint sr_source;
   myint a_source;
   myint b_source;
   myint c_source;
   myint we;
   myint alu_func;
   myint cin;
   myint shift_cond;
   myint mem_access;
   myint hibernate;
   myint ld_mar;
   myint ld_ir;
   myint ld_dout;
};

struct micro_word mw =            /* current micro_word */
{
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

struct micro_word tgm,stm;        /* trigger & store mw specs */

/*---------------------------------------------------------------
*                     trace data array
*---------------------------------------------------------------*/
struct trace_data
{
   myunsigned mad;            /* address of executing microword */
   myunsigned tick;           /* system clock (in cycles)       */
   myunsigned ir;             /* instruction register           */
   myunsigned mar;            /* memory address register        */
   myunsigned din;            /* data in register               */
   myunsigned dout;           /* data out register              */
   myunsigned microstatus;    /* microstatus register           */
   myunsigned macrostatus;    /* macrostatus register           */
   myunsigned rbus;           /* r bus                          */
   myunsigned sbus;           /* s bus                          */
   myunsigned ybus;           /* y bus                          */
   myunsigned dbus;           /* data bus                       */
   myunsigned abus;           /* address bus                    */
   myunsigned mpc;            /* micro_program counter (csad)   */
   myunsigned pitin;          /* pit switch inputs              */
   myunsigned pitout;         /* pit led outputs                */
   myunsigned ready;          /* ready line                     */
   myunsigned busreq;         /* bus request line               */
   myunsigned bgrant;         /* bus grant line                 */
   myunsigned vma;            /* valid memory access line       */
   myunsigned rw;             /* read-write line                */
   myunsigned iack;           /* interrupt acknowledge line     */
   myunsigned intreq;         /* interrupt request line         */
   myunsigned ssf;            /* alu shift control lines        */
   myunsigned sio0;           /*       ditto                    */
   myunsigned sio8;           /*       ditto                    */
   myunsigned asel;           /* alu a select lines             */
   myunsigned bsel;           /* alu b select lines             */
   myunsigned csel;           /* alu c select lines             */
   myunsigned cin;            /* alu cin line                   */
   myunsigned cc;             /* sequencer cc line              */
};

struct trace_data tr[TMAX];   /* trace_data array               */
struct trace_data tg;         /* trigger_on state spec          */
struct trace_data st;         /* store_on state spec            */

/****************************************************************
 *       load_mw()   loads the microword from cs
 ****************************************************************/

int load_mw(cindex)
unsigned cindex;
{
   unsigned windex;
   windex=cindex*CS_INT_WIDTH;
   mw.micro_ad   = getbits(control_store+windex,0,12);
   mw.seq        = getbits(control_store+windex,12,4);
   mw.rld        = getbits(control_store+windex,16,1);
   mw.cond       = getbits(control_store+windex,17,5);
   mw.sr_func    = getbits(control_store+windex,22,2);
   mw.sr_source  = getbits(control_store+windex,24,1);
   mw.a_source   = getbits(control_store+windex,25,6);
   mw.b_source   = getbits(control_store+windex,31,7);
   mw.c_source   = getbits(control_store+windex,38,5);
   mw.we         = getbits(control_store+windex,43,1);
   mw.alu_func   = getbits(control_store+windex,44,9);
   mw.cin        = getbits(control_store+windex,53,2);
   mw.shift_cond = getbits(control_store+windex,55,3);
   mw.mem_access = getbits(control_store+windex,58,2);
   mw.hibernate  = getbits(control_store+windex,60,1);
   mw.ld_mar     = getbits(control_store+windex,61,1);
   mw.ld_ir      = getbits(control_store+windex,62,1);
   mw.ld_dout    = getbits(control_store+windex,63,1);
}

/****************************************************************
 *       store_mw()   stores the microword into cs
 ***************************************************************/

int store_mw(cindex)
unsigned cindex;
{
   unsigned windex;
   windex=cindex*CS_INT_WIDTH;
   putbits(control_store+windex,0,12,(unsigned) mw.micro_ad);
   putbits(control_store+windex,12,4,(unsigned) mw.seq);
   putbits(control_store+windex,16,1,(unsigned) mw.rld);
   putbits(control_store+windex,17,5,(unsigned) mw.cond);
   putbits(control_store+windex,22,2,(unsigned) mw.sr_func);
   putbits(control_store+windex,24,1,(unsigned) mw.sr_source);
   putbits(control_store+windex,25,6,(unsigned) mw.a_source);
   putbits(control_store+windex,31,7,(unsigned) mw.b_source);
   putbits(control_store+windex,38,5,(unsigned) mw.c_source);
   putbits(control_store+windex,43,1,(unsigned) mw.we);
   putbits(control_store+windex,44,9,(unsigned) mw.alu_func);
   putbits(control_store+windex,53,2,(unsigned) mw.cin);
   putbits(control_store+windex,55,3,(unsigned) mw.shift_cond);
   putbits(control_store+windex,58,2,(unsigned) mw.mem_access);
   putbits(control_store+windex,60,1,(unsigned) mw.hibernate);
   putbits(control_store+windex,61,1,(unsigned) mw.ld_mar);
   putbits(control_store+windex,62,1,(unsigned) mw.ld_ir);
   putbits(control_store+windex,63,1,(unsigned) mw.ld_dout);
}

/****************************************************************
 *      display_mw()   displays the current microword
 ***************************************************************/

int display_mw()
{
   char *head1="LdDO/IR/MAR Hib MemAc ShCnd Cin AlFun WE";
   char *head2=" C/B/ASrc SSrc SFun Cnd RLd Seq MicAd\n";

   myprintf("Microword:\n");
   myprintf("%s%s",head1,head2);
   myprintf("   %01lx  %01lx  %01lx   %01lx    %01lx     %01lx    %01lx   %03lx   %01lx",
             mw.ld_dout,mw.ld_ir,mw.ld_mar,mw.hibernate,mw.mem_access,
             mw.shift_cond,mw.cin,mw.alu_func,mw.we);
   myprintf(" %02lx %02lx %02lx   %01lx    %01lx   %02lx  %01lx   %01lx   %03lx\n",
             mw.c_source,mw.b_source,mw.a_source,mw.sr_source,mw.sr_func,
             mw.cond,mw.rld,mw.seq,mw.micro_ad);
}

/***************************************************************
 *       edit_mw_menu() puts up a menu of edit choices 
 **************************************************************/

int edit_mw_menu()
{
   int mwfield;

   display_mw();
   myprintf("\n");
   myprintf("                 === EDIT MICROWORD MENU ===              \n\n");
   myprintf("   1] Micro_Ad         7] A_Source         13] Shift_Cond   \n");
   myprintf("   2] Seq              8] B_Source         14] Mem_Access   \n");
   myprintf("   3] RLD              9] C_Source         15] Hibernate    \n");
   myprintf("   4] Cond            10] WE               16] LD_MAR       \n");
   myprintf("   5] SR_Func         11] ALU_Func         17] LD_IR        \n");
   myprintf("   6] SR_Source       12] Cin              18] LD_DOut      \n");
   myprintf("\n");
   myprintf("                       0] Exit (Done)                       \n");
   myprintf("\n");
   myprintf("Your selection [0-18]? "); 
   mwfield = get_dec();
   if (logging) fprintf(logfile,"%d\n",mwfield);
   while(mwfield < 0 || mwfield > 18)
   {
      myprintf("Please choose a number between 0 and 18: ");
      mwfield = get_dec();
      if (logging) fprintf(logfile,"%d\n",mwfield);
   }
   return(mwfield);
}

/***************************************************************
 *       edit_mw()  changes the microword
 **************************************************************/

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
		break;
	case 2:
                request("SEQ = ",&(mw.seq),0x0fL);
		break;
	case 3:
                request("RLD = ",&(mw.rld),1L);
		break;
	case 4:
                request("COND = ",&(mw.cond),0x1fL);
		break;
	case 5:
                request("SR_FUNC = ",&(mw.sr_func),3L);
		break;
	case 6:
                request("SR_SOURCE = ",&(mw.sr_source),1L);
		break;
	case 7:
                request("A_SOURCE = ",&(mw.a_source),0x3fL);
		break;
	case 8:
                request("B_SOURCE = ",&(mw.b_source),0x7fL);
		break;
	case 9:
                request("C_SOURCE = ",&(mw.c_source),0x1fL);
		break;
	case 10:
                request("WE = ",&(mw.we),1L);
		break;
	case 11:
                request("ALU_FUNC = ",&(mw.alu_func),0x1ffL);
		break;
	case 12:
                request("CIN = ",&(mw.cin),3L);
		break;
	case 13:
                request("SHIFT_COND = ",&(mw.shift_cond),7L);
		break;
	case 14:
                request("MEM_ACCESS = ",&(mw.mem_access),3L);
		break;
	case 15:
                request("HIBERNATE = ",&(mw.hibernate),1L);
		break;
	case 16:
                request("LD_MAR = ",&(mw.ld_mar),1L);
		break;
	case 17:
                request("LD_IR = ",&(mw.ld_ir),1L);
		break;
	case 18:
                request("LD_DOUT = ",&(mw.ld_dout),1L);
		break;
        default:
		break;
      }
   }
}
