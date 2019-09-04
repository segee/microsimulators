/*****************************************************************
*                       Lower.c                                  *
/*****************************************************************
*    This is a file that is included when linking on SunOs.
*   [It seems Suns don't recognise the function tolower().]
******************************************************************/

int tolower(c)
int c;
{
   if (c >= 'A' && c <= 'Z')
      return(c + 'a' - 'A');
   else
      return(c);
}

