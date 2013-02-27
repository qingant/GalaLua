#ifndef __OSTYPE_H_INCLUDE_AF05E962_9272_4259_85C7_CF46CB659CA1__
#define __OSTYPE_H_INCLUDE_AF05E962_9272_4259_85C7_CF46CB659CA1__

#if defined(__GNUC__)                  /* GCC */
#define _ALIGN_PACK1_
#elif defined(__XLC121__)              /* XLC FOR AIX */
#define _ALIGN_PACKED_
#else
#define _ALIGN_PACK1_                   /*default*/
#endif

#ifndef	HAVE_MSGHDR_MSG_CONTROL
#define HAVE_MSGHDR_MSG_CONTROL
#endif

#endif /*__OSTYPE_H_INCLUDE_AF05E962_9272_4259_85C7_CF46CB659CA1__*/
