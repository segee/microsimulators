/********************************************************************
*                           Sim2.c                                  *
*********************************************************************
*  This is the board 2 microsimulator code.
********************************************************************/

#include "sim.h"
#include "micro2.h"

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
*  Set the individual leds from the F register, and display the result.
**********************************************************************/

int set_leds()
{
   int i;

   myprintf("Executed: [%04x]   ",last_index);
   myprintf("Clock: [%04ld]   MPC: ",timer);
   myprintf("[%04x]  ",current_index);
   myprintf("LED status: [");
   for(i=7;i>=0;i--)
   {
      if(((f_reg>>i)&1)==0)
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
*      load the micro_word from the control store,
*      perform the alu function,
*      update the counter (load, count, or no change),
*      set the output leds.
**********************************************************************/

int step_clock()
{
   int count;
   int mpc_load;
  
   get_inputs_from_file();

   load_mw(current_index);
   last_index=current_index;

   mpc_load = logic();     /* perform the logic function */

/* distribute the result */

   if((mw.alu_dest&0x01L)==0) ta_reg=f_bus;
   if((mw.alu_dest&0x02L)==0) tb_reg=f_bus;
   if((mw.alu_dest&0x04L)==0) {f_reg=f_bus;write_outputs_to_file();}

   if(mw.count==1) current_index++;

   if(mpc_load)
   {
      if(mw.bop==0xfL)
      {
         current_index=(op_code<<4)|(mw.micro_ad&0xfL);
	 ia_reg=ia_switches;
	 ib_reg=ib_switches;
      }
      else
         current_index=mw.micro_ad;
   }
   current_index&=0xfff;
   timer++;
   return(trace());
}

/**********************************************************************
*                        reset_board()                                *
***********************************************************************
*   Reset the board to default state (system reset):
*      set counter to zero, zero out registers (negative logic),
*      turn outputs off, reset status bits.
**********************************************************************/

int reset_board()
{
   current_index=0;
   last_index=0;
   timer=0;
   go_button=1;
   ta_reg=tb_reg=f_reg=a_bus=b_bus=f_bus=0xffL;
   z_bit=c4_bit=c8_bit=0;
   n_bit=zp_bit=1;
   write_inputs_to_file();
}

/********************************************************************
*                     display_inputs()                              *
*********************************************************************
*   Display the state of the input switches.
*********************************************************************/

int display_inputs(c)
char c;
{
   set_inputs('d');
}

/********************************************************************
*                          set_inputs()                             *
*********************************************************************
*   Display and/or set the input switches, go button, and registers:
*      'd' = display only
*      'g' = toggle GO button
*      'i' = modify input switches
*      'r' = modify TA, TB and F registers
*********************************************************************/

int set_inputs(c)
char c;
{
   myint temp_reg;

   switch(c)
   {
      case 'g':
                 go_button=(go_button) ? 0 : 1;
		 write_inputs_to_file();
                 myprintf("Toggle GoButton! New value: %1x ",go_button);
                 if(!go_button)
                    myprintf("[PRESSED]\n");
                 else
                    myprintf("[RELEASED]\n");
                 break;
      case 'i':
                 if(logical_data)
                    myprintf("Logical input status: Op=%1lx  IA=%02lx  IB=%02lx  Go=%1x ",
                      op_code,(~ia_reg&0xffL),(~ib_reg&0xffL),go_button);
                 else
                    myprintf("Mapped input status: Op=%1lx  IA=%02lx  IB=%02lx  Go=%1x ",
                      op_code,(ia_reg&0xffL),(ib_reg&0xffL),go_button);
                 if(!go_button)
                    myprintf("[PRESSED]\n");
                 else
                    myprintf("[RELEASED]\n");

                 request("Op Code = ",&(op_code),0x0fL);
                 if(logical_data)
		 {
		    temp_reg=(~ia_reg)&0xffL;
                    request("Input A = ",&(temp_reg),0xffL);
                    ia_reg=(~temp_reg)&0xffL;
		    temp_reg=(~ib_reg)&0xffL;
                    request("Input B = ",&(temp_reg),0xffL);
                    ib_reg=(~temp_reg)&0xffL;
                 }
		 else
		 {
                    request("Input A = ",&(ia_reg),0xffL);
                    request("Input B = ",&(ib_reg),0xffL);
                 }
		 write_inputs_to_file();
      case 'd':
                 if(logical_data)
                    myprintf("Logical input status: Op=%1lx  IA=%02lx  IB=%02lx  Go=%1x ",
                      op_code,(~ia_reg&0xffL),(~ib_reg&0xffL),go_button);
                 else
                    myprintf("Mapped input status: Op=%1lx  IA=%02lx  IB=%02lx  Go=%1x ",
                      op_code,(ia_reg&0xffL),(ib_reg&0xffL),go_button);
                 if(!go_button)
                    myprintf("[PRESSED]\n");
                 else
                    myprintf("[RELEASED]\n");
                 break;
      case 'r':
                 if(logical_data)
                   myprintf("Logical register status: TA=%02lx  TB=%02lx  F=%02lx\n",
                       (~ta_reg&0xffL),(~tb_reg&0xffL),(~f_reg&0xffL));
                 else
                   myprintf("Mapped register status: TA=%02lx  TB=%02lx  F=%02lx\n",
                       (ta_reg&0xffL),(tb_reg&0xffL),(f_reg&0xffL));

                 if(logical_data)
		 {
		    temp_reg=(~ta_reg)&0xffL;
                    request("TA_register = ",&(temp_reg),0xffL);
                    ta_reg=(~temp_reg)&0xffL;
		    temp_reg=(~tb_reg)&0xffL;
                    request("TB_register = ",&(temp_reg),0xffL);
                    tb_reg=(~temp_reg)&0xffL;
		    temp_reg=(~f_reg)&0xffL;
                    request("F_register = ",&(temp_reg),0xffL);
                    f_reg=(~temp_reg)&0xffL;
                 }
		 else
		 {
                    request("TA_register = ",&(ta_reg),0xffL);
                    request("TB_register = ",&(tb_reg),0xffL);
                    request("F_register = ",&(f_reg),0xffL);
                 }
		 write_outputs_to_file();
                 break;
   }
}

/**************************************************************
 *         load_rom()        (not used in board 2)            *
 **************************************************************/

int load_rom(fname)
char *fname;
{
   return(0);
}

/**************************************************************
 *         display_ram()      (not used in board 2)           *
 **************************************************************/

int display_ram()
{
   return(0);
}

/**************************************************************
 *         edit_rom()         (not used in board 2)           *
 **************************************************************/

int edit_rom()
{
   return(0);
}

/**************************************************************
 *         edit_ram()         (not used in board 2)           *
 **************************************************************/

int edit_ram()
{
   return(0);
}

/**************************************************************
 *         display_rom()      (not used in board 2)           *
 **************************************************************/

int display_rom()
{
   return(0);
}

/**************************************************************
 *         store_rom()        (not used in board 2)           *
 **************************************************************/

int store_rom()
{
   return(0);
}

/**************************************************************
*                    display_regs()                           *
***************************************************************
*  Display the bus and register status
**************************************************************/

int display_regs(c)
char c;
{
   char bit[6];
   char *head1="   IA IB OpCode  TA TB  F  ABus BBus FBus  ";
   char *head2="Z N C4 C8 GO ZP";
   char *head3a=" [Logical]\n";
   char *head3b=" [1=HIGH]\n";

   bit[0] = (z_bit==1)?     'H':'L';
   bit[1] = (n_bit==1)?     'H':'L';
   bit[2] = (c4_bit==1)?    'H':'L';
   bit[3] = (c8_bit==1)?    'H':'L';
   bit[4] = (go_button==1)? 'H':'L';
   bit[5] = (zp_bit==1)?    'H':'L';

   set_leds();
   myprintf("%s",head1);
   myprintf("%s",head2);
   if(logical_data)
      myprintf("%s",head3a);
   else
      myprintf("%s",head3b);

   if(logical_data)
      myprintf("   %02lx %02lx    %01lx    %02lx %02lx %02lx   %02lx   %02lx   %02lx",
        (~ia_reg)&0xffL,(~ib_reg)&0xffL,op_code,
        (~ta_reg)&0xffL,(~tb_reg)&0xffL,(~f_reg)&0xffL,
        (~a_bus)&0xffL,(~b_bus)&0xffL,(~f_bus)&0xffL);
   else
      myprintf("   %02lx %02lx    %01lx    %02lx %02lx %02lx   %02lx   %02lx   %02lx",
        (ia_reg)&0xffL,(ib_reg)&0xffL,op_code,
        (ta_reg)&0xffL,(tb_reg)&0xffL,(f_reg)&0xffL,
        (a_bus)&0xffL,(b_bus)&0xffL,(f_bus)&0xffL);
   myprintf("   %c %c  %c  %c  %c  %c\n",
         bit[0],bit[1],bit[2],bit[3],bit[4],bit[5]);
}

int read_inputs_from_file(void)
{
   char *fname="./i_o_directory/b2_inputs.txt";
   /*TODO settle on a filename and #define it */
   char line[80];
   FILE * fp;
   int go,op,ia,ib;
  
   go=go_button;
   op=op_code;
   ia=ia_reg;
   ib=ib_reg;
  
   fp=fopen(fname,"r");
   if(fp)
   {
      while(fgets(line,sizeof(line),fp))
       { sscanf(line,"Go:%i",&go);
	 sscanf(line,"Op:%i",&op);
	 sscanf(line,"Ia:%i",&ia);
	 sscanf(line,"Ib:%i",&ib);
       }
      fclose(fp);
      go_button=go;
      op_code=op;
      ia_switches=ia;
      ib_switches=ib;
    }

}

int write_inputs_to_file(void)
{
   char *fname="./i_o_directory/b2_inputs.txt";
   /*TODO settle on a filename and #define it */
   FILE * fp;
   int temp,i;
   
   fp=fopen(fname,"w");
   if(fp)
   {
      fprintf(fp,"Go:0x%x",go_button);
      fprintf(fp,"Op:0x%x",op_code);
      fprintf(fp,"Ia:0x%x",ia_reg);
      fprintf(fp,"Ib:0x%x",ib_reg);
       
      fclose(fp);
    }

}

int write_outputs_to_file(void)
{
   char *fname="./i_o_directory/b2_outputs.txt";
   /*TODO settle on a filename and #define it */
   FILE * fp;
   fp=fopen(fname,"w");
   if(fp)
   {
      fprintf(fp,"LEDs:0x%x",(f_reg));
    
      fclose(fp);
      //printf("LEDS are 0x%x\n",(mw.data&0xf));
    }

}

