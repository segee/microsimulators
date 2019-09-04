/*************************************************************************
*                          Micro3.h                                      *
**************************************************************************
*  Contains all definitions and data structures unique to board 3.
*************************************************************************/

#define MEMSIZE 2048         /* 13 bits (8k) of address are available for  */
#define HIGHMEM (0x7ff)      /* ram and rom but only 11 bits (2k) are used */
#define ADDSIZE 8192
#define HIGHADD (0x1fff)

/*------------------------------------------------------------------------
*                    board externals
*-----------------------------------------------------------------------*/
extern int leds;                          /* LED output (register)   */
extern unsigned char *memory;             /* ram and rom             */

/*-----------------------------------------------------------------------
*                     other globals
*----------------------------------------------------------------------*/

extern int cmicro;                   /* latch for alu C8 bit         */
extern int cin_bit;                  /* active low alu carry in bit  */
extern int c4_bit;                   /* half_carry from alu          */
extern int c8_bit;                   /* full_carry from alu          */
extern int z_bit;                    /* zero-detect bit              */
extern int n_bit;                    /* negative detect bit          */

/*-----------------------------------------------------------------------
*                    registers and busses
*----------------------------------------------------------------------*/
extern myint data_in;                  /* data in register             */
extern myint data_bus;                 /* data bus                     */
extern myint address_bus;              /* address bus (16 bits)        */

extern myint acc;                      /* accumulator                  */
extern myint sr;                       /* status register              */
extern myint sph;                      /* stack pointer high           */
extern myint spl;                      /* stack pointer low            */
extern myint marh;                     /* memory address register high */
extern myint marl;                     /* memory address register low  */
extern myint pch;                      /* program counter high         */
extern myint pcl;                      /* program counter low          */

extern myint a_bus,b_bus,f_bus;        /* alu busses                   */

/*-----------------------------------------------------------------------
*                    micro word structure
*----------------------------------------------------------------------*/
extern struct micro_word
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

extern struct micro_word mw;             /* current micro_word */
extern struct micro_word tgm,stm;        /* trigger & store mw specs */

extern struct trace_data
{
   myint          mad;              /* address of executing microword */
   myint          tick;             /* system clock (in cycles)       */
   myint          reg[9];           /* acc,sr,sph/l,marh/l,pch/l,data */
   myint          bus[5];           /* a, b, f, data, and address bus */
   myint          mpc;              /* micro_program counter          */
   myint          cmicro;           /* cmicro latch                   */
   myint          leds;             /* led state                      */
};

extern struct trace_data tr[TMAX];       /* trace_data array               */
extern struct trace_data tg;             /* trigger_on state spec          */
extern struct trace_data st;             /* store_on state spec            */
