/******************************************
 * sample my_user code
 *****************************************/


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

extern int my_lib(int arg);
int my_debug = 1;

int main ( int argc, char *argv[])
{
  int rc =  my_lib(argc);

  if(my_debug == 1)
    printf(" Hello from my program rc = %d\n", rc);

  return 0;
}
