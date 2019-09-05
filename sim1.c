/********************************************************************
*                           Sim1.c                                  *
*********************************************************************
*  This is the board 1 microsimulator code.
********************************************************************/

#include "sim.h"
#include "micro1.h"

/********************************************************************
*             initialize_memory()   (not used in board 1)           *
********************************************************************/

int initialize_memory()
{
   return(0);
}

/********************************************************************
*             release_memory()      (not used in board 1)           *
********************************************************************/

int release_memory()
{
   return(0);
}

/*********************************************************************
*                           set_leds()                               *
**********************************************************************
*   Displays the state of the led outputs.
**********************************************************************/

int set_leds()
{
   int i;

   myprintf("Executed: [%04x]   ",last_index);
   myprintf("Clock: [%04ld]   MPC: ",timer);
   myprintf("[%04x]   ",current_index);
   myprintf("LED status: [");
   for(i=3;i>=0;i--)
   {
      if((mw.data>>i)&1)
         myprintf("*");
      else
         myprintf("O");
   }
   myprintf("]\n");
}

/**********************************************************************
*                           step_clock()                              *
***********************************************************************
*  Steps the master clock one cycle:
*      load the micro_word from the control store,
*      update the counter (load, count, or no change),
*      set the output leds.
**********************************************************************/

int step_clock()
{
   int count;
   int mpc_load;
   read_inputs_from_file();
   load_mw(current_index);
   last_index=current_index;

   count=((mw.bop&1)==1);

   mpc_load = logic();

   if(mpc_load)
      current_index=mw.micro_ad;
   else if(count)
      current_index++;
   
   timer++;
   write_outputs_to_file();
   return(trace());
}

/**********************************************************************
*                        reset_board()                                *
***********************************************************************
*   Reset the board to default state (system reset):
*      set counter to zero, turn outputs off.
**********************************************************************/

int reset_board()
{
   current_index=0;
   last_index=0;
   timer=0;
   load_mw(current_index);
}

/********************************************************************
*                          set_inputs()                             *
*********************************************************************
*   Display and/or set the input switches. ('d' = display only)
*********************************************************************/

int set_inputs(c)
char c;
{
   char s[N];
   int i,j;

   read_inputs_from_file(); 

   myprintf("Input switches:  [%01x%01x%01x%01x]\n",
            sw[3],sw[2],sw[1],sw[0]);

   if(c!='d')                /* if not display-only...  */
   {
      myprintf("New values [0/1]: "); 
      gets(s);
      if (logging) fprintf(logfile,"%s\n",s);
      for(i=0,j=3;i<strlen(s);i++)
      {
         switch(s[i])
         {
            case '0': sw[j--]=0;  break;
            case '1': sw[j--]=1;  break;
             default:             break;
         }
         if(j<0) break;
      }
      myprintf("Input switches:  [%01x%01x%01x%01x]\n",
               sw[3],sw[2],sw[1],sw[0]);
      write_inputs_to_file();
   }
}

/********************************************************************
*                         display_inputs()                          *
*********************************************************************
*   Display the state of the input switches.
*********************************************************************/

int display_inputs(c)
char c;
{
   set_inputs('d');
}

/******************************************************************
*            load_rom()      (function not used in board 1)       *
******************************************************************/

int load_rom(fname)
char *fname;
{
   return(0);
}

/*****************************************************************
*                     display_regs()                             *
******************************************************************
*  Display status of input switches and output leds.
*****************************************************************/

int display_regs(c)
char c;
{
   myprintf("Input switches:  [%01x%01x%01x%01x]\n",
            sw[3],sw[2],sw[1],sw[0]);
   set_leds();
}

/*****************************************************************
*          display_ram()   (not used in board 1)                 *
*****************************************************************/

int display_ram()
{
   return(0);
}

/*****************************************************************
*          display_rom()   (not used in board 1)                 *
*****************************************************************/

int display_rom()
{
   return(0);
}

/*****************************************************************
*          edit_ram()      (not used in board 1)                 *
*****************************************************************/

int edit_ram()
{
   return(0);
}

/*****************************************************************
*          edit_rom()      (not used in board 1)                 *
*****************************************************************/

int edit_rom()
{
   return(0);
}

/*****************************************************************
*           store_rom()    (not used in board 1)                 *
*****************************************************************/

int store_rom()
{
   return(0);
}

int read_inputs_from_file(void)
{
   char *fname="./b1_inputs.txt";
   /*TODO settle on a filename and #define it */
   char line[80];
   FILE * fp;
   int temp,i;
   for(i=0,temp=0;i<4;i++) temp |= (sw[i]&1)<<i;
   fp=fopen(fname,"r");
   if(fp)
   {
      while(fgets(line,sizeof(line),fp))
       { sscanf(line,"Switches:%i",&temp);
       }
      fclose(fp);
    for(i=0;i<4;i++) sw[i] = temp >> i & 1;
    }

}

int write_inputs_to_file(void)
{
   char *fname="./b1_inputs.txt";
   /*TODO settle on a filename and #define it */
   FILE * fp;
   int temp,i;
   for(i=0,temp=0;i<4;i++) temp |= (sw[i]&1)<<i;
   fp=fopen(fname,"w");
   if(fp)
   {
      fprintf(fp,"Switches:0x%x",temp);
       
      fclose(fp);
    }

}

int write_outputs_to_file(void)
{
   char *fname="./b1_outputs.txt";
   /*TODO settle on a filename and #define it */
   FILE * fp;
   fp=fopen(fname,"w");
   if(fp)
   {
      fprintf(fp,"LEDs:0x%x",mw.data);
    
      fclose(fp);
    }

}


