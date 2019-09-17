/**********************************************************************
*                             Micro2.h                                *
***********************************************************************
*  Contains all definitions and data structures unique to board 2.
**********************************************************************/

extern unsigned char *memory;         /* not used in board 2          */

/*--------------------------------------------------------------------
*                   external  variables
*-------------------------------------------------------------------*/
extern myint op_code;                 /* op code input switches       */
extern int go_button;                 /* go button status             */
extern myint ia_reg;                  /* data input register a        */
extern myint ib_reg;                  /* data input register b        */
extern myint ia_switches;             //added by segee for external board
extern myint ib_switches;

extern myint f_reg;                   /* result register f            */

extern myint ta_reg;                  /* temporary register a         */
extern myint tb_reg;                  /* temporary register b         */
extern myint a_bus,b_bus,f_bus;       /* alu busses                   */

extern int z_bit;                   /* zero-detect                  */
extern int n_bit;                   /* negative result              */
extern int c4_bit;                  /* half-carry bit               */
extern int c8_bit;                  /* full carry bit               */
extern int zp_bit;                  /* zero-detect (positive logic) */


extern struct micro_word
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

extern struct micro_word mw;         /* current micro_word */
extern struct micro_word tgm,stm;   /* trigger & store mw specs   */

extern struct trace_data
{
   myint mad;                   /* address of executing micro_word    */
   myint tick;                  /* system clock (in cycles)           */
   myint insw[3];               /* input switches: ia, ib, opcode     */
   myint reg[3];                /* ia,ib,ta,tb,f-registers            */
   myint bus[3];                /* a,b,f-busses                       */
   myint mpc;                   /* micro_program counter              */
   int   bit[6];                /* status bits and go button state    */
   int   rep;                   /* data representation state          */
};

extern struct trace_data tr[TMAX]; /* trace data array                   */
extern struct trace_data tg,st;    /* trigger_on & store_on state specs  */

