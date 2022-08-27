#include "HandVsHand.h"
#include <iostream>

int hvh_main(int argc, char* const argv[])
{
   HandVsHand hvh;
   hvh.build();
   hvh.save("hvh.dat");
   return 0;
}
