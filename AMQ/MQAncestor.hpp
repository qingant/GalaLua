#ifndef MQANCESTOR_HPP_1F0EDDAE_68AC_45A2_9C4D_4313D31CD373
#define MQANCESTOR_HPP_1F0EDDAE_68AC_45A2_9C4D_4313D31CD373

#pragma once

#include <cstub.h>
#include "stlincl.hpp"

namespace Galaxy
{
   namespace AMQ
   {
      class CAncestor
      {
      protected:
         CAncestor() {};
      public:
         virtual ~CAncestor() {};
      };


      class CLoneAncestor : public CAncestor
      {
      private:  // emphasize the following members are private
         CLoneAncestor( const CLoneAncestor &);
         const CLoneAncestor& operator=( const CLoneAncestor & );
      protected:
         CLoneAncestor(){};
      public:
         virtual ~CLoneAncestor() {};
      };
   } // AMQ namespace
} // Galaxy namespace

#endif /*MQANCESTOR_HPP_1F0EDDAE_68AC_45A2_9C4D_4313D31CD373*/
