/**********************************************************************
*                             Logic3.c                                *
***********************************************************************
*   Contains the alu and sequencing logic for board 3.
***********************************************************************/

#include "sim.h"
#include "micro3.h"

/**********************************************************************
*                              logic() 
***********************************************************************
*      Load the pipeline register (microword) from the control store.
*             (the sequencing function).
*      Perform the alu function,
**********************************************************************/

int logic()
{
   int mpc_load;
   int i,a4,b4,f4;

   load_mw(current_index);
   last_index=current_index;

/*---------------------------------------------------------
*   set branch condition logic
*----------------------------------------------------------*/
   switch(mw.bop&0x0f)
   {
      case 0:  mpc_load=(sr&0x01);  break; /* Z set        */
      case 1:  mpc_load=(sr&0x02);  break; /* N set        */
      case 2:  mpc_load= (sr&0x04); break; /*~C4 clear     */
      case 3:  mpc_load= (sr&0x08); break; /*~C8 clear     */
      case 4:  mpc_load= (cmicro);  break; /*~cmicro clear */
      case 5:  mpc_load= FALSE;     break; /* never        */
      case 8:  mpc_load=!(sr&0x01); break; /* Z clear      */
      case 9:  mpc_load=!(sr&0x02); break; /* N clear      */
      case 10: mpc_load=!(sr&0x04); break; /*~C4 set       */
      case 11: mpc_load=!(sr&0x08); break; /*~C8 set       */
      case 12: mpc_load=!(cmicro);  break; /*~cmicro set   */
      case 13: mpc_load= TRUE;      break; /* always       */
      default: mpc_load= FALSE;     break; /* never        */
   }

/*---------------------------------------------------------
*   set up address bus for external read or write
*----------------------------------------------------------*/
   switch(mw.ad_bus)
   {
      case 0:   address_bus=pch*256+pcl;   break;
      case 1:   address_bus=sph*256+spl;   break;
      case 2:   address_bus=marh*256+marl; break;
      default:  address_bus=0xffff;        break;
   }

/*--------------------------------------------------------
*  alu logic section
*---------------------------------------------------------*/

/* select the input values */

   switch(mw.op_con_ctl)
   {
      case 0:   b_bus=data_in;     break;
      case 1:   b_bus=mw.constant; break;
      case 2:   b_bus=data_in;     break;
      default:  b_bus=0xff;        break;
   }
   switch(mw.alu_source&0x07)
   {
      case 0:   a_bus=pch;         break;
      case 1:   a_bus=pcl;         break;
      case 2:   a_bus=sph;         break;
      case 3:   a_bus=spl;         break;
      case 4:   a_bus=marh;        break;
      case 5:   a_bus=marl;        break;
      case 6:   a_bus=acc;         break;
      case 7:   a_bus=sr;          break;
      default:  a_bus=0xff;        break;
   }
   switch(mw.cin)    /* active low */
   {
      case 0:  cin_bit=cmicro;       break;
      case 1:  cin_bit=(sr>>3)&0x01; break;
      case 2:  cin_bit=1;            break;
      case 3:  cin_bit=0;            break;
   }

/*  Increment PC if requested.  */

   if(mw.inc_pc==1)
      if(++pcl>0xff)
      {
         pch++;
         pcl=0;
      }

/* prepare for alu logic */

   a4=a_bus&0x0f;
   b4=b_bus&0x0f;
   c4_bit=1;              /* default values */
   c8_bit=1;

/* do the logic */

   switch(mw.alu_func)
   {
      case 0x00:  f_bus=a_bus+1-cin_bit;
                  f4=(a4+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x01:  f_bus=(a_bus|b_bus)+1-cin_bit;
                  f4=((a4|b4)+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x02:  f_bus=(a_bus|(~b_bus))+1-cin_bit;
                  f4=((a4|(~b4))+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x03:  f_bus=(0-cin_bit);
                  f4=(0-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x04:  f_bus=a_bus+(a_bus&(~b_bus))+1-cin_bit;
                  f4=(a4+(a4&(~b4))+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x05:  f_bus=(a_bus|b_bus)+(a_bus&(~b_bus))+1-cin_bit;
                  f4=((a4|b4)+(a4&(~b4))+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x06:  f_bus=a_bus-b_bus-cin_bit;
                  f4=(a4-b4-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 1:0;
                  c8_bit=((f_bus&0x1ff)>0xff)? 1:0;
                  break;
      case 0x07:  f_bus=(a_bus&(~b_bus))-cin_bit;
                  f4=((a4&(~b4))-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 1:0;
                  c8_bit=((f_bus&0x1ff)>0xff)? 1:0;
                  break;
      case 0x08:  f_bus=a_bus+(a_bus&b_bus)+1-cin_bit;
                  f4=(a4+(a4&b4)+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x09:  f_bus=a_bus+b_bus+1-cin_bit;
                  f4=(a4+b4+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0a:  f_bus=(a_bus|(~b_bus))+(a_bus&b_bus)+1-cin_bit;
                  f4=((a4|(~b4))+(a4&b4)+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0b:  f_bus=(a_bus&b_bus)-cin_bit;
                  f4=((a4&b4)-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 1:0;
                  c8_bit=((f_bus&0x1ff)>0xff)? 1:0;
                  break;
      case 0x0c:  f_bus=a_bus+a_bus+1-cin_bit;
                  f4=(a4+a4+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0d:  f_bus=(a_bus|b_bus)+a_bus+1-cin_bit;
                  f4=((a4|b4)+a4+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0e:  f_bus=(a_bus|(~b_bus))+a_bus+1-cin_bit;
                  f4=((a4|(~b4))+a4+1-cin_bit)&0x1f;
                  c4_bit=(f4>0x0f)? 0:1;
                  c8_bit=((f_bus&0x1ff)>0xff)? 0:1;
                  break;
      case 0x0f:  f_bus=a_bus-cin_bit;
                  f4=(a4-cin_bit)&0x1f;
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
   return(mpc_load);
}
