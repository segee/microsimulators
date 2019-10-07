/********************************************************************
*                           Sim3.c                                  *
*********************************************************************
*  This is the board 3 microsimulator code.
********************************************************************/

#include "sim.h"
#include "micro3.h"

int write_outputs_to_file(void);

/*******************************************************************
*                     initialize_memory()                          *
********************************************************************
*  Allocate memory for the ram and rom.
********************************************************************/

int initialize_memory()
{
   memory = (unsigned char *) calloc(MEMSIZE*2,sizeof(char));      
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
   unsigned char c;
   char macname[N];

   strcpy(macname,fname);

/* if the macrocode file name has no extension, add one [.mac] */

   i=0;
   while(macname[i] != '\0' && macname[i] != '.')
      i++;
   if(macname[i]=='\0')
      strcat(macname,".mac");
   myprintf("Loading... \n");

/* open the macrocode file for input as a binary file */

   if((mac=open(macname,READ_ARGS))== -1)
   {
      myprintf("Error opening macrocode file %s.\n",macname);
      return(-1);
   }

/* if open was successful, zero rom, load rom, and close rom file.  */

   for(i=0;i<MEMSIZE;i++)
      *(memory+(rom*MEMSIZE)+i)=0;
   for(i=0;(read(mac,(char *) &c,1)>0)&&(i<MEMSIZE);i++)
      *(memory+(rom*MEMSIZE)+i)=c&0xff;
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
   unsigned char c;
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
      if(write(mac,(char *) (memory+(rom*MEMSIZE)+i),1)<=0) break;
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
*  Allow user to modify RAM or ROM (with partial address decoding)
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
      valid=((0<=index)&&(index<ADDSIZE));
      if(!valid)
         myprintf("\n   Invalid address. \n\n");
   }
   myprintf("\nEnter a period (.) when done.\n\n");

   while(!done)
   {
      myprintf("   [%04x] old: %02x   new: ",index,
                  (*(memory+(mem_type*MEMSIZE)+(index&HIGHMEM)))&0xff);
      done=((temp=get_raw_hex())==(-1));
      if(!done)
      {
         if (logging) fprintf(logfile,"%x\n",temp);
         *(memory+(mem_type*MEMSIZE)+(index&HIGHMEM))=temp&0xff;
      }
      if(++index>=ADDSIZE)
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
* Display the contents of RAM or ROM (with partial address decoding)
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
      valid=((0<=first)&&(first<=last)&&(last<ADDSIZE));
      if(!valid)
         myprintf("\nInvalid address.\n\n");
   }

   if(mem_type==ram)
      myprintf("RAM contents:\n");
   else
      myprintf("ROM contents:\n");

   for(i=first;i<=last;)
   {
      if(((i-first)%16)==0)
         myprintf("[%03x] ",i);
      for(j=0;(j<16)&&(i<=last);j++,i++)
         myprintf("%02x ",*(memory+(mem_type*MEMSIZE)+(i&HIGHMEM)));
      if((((i-first)%16)==0)||(i>=last))
         myprintf("\n");
      if(((i-first)%320)==0)
         if(more())
         {
            myprintf("Cancelled!\n\n");
            return(0);
         }
   }
}


/*********************************************************************
*                           set_leds()                               *
**********************************************************************
*  Set the individual leds from the F register, and display the result.
**********************************************************************/

int set_leds()
{
   int i;

   myprintf("Executed: [%04x]   ",last_index);
   myprintf("Clock: [%04ld]   MPC: ",timer);
   myprintf("[%04x]   ",current_index);
   myprintf("LEDs: [");
   for(i=7;i>=0;i--)
   {
      if(((leds>>i)&1)==0)
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
*      load the pipeline register (microword) from the control store
*      perform the alu function (in the logic() function),
*      update the counter (load, count, or no change),
*      set the output leds or read/write external memory.
**********************************************************************/

int step_clock()
{
   int count;
   int mpc_load;

   load_mw(current_index);
   last_index=current_index;
 
   mpc_load = logic();

/* set the rest of the status bits and clean up f_bus */

   n_bit = (f_bus>>7)&0x01;
   f_bus = f_bus&0xff;
   z_bit = (f_bus==0) ? 1 : 0;

/*  distribute the result */

   switch(mw.alu_dest)
   {
      case 0:   pch=f_bus;         break;
      case 1:   pcl=f_bus;         break;
      case 2:   sph=f_bus;         break;
      case 3:   spl=f_bus;         break;
      case 4:   marh=f_bus;        break;
      case 5:   marl=f_bus;        break;
      case 6:   acc=f_bus;         break;
      case 7:                              /* see below */
      default:                     break;
   }
   if(mw.en_sr==0)
   {
      if(mw.alu_dest==7)         /* load sr from f_bus */
         sr=f_bus;
      else
      {
         sr=sr&0xf0;             /* clear low bits */
         sr=sr|(z_bit&1);        /* (re)set z_bit  */
         sr=sr|((n_bit&1)<<1);   /* (re)set n_bit  */
         sr=sr|((c4_bit&1)<<2);  /* (re)set c4_bit */
         sr=sr|((c8_bit&1)<<3);  /* (re)set c8_bit */
      }
   }

/* Access memory (ram/rom/led reg) for read/write as req'd */

   data_bus=0xffL;                      /* default floating address bus */
   if(mw.r_w==0) data_bus=f_bus;
   if(mw.vma==0)
      switch((address_bus>>13)&0x07)
      {
         case 0:   if(mw.r_w==1)
                      data_bus=(*(memory+(rom*MEMSIZE)+(address_bus&0x7ff)));
                   break;
         case 4:   if(mw.r_w==0)
                      leds=data_bus&0xff;
		      write_outputs_to_file();
                   break;
         case 7:   if(mw.r_w==1)
                      data_bus=(*(memory+(ram*MEMSIZE)+(address_bus&0x7ff)))&0xff;
                   else
                      *(memory+(ram*MEMSIZE)+(address_bus&0x7ff))=data_bus&0xff;
                   break;
         default:  break;
      }

   if(mw.r_w==1) data_in=data_bus;

   if(mw.count==1) current_index++;

   if(mpc_load)
      switch(mw.op_con_ctl)
      {
         case 0:
         case 1:   current_index=mw.micro_ad;
                   break;
         case 2:   current_index=((b_bus<<4)&0xff0)|(mw.micro_ad&0x0f);
                   break;
         default:  current_index=0xff0|(mw.micro_ad&0x0f);
                   break;
      }
   current_index&=0xfff;

/*----------------------------------------------------------------
 *  latch c8 for cmicro and store tracedata if required.
 *---------------------------------------------------------------*/
   cmicro=c8_bit;
   timer++;
   return(trace());
}


/**********************************************************************
*                        reset_board()                                *
***********************************************************************
*   Reset the board to default state (system reset):
*      set counter to zero, zero out registers and busses.
**********************************************************************/

int reset_board()
{
   current_index=0;
   last_index=0;
   timer=0;
   acc=sr=sph=spl=marh=marl=pch=pcl=data_in=0;
   a_bus=b_bus=f_bus=data_bus=address_bus=0;
}

/********************************************************************
*                      display_inputs()                             *
*********************************************************************
*   Display all register and bus states.
*********************************************************************/
int display_inputs(c)
char c;
{
   display_regs('a');
}

/********************************************************************
*                      set_input_menu()                             *
*********************************************************************
*   Puts up a menu of registers to modify
*********************************************************************/

int set_input_menu()
{
   int choice;

   myprintf("\n");
   myprintf("                          REGISTER MENU              \n\n");
   myprintf("       1] ACC              4] SPH              7] SPL  \n");
   myprintf("       2] SR               5] MARH             8] MARL \n");
   myprintf("       3] Data_In          6] PCH              9] PCL  \n");
   myprintf("\n");
   myprintf("                           0] Exit (Done)              \n");
   myprintf("\n");
   myprintf("Your selection [0-9]? ");
   choice = get_dec();
   if (logging) fprintf(logfile,"%d\n",choice);
   while(choice < 0 || choice > 9)
   {
      myprintf("Please choose a number between 0 and 9: ");
      choice = get_dec();
      if (logging) fprintf(logfile,"%d\n",choice);
   }
   return(choice);
}

/********************************************************************
*                      set_inputs()                                 *
*********************************************************************
*   Allow the user to modify the register contents
*********************************************************************/

int set_inputs(c)
char c;
{
   int i;

   myprintf("\nCurrent values:\n");
   myprintf("ACC, SR, SPH, SPL, MARH, MARL, PCH, PCL, DATA_IN\n");
   myprintf(" %02lx  %02lx   %02lx   %02lx   %02lx    %02lx   %02lx   %02lx   %02lx\n",
           acc,sr,sph,spl,marh,marl,pch,pcl,data_in);

   while((i=set_input_menu()) != 0)    
   {
      switch(i)
      {
	 case 0:                                        break;
         case 1: request("ACC =  ",&acc,0xffL);         break;
         case 2: request("SR =  ",&sr,0xffL);           break;
         case 4: request("SPH =  ",&sph,0xffL);         break;
         case 7: request("SPL =  ",&spl,0xffL);         break;
         case 5: request("MARH =  ",&marh,0xffL);       break;
         case 8: request("MARL =  ",&marl,0xffL);       break;
         case 6: request("PCH =  ",&pch,0xffL);         break;
         case 9: request("PCL =  ",&pcl,0xffL);         break;
         case 3: request("Data_In =  ",&data_in,0xffL); break;
	default:                                        break;
      }
   }
   myprintf("\nNew values:\n");
   myprintf("ACC, SR, SPH, SPL, MARH, MARL, PCH, PCL, DATA\n");
   myprintf(" %02lx  %02lx   %02lx   %02lx   %02lx    %02lx   %02lx   %02lx   %02lx\n",
           acc,sr,sph,spl,marh,marl,pch,pcl,data_in);
}

/******************************************************************
*                      display_regs()                             *
*******************************************************************
*   Display the state of busses and registers
******************************************************************/

int display_regs(c)
char c;
{
   char *head1=" ACC SR  SP   MAR  PC  DI ABus BBus FBus";
   char *head2=" DtBus AdBus Cmicr* R_W Vma\n";

   set_leds();
   myprintf("\n%s",head1);
   myprintf("%s",head2);

    myprintf("  %02lx %02lx %02lx%02lx %02lx%02lx %02lx%02lx %02lx",
            acc,sr,sph,spl,marh,marl,pch,pcl,data_in);
    myprintf("  %02lx   %02lx   %02lx    %02lx   %04lx    %01x",
            a_bus,b_bus,f_bus,data_bus,address_bus,cmicro);
    myprintf("    %01lx   %01lx\n", mw.r_w,mw.vma);
}

int write_outputs_to_file(void)
{
   char *fname="./i_o_directory/b3_outputs.txt";
   /*TODO settle on a filename and #define it */
   FILE * fp;
   fp=fopen(fname,"w");
   if(fp)
   {
      fprintf(fp,"LEDs:0x%x",leds);
    
      fclose(fp);
   
    }

}

