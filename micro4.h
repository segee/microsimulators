/*************************************************************************
*                          Micro4.h                                      *
**************************************************************************
*  Contains all definitions and data structures unique to board 4.
*************************************************************************/

#define MEMSIZE 8192              /* 13-bits of address for ram & rom */

extern unsigned int *memory;           /* ram and rom for board 4  */

/*------------------------------------------------------------------------
*                    board extern
*-----------------------------------------------------------------------*/
extern unsigned pitin;
extern unsigned pitout;
extern int ram_delay;
extern int rom_delay;
extern int pit_delay;
extern int prescaler;
extern struct pits
{
   unsigned char reg[32];
};
extern struct pits pit;

/*-----------------------------------------------------------------------
*                 control signals and state bits
*----------------------------------------------------------------------*/

extern unsigned ready;
extern unsigned busreq;
extern unsigned bgrant;
extern unsigned vma;
extern unsigned rw;
extern unsigned iack;
extern unsigned intreq;
extern unsigned hung;
extern unsigned cc;
extern unsigned cin;
extern unsigned full;
extern unsigned ssf;
extern unsigned sio0,sio8;
extern unsigned asel,bsel,csel;
extern unsigned zbit,nbit,vbit,cbit;
extern unsigned v_latch;

/*-----------------------------------------------------------------------
*                    registers and busses
*----------------------------------------------------------------------*/
extern myunsigned ir;
extern myunsigned mar;
extern myunsigned din;
extern myunsigned dout;
extern myunsigned microstatus;
extern myunsigned macrostatus;
extern unsigned   mpc;
extern myunsigned data_bus;
extern myunsigned address_bus;
extern myunsigned y_bus;
extern myunsigned a_bus;
extern myunsigned b_bus;
extern myunsigned r_bus;
extern myunsigned s_bus;
extern myunsigned mq;
extern myunsigned r[16];

/*-----------------------------------------------------------------------
*                   microsequencer structure
*----------------------------------------------------------------------*/
extern struct micro_sequencer
{
   myunsigned rc;
   myunsigned sp;
   myunsigned stack[9];
   myunsigned mpc;
   myunsigned data;
   myunsigned inst;
};

extern struct micro_sequencer ms;

/*-----------------------------------------------------------------------
*                    micro word structure
*----------------------------------------------------------------------*/
extern struct micro_word
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

extern struct micro_word mw;

extern struct micro_word tgm,stm;

/*-----------------------------------------------------------------------
*                     trace data array
*----------------------------------------------------------------------*/
extern struct trace_data
{
   myunsigned mad;
   myunsigned tick;
   myunsigned ir;
   myunsigned mar;
   myunsigned din;
   myunsigned dout;
   myunsigned microstatus;
   myunsigned macrostatus;
   myunsigned rbus;
   myunsigned sbus;
   myunsigned ybus;
   myunsigned dbus;
   myunsigned abus;
   myunsigned mpc;
   myunsigned pitin;
   myunsigned pitout;
   myunsigned ready;
   myunsigned busreq;
   myunsigned bgrant;
   myunsigned vma;
   myunsigned rw;
   myunsigned iack;
   myunsigned intreq;
   myunsigned ssf;
   myunsigned sio0;
   myunsigned sio8;
   myunsigned asel;
   myunsigned bsel;
   myunsigned csel;
   myunsigned cin;
   myunsigned cc;
};

extern struct trace_data tr[TMAX];
extern struct trace_data tg;
extern struct trace_data st;

