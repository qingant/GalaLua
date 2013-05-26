
#ifndef _OZIP_UTIL_H_
#define _OZIP_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define OZIP_ERROR(args...)                                             \
    do                    																		\
    {                     																		\
       fprintf(stderr,"[ FILE=%s FUNC=%s LINE=%d ] [", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
       fprintf(stderr,args);                                            						\
       fprintf(stderr,"]\n");                                           						\
    }while(false)

#ifdef _DEBUG_
#define OZIP_DEBUG(args...)                                     						\
   do                                                                                                                                   \
   {                                                                                                                                    \
      printf("FILE=%s FUNC=%s LINE=%d\n  ", __FILE__, __PRETTY_FUNCTION__, __LINE__); 	\
      printf(args);                                                     				\
      printf("\n");                                                     				\
   }while(0)

#else
#define OZIP_DEBUG(msg...) (void)0;
#endif

#include "io_api.h"
#include "otype.h"


/*
 *  Read a byte from a gz_stream; update next_in and avail_in. Return EOF
 *  for end of file.
 *  IN assertion: the stream s has been sucessfully opened for reading.
 */
extern int get_byte(const ozip_filefunc* filefunc, voidpf filestream, int *pi);

/*
 * Reads a long/2 in LSB order from the given gz_stream. Sets
 */
extern int get_short(const ozip_filefunc* filefunc, voidpf filestream,uLong *pX);

/*
 * Reads a long in LSB order from the given gz_stream. Sets
 */
extern int get_long(const ozip_filefunc* filefunc,voidpf filestream,uLong *pX);



/*
 *  Compare two filename (fileName1,fileName2).
 *  If iCaseSenisivity = 1, comparision is case sensitivity (like strcmp)
 *  If iCaseSenisivity = 2, comparision is not case sensitivity (like strcmpi
 *                                                               or strcasecmp)
 *  If iCaseSenisivity = 0, case sensitivity is defaut of your operating system
 *       (like 1 on Unix, 2 on Windows)
 */

/* ozip strcmpi / strcasecmp */
extern int strcmp_case_nosensitive (const char* fileName1,const char* fileName2);

extern int string_file_name_compare (const char* fileName1,const char* fileName2,int iCaseSensitivity);

#ifdef __cplusplus
}
#endif

#endif /* _OZIP_UTIL_H_ */
