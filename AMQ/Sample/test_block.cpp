#include "Block.hpp"

void test( IBlock &tt ){
   printf( "%s\n", tt.RAWEntry(  ) );
}

int main(  ){
   std::string ttt = "testit";
   std::string &t = ttt;
   test( t );
   return 0;
   
}
