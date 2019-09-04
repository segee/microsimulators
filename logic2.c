/**********************************************************************
*                             Logic2.c                                *
***********************************************************************
*   Contains the alu and sequencing logic for board 2.
***********************************************************************/

#include "sim.h"
#include "micro2.h"

/**********************************************************************
*                       logic()
***********************************************************************
*   Perform the logic functions
**********************************************************************/

int logic()
{
   int mpc_load;
   int i,a4,b4,f4;
   int pol;

/*--------------------------------------------
*  alu logic section
*-------------------------------------------*/

/* select the input values */

   a_bus = (mw.a_source) ? ia_reg : ta_reg;
   b_bus = (mw.b_source) ? ib_reg : tb_reg;

   a4=a_bus&0x0f;
   b4=b_bus&0x0f;
   c4_bit=1;              /* default values */
   c8_bit=1;

/* do the logic */

   switch(mw.alu_func)
   {
      case 0x00:  f_bus=a_bus+1-mw.cin;
                  f4=(a4+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x01:  f_bus=(a_bus|b_bus)+1-mw.cin;
                  f4=((a4|b4)+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x02:  f_bus=(a_bus|(~b_bus))+1-mw.cin;
                  f4=((a4|(~b4))+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x03:  f_bus=(0-mw.cin);
                  f4=(0-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x04:  f_bus=a_bus+(a_bus&(~b_bus))+1-mw.cin;
                  f4=(a4+(a4&(~b4))+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x05:  f_bus=(a_bus|b_bus)+(a_bus&(~b_bus))+1-mw.cin;
                  f4=((a4|b4)+(a4&(~b4))+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x06:  f_bus=a_bus-b_bus-mw.cin;
                  f4=(a4-b4-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 1:0;
                  c8_bit=((f_bus&0x1ff)>0xff)? 1:0;
                  break;
      case 0x07:  f_bus=(a_bus&(~b_bus))-mw.cin;
                  f4=((a4&(~b4))-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 1:0;
                  c8_bit=((f_bus&0x1ff)>0xff)? 1:0;
                  break;
      case 0x08:  f_bus=a_bus+(a_bus&b_bus)+1-mw.cin;
                  f4=(a4+(a4&b4)+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x09:  f_bus=a_bus+b_bus+1-mw.cin;
                  f4=(a4+b4+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0a:  f_bus=(a_bus|(~b_bus))+(a_bus&b_bus)+1-mw.cin;
                  f4=((a4|(~b4))+(a4&b4)+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0b:  f_bus=(a_bus&b_bus)-mw.cin;
                  f4=((a4&b4)-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 1:0;
                  c8_bit=((f_bus&0x1ff)>0xff)? 1:0;
                  break;
      case 0x0c:  f_bus=a_bus+a_bus+1-mw.cin;
                  f4=(a4+a4+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0d:  f_bus=(a_bus|b_bus)+a_bus+1-mw.cin;
                  f4=((a4|b4)+a4+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0e:  f_bus=(a_bus|(~b_bus))+a_bus+1-mw.cin;
                  f4=((a4|(~b4))+a4+1-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0f:  f_bus=a_bus-mw.cin;
                  f4=(a4-mw.cin)&0x1f;
                  c4_bit=(f4>0x0f)? 1:0;
                  c8_bit=((f_bus&0x1ff)>0xff)? 1:0;
                  break;
      case 0x10:  f_bus=~(a_bus);             break;
      case 0x11:  f_bus=~(a_bus|b_bus);       break;
      case 0x12:  f_bus=(~a_bus)&b_bus;       break;
      case 0x13:
        default:  f_bus= 0;                   break;
      case 0x14:  f_bus=~(a_bus&b_bus);       break;
      case 0x15:  f_bus=~(b_bus);             break;
      case 0x16:  f_bus= (a_bus^b_bus);       break;
      case 0x17:  f_bus=a_bus&(~b_bus);       break;
      case 0x18:  f_bus=(~a_bus)|b_bus;       break;
      case 0x19:  f_bus=~(a_bus^b_bus);       break;
      case 0x1a:  f_bus=b_bus;                break;
      case 0x1b:  f_bus=a_bus&b_bus;          break;
      case 0x1c:  f_bus=~0;                   break;
      case 0x1d:  f_bus=a_bus|(~b_bus);       break;
      case 0x1e:  f_bus=a_bus|b_bus;          break;
      case 0x1f:  f_bus=a_bus;                break;
   }

/* clean up the f_bus and set the rest of the status bits */

   n_bit  = (f_bus>>7)&0x01L;
   f_bus  = f_bus&0xffL;
   z_bit  = (f_bus==0xffL) ? 1 : 0;
   zp_bit = (f_bus==0)    ? 0 : 1;

/*--------------------------------------------
 *   micro-address (counter) logic
 *-------------------------------------------*/

   pol=(mw.bop&0x08) ? 1 : 0;
   switch(mw.bop&0x07)
   {
      case 0:  mpc_load=( z_bit!=pol);    break;
      case 1:  mpc_load=( n_bit!=pol);    break;
      case 2:  mpc_load=(c4_bit!=pol);    break;
      case 3:  mpc_load=(c8_bit!=pol);    break;
      case 4:  mpc_load=(go_button!=pol); break;
      case 5:  mpc_load=(zp_bit!=pol);    break;
      case 6:
      case 7:
      default: mpc_load=pol;              break;
   }

   return(mpc_load);
}


