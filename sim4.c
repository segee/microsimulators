/********************************************************************
*                           Sim4.c                                  *
*********************************************************************
*  This is the board 4 microsimulator code.
********************************************************************/

#include "sim.h"
#include "micro4.h"

/*******************************************************************
*                     initialize_memory()                          *
********************************************************************
*  Allocate memory for the ram and rom.
********************************************************************/

int initialize_memory()
{
   memory = (unsigned int *) calloc(MEMSIZE*2, sizeof(int));      
   if (memory == NULL)
      return(0);
   else
      return(1);
}

/*******************************************************************
*                       release_memory()                           *
********************************************************************
*  Free memory previously allocated to ram and rom.
********************************************************************/

int release_memory()
{
   free(memory);
}

/*******************************************************************
*                         load_rom()                               *
********************************************************************
*  Reads a file of macrocode into the rom array.
********************************************************************/

int load_rom(fname)
char *fname;
{
   int i,j,mac;
   unsigned char c[4];
   char macname[N];

   strcpy(macname,fname);

/* if the macrocode file name has no extension, add one [.mac] */

   i=0;
   while(macname[i] != '\0' && macname[i] != '.')
      i++;
   if(macname[i]=='\0')
      strcat(macname,".mac");

/* open the macrocode file for input as a binary file */

   if((mac=open(macname,READ_ARGS))== -1)
   {
      myprintf("Error opening macrocode file %s.\n",macname);
      return(-1);
   }

/* if open was successful, zero rom, load rom, and close rom file.  */

   myprintf("Loading... \n");
   for(i=0;i<MEMSIZE;i++)
      *(memory+(rom*MEMSIZE)+i)=0;
   for(i=0;(read(mac,(char *) c,2)>0)&&(i<MEMSIZE);i++)
      *(memory+(rom*MEMSIZE)+i)=(c[0]*256 + c[1])&0xffff;
   close(mac);
   myprintf("Done.\n");
   return(0);
}

/*********************************************************************
*                         store_rom()                                *
**********************************************************************
*   Write a file of macrocode from the rom array.
**********************************************************************/

int store_rom(fname)
char *fname;
{
   int i,j,mac;
   unsigned char c[4];
   char macname[N];

   strcpy(macname,fname);

/* if the macrocode file name has no extension, add one [.mac] */

   i=0;
   while(macname[i] != '\0' && macname[i] != '.')
      i++;
   if(macname[i]=='\0')
      strcat(macname,".mac");
   myprintf("Writing... "); lnl;

/* open the macrocode file for output as a binary file */

   if((mac=open(macname,WRITE_ARGS))== -1)
   {
      myprintf("Error opening macrocode file %s.\n",macname);
      return(-1);
   }

/* if open was successful, write out the rom, and close rom file.  */

   for(i=0;i<MEMSIZE;i++)
   {
      c[0]=((*(memory+(rom*MEMSIZE)+i))>>8)&0xff;
      c[1]=(*(memory+(rom*MEMSIZE)+i))&0xff;
      if(write(mac,(char *) c,2)<=1) break;
   }
   close(mac);
   myprintf("Done.\n");
   return(0);
}

/******************************************************************
*                       edit_rom()                                *
*******************************************************************
*  Allow user to modify ROM.
******************************************************************/

int edit_rom()
{
   edit_memory(rom);
}

/******************************************************************
*                       edit_ram()                                *
*******************************************************************
*  Allow user to modify RAM.
******************************************************************/

int edit_ram()
{
   edit_memory(ram);
}

/******************************************************************
*                       edit_memory()                             *
*******************************************************************
*  Allow user to modify RAM or ROM 
******************************************************************/

int edit_memory(mem_type)
int mem_type;
{
   int index;
   myint temp;
   int valid=0;
   int done=0;
   char ch;

   if(mem_type==ram)
   {
      myprintf("Editing RAM...\n");
      myprintf("Note: Addresses are relative to the start of RAM.\n");
      myprintf("    (RAM address 0000 is ABSOLUTE address E000)\n");
   }
   else
      myprintf("Editing ROM...\n");

   while(!valid)
   {
      myprintf("   Start address [hex]: ");
      index=get_hex();
      if (logging) fprintf(logfile,"%x\n",index);
      valid=((0<=index)&&(index<MEMSIZE));
      if(!valid)
         myprintf("\n   Invalid address. Use 0-%03x.\n\n",(MEMSIZE-1));
   }
   myprintf("\nEnter a period (.) when done.\n\n");

   while(!done)
   {
      myprintf("   [%04x] old: %04x   new: ",index,
                   *(memory+(mem_type*MEMSIZE)+index)&0xffff);
      done=((temp=get_raw_hex())==(-1));
      if(!done)
      {
         if (logging) fprintf(logfile,"%lx\n",temp);
         *(memory+(mem_type*MEMSIZE)+(index++))=temp&0xffff;
      }
      if(index>=MEMSIZE)
         done = TRUE;
   }
   myprintf("\n");
}

/******************************************************************
*                    display_rom()                                *
*******************************************************************
*  Display the contents of ROM
******************************************************************/

int display_rom()
{
   display_memory(rom);
}

/******************************************************************
*                    display_ram()                                *
*******************************************************************
*  Display the contents of RAM
******************************************************************/

int display_ram()
{
   display_memory(ram);
}

/******************************************************************
*                    display_memory()                             *
*******************************************************************
* Display the contents of RAM or ROM 
******************************************************************/

int display_memory(mem_type)
int mem_type;
{
   int i,j,first,last;
   int valid = FALSE;

   if(mem_type==ram)
   {
      myprintf("Addresses are offsets from the start of RAM.\n");
      myprintf(" (RAM address 0000 is ABSOLUTE address E000)\n");
   }
   while(!valid)
   {
      myprintf("Start address [hex]: "); 
      first=get_hex();
      if (logging) fprintf(logfile,"%x\n",first);
      myprintf("End address [hex]:   ");
      last=get_hex();
      if (logging) fprintf(logfile,"%x\n",last);
      valid=((0<=first)&&(first<=last)&&(last<MEMSIZE));
      if(!valid)
         myprintf("\nInvalid address. Use 0-%03x.\n\n",(MEMSIZE-1));
   }

   if(mem_type==ram)
      myprintf("RAM contents:\n");
   else
      myprintf("ROM contents:\n");

   for(i=first;i<=last;)
   {
      if(((i-first)%8)==0)
         myprintf("[%04x] ",i);
      for(j=0;(j<8)&&(i<=last);j++,i++)
         myprintf("%04x ",*(memory+(mem_type*MEMSIZE)+i));
      if((((i-first)%8)==0)||(i>=last))
         myprintf("\n");
      if(((i-first)%160)==0)
         if(more())
         {
            myprintf("Cancelled!\n\n");
            return(0);
         }
   }
}

/*********************************************************************
*                          set_leds()                                *
**********************************************************************
*  Display the switches, leds, clock, and micro-address.
*********************************************************************/

int set_leds()
{
   int i;

   myprintf("Starting CS Address: [%04x]   ",last_index);
   myprintf("Clock: [%04ld]   Ending CS Address: ",timer);
   myprintf("[%04x] \n",current_index);
   myprintf("         Switches:  [");
   for(i=7;i>=0;i--)
      myprintf("%01x",(pitin>>i)&1);
   myprintf("]  ");
   myprintf("LEDs: [");
   for(i=7;i>=0;i--)
   {
      if(((pitout>>i)&1)==0)
         myprintf("*");
      else
         myprintf("O");
   }
   myprintf("]\n");
}

/**********************************************************************
*                           step_clock()                              *
***********************************************************************
*  Step the master clock one cycle:
*      load the pipeline register (microword) from the control store.
*      perform the microsequencer function,
*      perform the alu function,
*      read/write external memory or pit.
**********************************************************************/

int step_clock()
{
   unsigned stat;
   int ssfop;
   int hibernate_delay, ii;

   if(hung) return(-1);
   load_mw(current_index);
   last_index=current_index;

/*---------------------------------------------------------
*   setup microsequencer input busses (data & inst),
*   and determine the state of cc.
*----------------------------------------------------------*/
   if(mw.seq==2)
      ms.data=(mw.micro_ad&0x0f)|((ir>>4)&0xff0);
   else
      ms.data=mw.micro_ad;

   ms.inst=mw.seq;

                    /*------------------------------------
                      microstatus reg contents:   WCNVZ
                      macrostatus reg contents:  WMCNVZ
                      where M is interrupt mask for macro
                      status register only, and W is N^V.
                      ------------------------------------*/
   switch(mw.cond)
   {
                /* microstatus branches */

      case 0: cc=(microstatus&0x04)? 0:1; break; /*  N    */
      case 1: cc=(microstatus&0x01)? 0:1; break; /*  Z    */
      case 2: cc=(microstatus&0x08)? 0:1; break; /*  C    */
      case 3: cc=(microstatus&0x09)? 0:1; break; /*  C|Z  */
      case 4: cc=(microstatus&0x02)? 0:1; break; /*  V    */
      case 5: cc=0;                       break; /* always*/
      case 6: cc=(microstatus&0x10)? 0:1; break; /*  W    */
      case 7: cc=(microstatus&0x11)? 0:1; break; /*  Z|W  */
      case 8: cc=(microstatus&0x04)? 1:0; break; /* !N    */
      case 9: cc=(microstatus&0x01)? 1:0; break; /* !Z    */
     case 10: cc=(microstatus&0x08)? 1:0; break; /* !C    */
     case 11: cc=(microstatus&0x09)? 1:0; break; /* !C|Z  */
     case 12: cc=(microstatus&0x02)? 1:0; break; /* !V    */
     case 13: cc=1;                       break; /* never */
     case 14: cc=(microstatus&0x10)? 1:0; break; /* !W    */
     case 15: cc=(microstatus&0x11)? 1:0; break; /* !Z|W  */

               /* macrostatus branches */

     case 16: cc=(macrostatus&0x04)? 0:1; break; /*  N    */
     case 17: cc=(macrostatus&0x01)? 0:1; break; /*  Z    */
     case 18: cc=(macrostatus&0x08)? 0:1; break; /*  C    */
     case 19: cc=(macrostatus&0x09)? 0:1; break; /*  C|Z  */
     case 20: cc=(macrostatus&0x02)? 0:1; break; /*  V    */
     case 21: cc=((intreq==0)&&((macrostatus&0x10)==0))? 0:1;
                         break; /* interrupt request & !M */
     case 22: cc=(macrostatus&0x20)? 0:1; break; /*  W    */
     case 23: cc=(macrostatus&0x21)? 0:1; break; /*  Z|W  */
     case 24: cc=(macrostatus&0x04)? 1:0; break; /* !N    */
     case 25: cc=(macrostatus&0x01)? 1:0; break; /* !Z    */
     case 26: cc=(macrostatus&0x08)? 1:0; break; /* !C    */
     case 27: cc=(macrostatus&0x09)? 1:0; break; /* !C|Z  */
     case 28: cc=(macrostatus&0x02)? 1:0; break; /* !V    */
     case 29: cc=1;                       break; /* never */
     case 30: cc=(macrostatus&0x20)? 1:0; break; /* !W    */
     case 31: cc=(macrostatus&0x21)? 1:0; break; /* !Z|W  */
   }

/*--------------------------------------------------------
 *       Perform the microsequencer function.
 *--------------------------------------------------------*/

   sequencer();

/*--------------------------------------------------------
*      determine the value of cin, ssf, sio0, sio8.
*---------------------------------------------------------*/

   switch(mw.cin&0x03)
   {
      case 0:  cin=(microstatus&0x08)? 1:0; break; /* micro C */
      case 1:  cin=(macrostatus&0x08)? 1:0; break; /* macro C */
      case 2:  cin=0;                       break; /*    0    */
      case 3:  cin=1;                       break; /*    1    */
   }

   ssfop = TRUE;
   switch(mw.alu_func&0x0f)                   /* if not shift op */
   {                                          /*   disable ssf.  */
      case 0x8: ssfop = FALSE; break;
      case 0xf: ssfop = FALSE; break;
      case 0x0: if((mw.alu_func&0x17)==0x00) ssfop = FALSE;
                if((mw.alu_func&0x20)==0x20) ssfop = FALSE;
                if((mw.alu_func&0x40)==0x40) ssfop = FALSE;
                if((mw.alu_func&0x80)==0x80) ssfop = FALSE;
                break;
      default:  if((mw.alu_func&0xe0)==0xe0) ssfop = FALSE;
                break;
   }

   if(ssfop == TRUE)
      switch(mw.shift_cond&0x07)              /* set ssf state  */
      {
         case 0: ssf=1;                        break;
         case 1: ssf=(microstatus&0x01)? 1:0;  break;  /*  Z */
         case 2: ssf=(microstatus&0x01)? 0:1;  break;  /* !Z */
         case 3: ssf=(microstatus&0x08)? 1:0;  break;  /*  C */
         case 4: ssf=(microstatus&0x08)? 0:1;  break;  /* !C */
         case 5: ssf=(microstatus&0x04)? 1:0;  break;  /*  N */
         case 6: ssf=(microstatus&0x04)? 0:1;  break;  /* !N */
         case 7: ssf=(microstatus&0x02)? 1:0;  break;  /*  V */
      }

   sio0=(mw.alu_func&0x100)? 0:1;              /* set sio0, sio8 */
   sio8=(sio0)? 0:1;

   switch(mw.alu_func&0xff)                    /* if not byte op */
   {                                           /*  disable both. */
      case 0x8f:
      case 0x08:
      case 0x18:
      case 0x28:
      case 0x38:
      case 0x88:
      case 0x98:
      case 0xa8:
      case 0xb8:
      case 0xc8:
      case 0xd8:
      case 0xe8:
      case 0xf8:  break;
      default:    sio0=sio8=1; break;
   }

/*----------------------------------------------------------------
*    Setup the main alu - read/write - load register loop
*      (those functions which continue during hibernation)
*      and go hibernate until memory has had time to respond.
*---------------------------------------------------------------*/

   if(mw.hibernate==0)
      hibernate_delay=1;
   else
      switch(mw.mem_access)
      {
        case 0:
        case 1: switch((address_bus>>13)&0x07)
                {
                   case 0: hibernate_delay=rom_delay; break;
                   case 4: hibernate_delay=pit_delay; break;
                   case 7: hibernate_delay=ram_delay; break;
                  default: hibernate_delay=2;         break;
                }
                break;
        case 2: hibernate_delay=pit_delay;
                break;
       default: hibernate_delay=1;
      }

   for(ii=0;ii<hibernate_delay;ii++)
      alu_rw_loop();

 /*--------------------------------------------------------------
 *   If hibernate was asserted and memory doesn't respond,
 *          the address was bad; hang the board.
 *---------------------------------------------------------------*/

   if((mw.hibernate==1)&&(ready==1))
   {
      trace();
      myprintf("\n\n\nBad Address.  You are HUNG!\n\n\n");
      hung = TRUE;
      return(-1);
   }

/*--------------------------------------------------------
 *       Load the internal alu register file.
 *--------------------------------------------------------*/

   if(mw.we==0)
      r[csel]=y_bus;

/*--------------------------------------------------------
 *       Load the status registers.
 *--------------------------------------------------------*/

   if(mw.sr_source==1)
   {
       stat=y_bus&0x03ff;              /* load from y_bus */
   }
   else                          /* load from status bits */
   {
      stat=(cbit<<3)|(nbit<<2)|(vbit<<1)|zbit;
      stat=stat|(stat<<6);
      stat=stat&0x03ef;                   /* preserve  */
      stat=stat|(macrostatus&0x0010);     /* old M bit */
   }
   stat=stat&0x03df;                   /* clear W bits */
   if((stat&0x0002)!=(stat&0x0004)>>1)
      stat=stat|0x0020;                /* set macro W  */
   if((stat&0x0080)!=(stat&0x0100)>>1)
      stat=stat|0x0400;                /* set micro W  */

   if((mw.sr_func&1)==0)               /* load desired */
      macrostatus=stat&0x3f;           /* status regs. */
   if((mw.sr_func&2)==0)
      microstatus=(stat>>6)&0x1f;

/*----------------------------------------------------------------
 *      Store tracedata and return.
 *---------------------------------------------------------------*/
   return(trace());
}

/****************************************************************
*                       alu_rw_loop()                           *
*****************************************************************
*  This is the alu_rw_loop: alu setup, alu function, read/write 
*  memory, load registers (except status and alu register file.)
*****************************************************************/
int alu_rw_loop()
{

/*--------------------------------------------------------
*      set up the alu a, b, and c select lines
*          and load the r_bus and s_bus
*---------------------------------------------------------*/

   switch(mw.a_source>>4)
   {
      case 0:   asel=(ir>>4)&0x0f;
                r_bus=r[asel];             break;
      case 1:   asel=(mw.a_source)&0x0f;
                r_bus=r[asel];             break;
      default:  if((mw.a_source&1)==0)
                {
                  asel=0;
                  r_bus=din;
                }
                else
                {
                  asel=1;
                  r_bus=(microstatus<<6)|(macrostatus&0x3f);
                }
                                            break;
   }
   switch(mw.b_source>>5)
   {
      case 0:   if(mw.b_source&0x10)
                {
                  bsel=(mw.b_source&0x0f);
                  s_bus=r[bsel];
                }
                else
                {
                  bsel=(ir&0x0f);
                  s_bus=r[bsel];
                }
                                           break;
      case 2:   bsel=0;
                s_bus=mw.micro_ad;
                if(s_bus&0x800)
                   s_bus|=0xf000;          break;
      default:  bsel=0;
                s_bus=mq;                  break;
   }
   switch(mw.c_source>>3)
   {
      case 0:   csel=(ir&0x0f);            break;
      case 1:   csel=(ir>>4)&0x0f;         break;
      default:  csel=mw.c_source&0x0f;     break;
   }

/*--------------------------------------------------------
 *       Perform the alu function.
 *--------------------------------------------------------*/

   alu();

/*---------------------------------------------------------
*          Access external devices (read/write)
*----------------------------------------------------------*/

   if(bgrant==0)
      address_bus=mar;
   ready=1;
   switch(mw.mem_access)
   {
      case 0:  vma=0; rw=0; iack=1;
               if(bgrant==0) data_bus=dout;
               break;
      case 1:  vma=0; rw=1; iack=1; 
               break;
      case 2:  vma=1; data_bus=0xffff;      /* data bus floats high */
               rw=1; iack=0;
               if(mw.hibernate==1) ready=0; 
               break;
      case 3:  
     default:  vma=1; data_bus=0xffff;      /* data bus floats high */
               rw=1; iack=1; 
               break;
   }

   if(vma==0)
      switch((address_bus>>13)&0x07)
      {
         case 0:   if(rw==1)
                      data_bus=(*(memory+(rom*MEMSIZE)+(address_bus&0x1fff)));
                   if(mw.hibernate==1)
                      ready=0;
                   break;
         case 4:   pitio();
                   if(mw.hibernate==1)
                      ready=0;
                   break;
         case 7:   if(rw==1)
                      data_bus=(*(memory+(ram*MEMSIZE)+(address_bus&0x1fff)))&0xffff;
                   else
                      (*(memory+(ram*MEMSIZE)+(address_bus&0x1fff)))=data_bus&0xffff;
                   if(mw.hibernate==1)
                      ready=0;
         default:  break;
      }

/*--------------------------------------------------------
 *       Load IR, Din, MAR, and Dout if requested.
 *        (Load Address & Data busses if WRITE.)
 *--------------------------------------------------------*/

   if((mw.mem_access==1)&&(mw.ld_ir==1)&&(bgrant==0))
      ir=data_bus;

   if(((mw.mem_access==1)||(mw.mem_access==2))&&(bgrant==0))
      din=data_bus;

   if(mw.ld_dout==1)
      dout=y_bus;

   if(mw.ld_mar==1)
      mar=y_bus;
   if(bgrant==0)
      address_bus=mar;

   if((mw.mem_access==0)&&(bgrant==0))
      data_bus=dout;

/*----------------------------------------------------------------
 *      Tick the cpu clock
 *---------------------------------------------------------------*/
   inctimer();
}

/**********************************************************************
*                           inctimer()                                *
***********************************************************************
*  Increment the system clock and decrement the pit timer if configured.
**********************************************************************/

int inctimer()
{
   int i;
   myint temp;

   timer++;
   if(pit.reg[16]==0xa1)          /* if countdown is enabled,   */
      if(--prescaler<=0)          /* decrement prescaler.       */
      {
         prescaler=31;
         temp=(((myint)pit.reg[23]<<16)|((myint)pit.reg[24]<<8)|(myint)(pit.reg[25]));
         if((temp)==0)         /* if zero, preload.          */
         {
            temp=(((myint)pit.reg[19]<<16)|((myint)pit.reg[20]<<8)|(myint)(pit.reg[21]));
            pit.reg[26]=1;
            intreq=0;
         }
         temp--;
         pit.reg[23]=(temp>>16)&0xff;
         pit.reg[24]=(temp>>8)&0xff;
         pit.reg[25]=temp&0xff;
      }
}

/**********************************************************************
*                             pitio()                                 *
***********************************************************************
*     Perform the pit I/O functions.
**********************************************************************/

int pitio()
{
   unsigned char addr;

   addr=address_bus&0x1f;

   if(rw==1)
      data_bus=pit.reg[addr];
   else
      if((addr==26)&&(data_bus&1==1))   /* if 1 is written to tsr */
      {
         pit.reg[26]=0;                       /* reset tsr */
         intreq=1;                            /* reset intreq */
      }
      else                              /* otherwise write to register */
      {
         pit.reg[addr]=data_bus&0xff;
         pit.reg[14]=pit.reg[15]=pit.reg[18]=pit.reg[22]=pit.reg[27]=0;
         pit.reg[28]=pit.reg[29]=pit.reg[30]=pit.reg[31]=0;
         pit.reg[1]&=0x7f;
         pit.reg[5]&=0xfc;
         pit.reg[16]&=0xf7;
      }

   if((pit.reg[0]==0x3f)&&   /* pgcr == 3F  */
      (pit.reg[6]==0xa0)&&   /* pacr == A0  */
      (pit.reg[7]==0xa0))    /* pbcr == A0 */
   {
      if(pit.reg[2]==0xff)   /* paddr == all outputs */
         pitout=pit.reg[8];
      if(pit.reg[3]==0)      /* pbddr == all inputs  */
         pit.reg[9]=pitin;
   }

         /*++++++++++++++++++++++++++++++++++++++++++++++++++
           Right now, all the pit does is read or write
            the internal registers and use port A & B only
            if configured as PortA-output, PortB-input;
            IACK is handled inline during vma determination.
           ++++++++++++++++++++++++++++++++++++++++++++++++++*/
}


/**********************************************************************
*                        reset_board()                                *
***********************************************************************
*   Reset the board to default state (system reset):
*      zero out all registers and busses.
**********************************************************************/

int reset_board()
{
   int i;

   current_index=0;
   last_index=0;
   timer=0;
   ir=0;
   mar=0;
   din=0;
   dout=0;
   mq=0;
   microstatus=0;
   macrostatus=0;
   mpc=0;
   data_bus=0;
   s_bus=0;
   r_bus=0;
   address_bus=0;
   y_bus=0;
   a_bus=0;
   b_bus=0;
   ms.rc=0;
   ms.sp=0;
   ms.mpc=0;
   ms.data=0;
   ms.inst=0;
   for(i=0;i<9;i++)
      ms.stack[i]=0;
   for(i=0;i<16;i++)
      r[i]=0;
   for(i=0;i<32;i++)
      pit.reg[i]=0;
   pit.reg[5]=0x0c;
   pit.reg[17]=0x0f;
   prescaler=31;
   ready=1;
   busreq=1;
   vma=1;
   iack=1;
   intreq=1;
   cc=1;
   sio0=1;
   sio8=1;
   bgrant=ssf=0;
   hung = FALSE;
}

/********************************************************************
*                       display_inputs()                            *
*********************************************************************
*   Display input switch/register/bus/alu/sequencer status.
*       'i' or 'd' = display input switches
*       'm' = display microsequencer registers
*       'c' = display alu registers 
*       'r' = display instruction, address, data, and status registers 
*       'p' = display PIT registers 
*       'a' = display all registers and busses
*********************************************************************/

int display_inputs(c)
char c;
{
   switch(c)
   {
      case 'i':
      case 'd': set_switch('d'); break;
      case 'm': set_ms('d');     break;
      case 'c': set_alu('d');    break;
      case 'r': set_regs('d');   break;
      case 'p': set_pit('d');    break;
      case 'a': display_regs('a');
      default:  break;
   }
}

/********************************************************************
*   set_inputs() allows the user to modify switches and registers
*********************************************************************/
/********************************************************************
*                       set_inputs()                                *
*********************************************************************
*   Modify input switch/register/bus/alu/sequencer registers/busses.
*       'i' or 'd' = modify input switches
*       'm' = modify microsequencer registers
*       'c' = modify alu registers 
*       'r' = modify instruction, address, data, and status registers 
*       'p' = modify PIT registers 
*       'a' = modify general registers, microsequencer, and alu
*********************************************************************/

int set_inputs(c)
char c;
{
   switch(c)
   {
      case 'i':
      case 'd': set_switch(c); break;
      case 'm': set_ms(c);     break;
      case 'c': set_alu(c);    break;
      case 'r': set_regs(c);   break;
      case 'p': set_pit(c);    break;
      case 'a': set_regs(c);
                set_alu(c);
                set_ms(c);
      default:  break;
   }
}

/********************************************************************
*                        regs_menu()                                *
*********************************************************************
*   Puts up a menu of general registers to modify
*********************************************************************/

int regs_menu()
{
   int choice;

   myprintf("\n");
   myprintf("                          REGISTER MENU              \n\n");
   myprintf("       1] Instruction (IR)       4] Data_Out Register  \n");
   myprintf("       2] Memory Address (MAR)   5] Micro_Status       \n");
   myprintf("       3] Data_In Register       6] Macro_Status       \n");
   myprintf("\n");
   myprintf("                           0] Exit (Done)              \n");
   myprintf("\n");
   myprintf("Your selection [0-6]? ");
   choice = get_dec();
   if (logging) fprintf(logfile,"%d\n",choice);
   while(choice < 0 || choice > 6)
   {
      myprintf("Please choose a number between 0 and 6: ");
      choice = get_dec();
      if (logging) fprintf(logfile,"%d\n",choice);
   }
   return(choice);
}

/*********************************************************************
*                       set_regs() 
**********************************************************************
*    Display/modify general registers
*********************************************************************/

int set_regs(c)
char c;
{
   char *head1=" IR   MAR  Din Dout MiSt MaSt\n";
   int i;

   myprintf("\n%s",head1);
   myprintf("%04lx %04lx %04lx %04lx  %02lx   %02lx\n\n",
         ir,mar,din,dout,microstatus,macrostatus);
   if(c=='d')
      return(0);           /* if display-only, leave now.  */

   while((i=regs_menu()) != 0)    
   {
      switch(i)
      {
	 case 0:                                                break;
         case 1: request("IR = ",&ir,0xffffL);                  break;
         case 2: request("MAR = ",&mar,0xffffL);                break;
         case 3: request("Data_In = ",&din,0xffffL);            break;
         case 4: request("Data_Out = ",&dout,0xffffL);          break;
         case 5: request("Micro_Status = ",&microstatus,0x1fL); break;
         case 6: request("Macro_Status = ",&macrostatus,0x3fL); break;
	default:                                                break;
      }
   }
   myprintf("New values:\n");
   myprintf("\n%s",head1);
   myprintf("%04lx %04lx %04lx %04lx  %02lx   %02lx\n\n",
         ir,mar,din,dout,microstatus,macrostatus);
}

/********************************************************************
*                           ms_menu()                               *
*********************************************************************
*   Puts up a menu of microsequencer registers to modify
*********************************************************************/

int ms_menu()
{
   int choice;

   myprintf("\n");
   myprintf("                  MICROSEQUENCER REGISTER MENU              \n\n");
   myprintf("       1] RC               5] Stack Reg 1      9] Stack Reg 5 \n");
   myprintf("       2] MPC              6] Stack Reg 2     10] Stack Reg 6 \n");
   myprintf("       3] SP               7] Stack Reg 3     11] Stack Reg 7 \n");
   myprintf("       4] Stack Reg 0      8] Stack Reg 4     12] Stack Reg 8 \n");
   myprintf("\n");
   myprintf("                           0] Exit (Done)                     \n");
   myprintf("\n");
   myprintf("Your selection [0-12]? ");
   choice = get_dec();
   if (logging) fprintf(logfile,"%d\n",choice);
   while(choice < 0 || choice > 12)
   {
      myprintf("Please choose a number between 0 and 12: ");
      choice = get_dec();
      if (logging) fprintf(logfile,"%d\n",choice);
   }
   return(choice);
}

/*********************************************************************
*                            set_ms()                                *
**********************************************************************
*    Display/modify microsequencer registers
*********************************************************************/

int set_ms(c)
char c;
{
   int i;
   char *head1=" RC MPC SP --------------Stack----------------\n";

   myprintf("Microsequencer registers:\n%s",head1);
   myprintf("%03lx %03lx  %01lx",ms.rc,ms.mpc,ms.sp);
   for(i=0;i<9;i++)
      myprintf(" %03lx",ms.stack[i]);
   myprintf("\n\n");

   if(c=='d')
      return(0);           /* if display-only, leave now.  */

   while((i=ms_menu()) != 0)    
   {
      switch(i)
      {
	 case 0:                                                   break;
         case 1:  request("Register/Counter = ",&ms.rc,0xfffL);    break;
         case 2:  request("MPCounter/Register = ",&ms.mpc,0xfffL); break;
         case 3:  request("Microstack Pointer = ",&ms.sp,0xfffL);  break;
         case 4:  request("MS Stack [0] = ",&ms.stack[0],0xfffL);  break;
         case 5:  request("MS Stack [1] = ",&ms.stack[1],0xfffL);  break;
         case 6:  request("MS Stack [2] = ",&ms.stack[2],0xfffL);  break;
         case 7:  request("MS Stack [3] = ",&ms.stack[3],0xfffL);  break;
         case 8:  request("MS Stack [4] = ",&ms.stack[4],0xfffL);  break;
         case 9:  request("MS Stack [5] = ",&ms.stack[5],0xfffL);  break;
         case 10: request("MS Stack [6] = ",&ms.stack[6],0xfffL);  break;
         case 11: request("MS Stack [7] = ",&ms.stack[7],0xfffL);  break;
         case 12: request("MS Stack [8] = ",&ms.stack[8],0xfffL);  break;
	 default:                                                  break;
      }
   }

   myprintf("New values:\n%s",head1);
   myprintf("%03lx %03lx  %01lx",ms.rc,ms.mpc,ms.sp);
   for(i=0;i<9;i++)
      myprintf(" %03lx",ms.stack[i]);
   myprintf("\n\n");
}

/********************************************************************
*                           alu_menu()                              *
*********************************************************************
*   Puts up a menu of alu registers to modify
*********************************************************************/

int alu_menu()
{
   int choice;

   myprintf("\n");
   myprintf("                       ALU REGISTER MENU                     \n\n");
   myprintf("     1] Reg[0]       6] Reg[4]      10] Reg[8]      14]Reg[C]  \n");
   myprintf("     2] Reg[1]       7] Reg[5]      11] Reg[9]      15]Reg[D]  \n");
   myprintf("     3] Reg[2]       8] Reg[6]      12] Reg[A]      16]Reg[E]  \n");
   myprintf("     4] Reg[3]       9] Reg[7]      13] Reg[B]      17]Reg[F]  \n");
   myprintf("     5] MQ Register                                            \n");
   myprintf("\n");
   myprintf("                           0] Exit (Done)                      \n");
   myprintf("\n");
   myprintf("Your selection [0-17]? ");
   choice = get_dec();
   if (logging) fprintf(logfile,"%d\n",choice);
   while(choice < 0 || choice > 17)
   {
      myprintf("Please choose a number between 0 and 17: ");
      choice = get_dec();
      if (logging) fprintf(logfile,"%d\n",choice);
   }
   return(choice);
}

/*********************************************************************
*                             set_alu()                              *
**********************************************************************
*    Display/modify alu registers
*********************************************************************/

int set_alu(c)
char c;
{
   int i;

   myprintf("Alu registers:    MQ: %04lx\n",mq);
   for(i=0;i<15;i++)
      myprintf(" R%01x  ",i);
   myprintf(" R%01x\n",15);
   for(i=0;i<15;i++)
      myprintf("%04lx ",r[i]);
   myprintf("%04lx\n",r[15]);

   if(c=='d')
      return(0);          /* if display-only, leave now.  */


   while((i=alu_menu()) != 0)    
   {
      switch(i)
      {
	 case 0:                                          break;
         case 1:  request("Reg[0] =  ",&r[0],0xffffL);    break;
         case 2:  request("Reg[1] =  ",&r[1],0xffffL);    break;
         case 3:  request("Reg[2] =  ",&r[2],0xffffL);    break;
         case 4:  request("Reg[3] =  ",&r[3],0xffffL);    break;
         case 5:  request("MQ Register =  ",&mq,0xffffL); break;
         case 6:  request("Reg[4] =  ",&r[4],0xffffL);    break;
         case 7:  request("Reg[5] =  ",&r[5],0xffffL);    break;
         case 8:  request("Reg[6] =  ",&r[6],0xffffL);    break;
         case 9:  request("Reg[7] =  ",&r[7],0xffffL);    break;
         case 10: request("Reg[8] =  ",&r[8],0xffffL);    break;
         case 11: request("Reg[9] =  ",&r[9],0xffffL);    break;
         case 12: request("Reg[A] =  ",&r[10],0xffffL);   break;
         case 13: request("Reg[B] =  ",&r[11],0xffffL);   break;
         case 14: request("Reg[C] =  ",&r[12],0xffffL);   break;
         case 15: request("Reg[D] =  ",&r[13],0xffffL);   break;
         case 16: request("Reg[E] =  ",&r[14],0xffffL);   break;
         case 17: request("Reg[F] =  ",&r[15],0xffffL);   break;
	 default:                                         break;
      }
   }

   myprintf("Alu registers:    MQ: %04x\n",mq);
   for(i=0;i<15;i++)
      myprintf(" R%01x  ",i);
   myprintf(" R%01x\n",15);
   for(i=0;i<15;i++)
      myprintf("%04lx ",r[i]);
   myprintf("%04lx\n",r[15]);
}

/*********************************************************************
*                           set_pit()                                *
**********************************************************************
*    Display/modify pit registers
*********************************************************************/

int set_pit(c)
char c;
{
   int i;

   myprintf("Pit registers: (00-1F hex)\n");
   for(i=0;i<32;i++)
   {
      myprintf("%02x ",pit.reg[i]);
      if((i==15)||(i==31))
         myprintf("\n");
   }

   if(c=='d')
      return(0);          /* if display-only, leave now.  */

   while(TRUE)            /* loop forever */
   {
     myprintf("\nSelect register [00-1f] (ff when done): ");
     i=get_hex()&0xff;
     if (logging) fprintf(logfile,"%x\n",i);
     if(i == 0xff) break;
     i&=0x1f;
     myprintf("Reg[%x] Old value: %x New value [hex]: ",i,pit.reg[i]); 
     pit.reg[i]=get_hex()&0xff;
     if (logging) fprintf(logfile,"%x\n",pit.reg[i]);
   }
   myprintf("\n");
  
   pit.reg[14]=pit.reg[15]=pit.reg[18]=pit.reg[22]=pit.reg[27]=0;
   pit.reg[28]=pit.reg[29]=pit.reg[30]=pit.reg[31]=0;
   pit.reg[1]&=0x7f;
   pit.reg[5]&=0xfc;
   pit.reg[16]&=0xf7;
   pit.reg[26]&=1;
   myprintf("Pit registers: (0-1F hex)\n");
   for(i=0;i<32;i++)
   {
      myprintf("%02x ",pit.reg[i]);
      if((i==15)||(i==31))
         myprintf("\n");
   }
}

/********************************************************************
*                           set_switch()                            *
*********************************************************************
*   Display/modify the input switches.
*********************************************************************/

int set_switch(c)
char c;
{
   char s[N];
   int i,j,k;

   myprintf("Current input switch status:  [");
   for(i=7;i>=0;i--)
      myprintf("%01x",(pitin>>i)&1);
   myprintf("]\n");
   if(c!='d')
   {
      myprintf("Enter new switch values [0/1]: ");
      gets(s);
      if (logging) fprintf(logfile,"%s\n",s);
      k=strlen(s);
      for(i=0,j=k-1;i<k;i++)
         switch(s[i])
         {
            case '0': pitin &= ~(1<<j--); break;
            case '1': pitin |=  (1<<j--); break;
            default:  break;
         }
      myprintf("    New input switch status:  [");
      for(i=7;i>=0;i--)
         myprintf("%01x",(pitin>>i)&1);
      myprintf("]\n");
   }
}

/******************************************************************
*                        display_regs()                           *
*******************************************************************
*   Display all registers, busses, and control signals.
******************************************************************/

int display_regs(c)
char c;
{
   char *head1="Data Addr A/B/C      R    S    Y  ";
   char *head2="                                         SIO\n";
   char *head3=" Bus  Bus  Sel  Cin Bus  Bus  Bus";
   char *head4=" Rdy* BRq* BGt VMA* R/W* IAk* Int CC* SSF 0*8*\n";

   set_leds();
   set_regs('d');
   set_ms('d');
   set_alu('d');
   myprintf("\n%s%s%s%s",head1,head2,head3,head4);
   myprintf("%04lx %04lx %01x %01x %01x  %01x  %04lx %04lx %04lx",
         data_bus,address_bus,asel,bsel,csel,cin,r_bus,s_bus,y_bus);
   myprintf("  %01x   %01x    %01x   %01x    %01x    %01x    %01x   %01x ",
         ready,busreq,bgrant,vma,rw,iack,intreq,cc);
   myprintf("  %01x  %01x %01x\n\n",ssf,sio0,sio8);
}

