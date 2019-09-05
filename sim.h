/********************************************************************
*                              Sim.h
*********************************************************************
*  Contains all global definitions for the microsimulators.
*********************************************************************/

#include "version.h"

#include <stdio.h>            /* standard input and output          */
#include <string.h>           /* string handling functions          */

/********************************************************************
*                 Platform-dependent section.
*           (#includes and #defines for file I/O)
*********************************************************************/

/*----------------------------------------------------------------
*                        VAX
*----------------------------------------------------------------*/
#ifdef VAX                           /* VAX (VMS) version */
#include <unixio.h>
#include <file.h>
#include <iodef.h>
#include <ssdef.h>
#include <descrip.h>
#define PLATFORM "VMS (VAX)"
#define TMAX          256     /* number of trace states stored      */
#define myint int
#define myunsigned unsigned int
#define WRITE_ARGS O_CREAT|O_RDWR,0000
#define READ_ARGS O_RDONLY,0

#define hotkey(V)              \
   (V)=vax_hotkey();               /* This routine lives in vaxkey.c */
#endif                            /* conditionally included in sim.c */

/*----------------------------------------------------------------
*                        MSDOS
*----------------------------------------------------------------*/
#if defined(__MSDOS__)|| defined(MSDOS)  /* MSDOS (Turbo C) version */
#include <fcntl.h>
#include <sys\stat.h>
#include <stdlib.h>
#define PLATFORM "MS_DOS PC"
#define TMAX          180     /* number of trace states stored      */
#define myint long
#define myunsigned unsigned long
#define WRITE_ARGS O_CREAT|O_RDWR|O_BINARY,S_IREAD|S_IWRITE
#define READ_ARGS O_RDONLY|O_BINARY

#define hotkey(V)            \
     if(((V)=kbhit())!=0)    \
        (V)=read(0,hot_buffer,256)
#endif

/*----------------------------------------------------------------
*                        Macintosh
*----------------------------------------------------------------*/
#ifdef THINK_C                          /* Think C Mac version    */
#include <unix.h>
#include <fcntl.h>
#include <stdlib.h>
#define PLATFORM "Macintosh"
#define TMAX          180     /* number of trace states stored    */
#define myint long
#define myunsigned unsigned long
#define WRITE_ARGS O_CREAT|O_RDWR
#define READ_ARGS O_RDONLY

#define hotkey(V)      \
   (V)=mac_hotkey()           /* conditionally included in sim.c */

#endif

/*----------------------------------------------------------------
*                        UNIX
*----------------------------------------------------------------*/
#ifndef WRITE_ARGS                     /* UNIX standard version   */
#include <fcntl.h>
#include <stdlib.h>
#define PLATFORM "for UNIX "
#define TMAX          256        /* number of trace states stored */
#define myint long int                  /* assume int > 16 bits */
#define myunsigned unsigned  long
#define WRITE_ARGS O_CREAT|O_RDWR,0666
#define READ_ARGS O_RDONLY

#define hotkey(V)              \
   fcntl(0,F_SETFL,O_NDELAY);  \
   (V)=read(0,hot_buffer,256); \
   fcntl(0,F_SETFL,0)
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ The macro defined above implements a hotkey on these systems. +
+ It returns non-zero if [return] or [enter] key was pressed.   +
+                                                               +
+ Usage:     hotkey(int_flag);                                  +
+            if(int_flag>0) { Keypressed.code }                 +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


/********************************************************************
*
*                 Platform-independent section.
*
*********************************************************************/

#define TRUE            1     /* boolean values                     */
#define FALSE           0
#define ANY    0xffffffff     /* 'anyvalue' flag for comparisons    */
#define N             100     /* maximum size of strings            */
#define INTSIZE        16     /* assume 16-bit ints for cstore      */
#define CS_INT_WIDTH    4     /* cs_word is 4 ints wide             */
#define CS_BIT_WIDTH   64     /* max cs_word size is 64 usable bits */
#define MAX_CS_WORDS 4096     /* maximum # of cs microwords         */
#define MAX_BREAKS     16     /* maximum number of breakpoints      */
#define MAXLINES       20     /* number of data lines per screen    */
#define ram             0
#define rom             1
#define lnl if (logging) fprintf(logfile,"\n")

/********************************************************************
 *
 * Global variables and functions declared here but defined elsewhere.
 *
 ********************************************************************/

/*-------------------------------------------------------------------
 *  These are defined in sim.c:
 *------------------------------------------------------------------*/
    extern int myprintf();        /* print to log file if logging   */
    extern int myscanf();         /* print to log file if logging   */
    extern FILE *logfile;         /* session log file handle        */
    extern int logging;           /* session logging flag           */
    extern unsigned *control_store;  /* control store (to allocate) */
    extern char hot_buffer[];     /* buffer area for hotkey macro   */
    extern myint timer;           /* elapsed clock cycles           */
    extern int logical_data;      /* toggle for data representation */

    extern int whitespace();      /* checks for spaces, etc.        */
    extern char next_command();   /* returns the next command char  */
    extern int last_command();    /* is this last command in string?*/
    extern int more();            /* unix more function             */
    extern int vax_hotkey();      /* VAX hotkey function            */

                      /* also main(), menus, and help functions.    */

/*-------------------------------------------------------------------
 *  These are defined in csio.c:
 *------------------------------------------------------------------*/

    extern int current_index;     /* counter value (micro_address)  */
    extern int width;             /* effective cs_word width [bytes]*/
    extern int get_hex();         /* syntax-proof scanf %x format   */
    extern int get_dec();         /* syntax-proof scanf %d format   */
    extern myint get_raw_hex();   /* %x input syntax checker        */
    extern myint get_raw_dec();   /* %d input syntax checker        */
    extern int edit_cs();         /* edit the control store bits    */
    extern int load_cs();         /* load the cs from an object file*/
    extern int display_cs();      /* display the non-zero cs_words  */
    extern int clear_cs();        /* zero out the control store     */
    extern int store_cs();        /* save the cs to an object file  */

/*-------------------------------------------------------------------
 *  These are defined in micro.c:
 *------------------------------------------------------------------*/
    extern int request();         /* gets a value from the user     */
    extern int requestit();       /* prints message & calls request */
    extern int showoff();         /* spec display support           */

/*-------------------------------------------------------------------
 *  These are defined in micro?.c:
 *------------------------------------------------------------------*/
    extern int BOARD;             /* board number                   */
    extern int last_index;        /* latch for current_index;       */

     /* also microword and trace structures, and globals for sim?.c */

    extern int load_mw();         /* load the micro_word from cs    */
    extern int store_mw();        /* store the mw to cs             */
    extern int display_mw();      /* display a microword            */
    extern int edit_mw();         /* edit the mw fields             */

/*-------------------------------------------------------------------
 *  These are defined in trace.c:
 *------------------------------------------------------------------*/
    extern int tndx;              /* globals for tracing package    */
    extern int trigptr;
    extern int tracing;
    extern int triggered;
    extern int step_count;

    extern int inc_tndx();        /* increment trace_index          */
    extern int clear_trace();     /* clear trace_data array         */
    extern int start_trace();     /* start tracing                  */
    extern int stop_trace();      /* stop tracing                   */
    extern int savestate();       /* state matches store specs?     */
    extern int trigstate();       /* state matches trigger state?   */
    extern int trace();           /* processes state for tracing    */
    extern int set_trigger();     /* configures trigger state       */
    extern int set_store();       /* configures store_on state      */
    extern int show_config();     /* display trace specs            */

/*-------------------------------------------------------------------
 *  These are defined in trace?.c:
 *------------------------------------------------------------------*/
    extern int store_trace();     /* store the current state        */
    extern int reset_trace();     /* reset trace specs to defaults  */
    extern int matchstate();      /* current state matches specs?   */
    extern int set_spec();        /* configure a spec state         */
    extern int show_spec();       /* display a spec state           */
    extern int show_trace();      /* displays trace data            */
    extern int save_trace();      /* saves trace data to a file     */

/*-------------------------------------------------------------------
 *  These are defined in sim?.c:
 *------------------------------------------------------------------*/
    extern int set_leds();        /* display the clock, MPC, LEDs   */
    extern int step_clock();      /* step the master clock          */
    extern int reset_board();     /* reset the board                */
    extern int set_inputs();      /* set the inputs (if they exist) */
    extern int load_rom();        /* loads the macrocode file if any*/
    extern int display_ram();     /* display RAM                    */
    extern int display_rom();     /* display ROM                    */
    extern int edit_ram();        /* edit RAM                       */
    extern int edit_rom();        /* edit ROM                       */
    extern int store_rom();       /* save rom to a file             */
    extern int display_regs();    /* display state (regs,busses,..) */
/* added by segee much later */
    extern int read_inputs_from_file();
    extern int write_inputs_to_file();
    extern int write_inputs_to_file();
