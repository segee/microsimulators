/********************************************************************
*                          Micro.c                                  *
*********************************************************************
*   This is the global include file for the Micro?.c source files.
********************************************************************/


/********************************************************************
*                          request()                                *
*********************************************************************
*    Stuffs a value obtained from the user into an integer.
********************************************************************/

int request(s,i,k)
char *s;                                 /* field name            */
myint *i;                                /* field value           */
long k;                                  /* maximum field value   */
{  
   myint m,j;
   int dec_value;

   m = (myint) k;
   j = (*i);

   dec_value = (strncmp(s,"Clock",5)==0);

   myprintf("%s",s);
   if (j==(ANY))                          /* display old value     */
      myprintf("[*]: ");
   else if(dec_value)
      myprintf("[%ld] (decimal): ",j);
   else
      myprintf("[%lx]: ",j);  
   lnl;
                                          /* get a new value       */
   if(dec_value)
      while ((j=get_raw_dec())<=(long)(ANY))
          myprintf("Value out of range. Try again: ");
   else
      while (((j=get_raw_hex())>m) && (j != (ANY)))
          myprintf("Value out of range. Try again: ");
   lnl;
   if ((j != (ANY)) && (! dec_value))        /* mask it off and... */
      j &= m;
   (*i)=j;                                   /*    ...send it back */
}

/********************************************************************
*                         requestit()                               *
*********************************************************************
*   Prints message and calls request()
********************************************************************/

int requestit(s,i,k)
char *s;                                 /* field name            */
myint *i;                                /* field value           */
long  k;                                 /* maximum field value   */
{
   myprintf("('*' = ANY state)\n");
   request(s,i,k);
}

/********************************************************************
*                           showoff()                               *
*********************************************************************
*    Displays the state of a specification.
********************************************************************/

int showoff(s,i,j)
char *s;
myint i;
int *j;
{
   if (i != (myint)(ANY))
   {
      if(strncmp(s,"Clock",5)==0)
         myprintf("%s%ld\n",s,i);
      else
         myprintf("%s%lx\n",s,i);
      (*j)++;
      if((*j)>MAXLINES)
      {
	      (*j)=0;
	      if(more()) return(2);
      }
   }
   return(0);
}

