/*
 * test.c
 *
 *  Created on: Apr 15, 2013
 *      Author: esuomyekcim
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "resource.h"

#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
}
#endif

int main(int argc, char *argv[])
{
    char buf[256];

    resx_environ_t resxenv;
    if (resx_environ_open(&resxenv) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n", __FILE__,
                __FUNCTION__, __LINE__);
        exit(EXIT_FAILURE);
    }

    int32_t retval = resx_environ_read(&resxenv, "/grand_regular_a.txt");
    printf("Line %d: retval = %" PRId32 "\n", __LINE__, retval);
    if (retval > 0)
    {
        size_t siz = (retval < sizeof(buf) ? retval : sizeof(buf));
        retval = resx_fstream_read(&buf[0], 1, siz, resxenv.stream);
        printf("Line %d: retval = %d\n", __LINE__, retval);
        buf[siz] = '\0';
        printf("Line %d: buf = %s\n", __LINE__, &buf[0]);
    }
    printf("------------------------------\n");
    retval = resx_environ_read(&resxenv, "/grand_directory/child_regular.txt");
    printf("Line %d: retval = %" PRId32 "\n", __LINE__, retval);
    if (retval > 0)
    {
        size_t siz = (retval < sizeof(buf) ? retval : sizeof(buf));
        retval = resx_fstream_read(&buf[0], 1, siz, resxenv.stream);
        printf("Line %d: retval = %d\n", __LINE__, retval);
        buf[siz] = '\0';
        printf("Line %d: buf = %s\n", __LINE__, &buf[0]);
    }
    printf("------------------------------\n");
    retval = resx_environ_read(&resxenv, "/grand_directory/child_directory_a/grand_child_regular.txt");
    printf("Line %d: retval = %" PRId32 "\n", __LINE__, retval);
    if (retval > 0)
    {
        size_t siz = (retval < sizeof(buf) ? retval : sizeof(buf));
        retval = resx_fstream_read(&buf[0], 1, siz, resxenv.stream);
        printf("Line %d: retval = %d\n", __LINE__, retval);
        buf[siz] = '\0';
        printf("Line %d: buf = %s\n", __LINE__, &buf[0]);
    }
    return 0;
}

