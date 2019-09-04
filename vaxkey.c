/****************************************************************
*                       Vaxkey.c                                *
*****************************************************************
*    vax_hotkey()   This is the hotkey implementation for the VAX.
****************************************************************/

int vax_hotkey()
{
  struct i0_statblk
  {
        short val;
        short ct;
        short info;
  } status_block;

  struct buf_desc
  {
        char  count;
        char  first;
        long  info1;
        long  info2;
  } p1;

  int status;
  short chan, chan1;
  $DESCRIPTOR (terminal, "SYS$COMMAND");


  p1.count=0;
  if (((status=SYS$ASSIGN(&terminal,&chan,0,0))&1) != 1)
        LIB$STOP(status);
  chan1=chan;
  if (((status=SYS$QIOW(0,chan1,(IO$_SENSEMODE|IO$M_TYPEAHDCNT),
                &status_block,0,0,
                &p1,1,0,0,0,0))&1) != 1)
        LIB$STOP(status);
  chan=chan1;
  if (((status=SYS$DASSGN(chan))&1) != 1)
        LIB$STOP(status);
  if(p1.count!=0)
    gets(hot_buffer);
  return(p1.count);
}

