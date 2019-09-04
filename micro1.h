/*************************************************************************
*                          Micro1.h                                      *
**************************************************************************
*  Contains all definitions and data structures unique to board 1.
*************************************************************************/

extern int sw[4];                    /* board1 input switches */
extern unsigned char *memory;        /* not used in board 1   */

extern struct micro_word
{
  myint micro_ad;
  myint bop;
  myint pol;
  myint cond;
  myint data;
};

extern struct micro_word mw;         /* current micro_word */
extern struct micro_word tgm,stm;    /* trigger and store mw specs */

extern struct trace_data
{
  myint    mad;                /* address of executing microword             */
  myint    tick;               /* system clock (in cycles)                   */
  myint    insw[4];            /* input switches                             */
  myint    mpc;                /* micro_program counter                      */
};

extern struct trace_data tr[TMAX];   /* trace_data array                    */
extern struct trace_data tg;         /* trigger_on state specs              */
extern struct trace_data st;         /* store_on state specs                */

