
/******************************************
 * sample my_lib library
 *****************************************/
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

extern int my_debug;


int my_lib(int arg)
{
  
  if(my_debug == 1)
    printf(" Hello from my lib arg = %d\n", arg);  
  return arg * 2;

}
