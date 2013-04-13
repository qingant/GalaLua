/*
 * walk.c
 *
 *  Created on: 2013-4-12
 *      Author: ocaml
 */
#include <sys/types.h>  
#include <dirent.h>  
#include <sys/stat.h>  
#include <string.h>  
#include <malloc.h>  
  
#include <lua.h>  
#include <lauxlib.h>  
#include <lualib.h>


#include <errno.h>
  
#ifndef MAX_PATH  
#define MAX_PATH        260  
#endif  
  
enum   
{  
    PQFT_UNKNOWN = 0,  
    PQFT_FILE = 1,  
    PQFT_DIR = 2  
};  
  
  
typedef struct   
{  
    DIR *   dir;  
    char    curr_path[MAX_PATH];  
}PQDIR;  
  
  
static int search_dir_first(lua_State *L)  
{  
    PQDIR * pd = NULL;  
    const char *dir_path = luaL_checkstring(L, 1);

    if ( dir_path ) {
        pd = (PQDIR *)malloc(sizeof(PQDIR));  
        if ( pd ) {
            pd->dir = opendir(dir_path);
            //fprintf (stderr, "%s\n", strerror( errno));
            if ( pd->dir ) {
               //printf ("=== search dir first ====\n");
               //printf ("xxxxxxxxx %s\n", dir_path);
            
                memset(pd->curr_path, 0, MAX_PATH);                
                memcpy(pd->curr_path, dir_path, strlen(dir_path));  
                lua_pushboolean(L, 1);  
                lua_pushlightuserdata(L, pd);  
                return 2;  
            }         
        }     
    }  
      
    lua_pushboolean(L, 0);  
    lua_pushnil(L);  
    return 2;  
}  
  
static int search_dir_next(lua_State *L)  
{  
    char filepath[MAX_PATH];  
    struct dirent *ent = NULL;  
    struct stat file_stat;  
    PQDIR *pd = (PQDIR *)lua_touserdata(L, 1);  

    if ( pd ) {  
        if ( (ent = readdir(pd->dir)) ) {  
            lua_pushboolean(L, 1);  
            lua_pushstring(L, ent->d_name);  
            sprintf(filepath, "%s/%s", pd->curr_path, ent->d_name);
            // printf ("%s\n",filepath);
            lstat(filepath, &file_stat);  
            if ( S_ISDIR(file_stat.st_mode) && !S_ISLNK(file_stat.st_mode) ) {  
                lua_pushnumber(L, PQFT_DIR);  
            } else if ( S_ISREG(file_stat.st_mode) && !S_ISLNK(file_stat.st_mode) ) {  
                lua_pushnumber(L, PQFT_FILE);  
            } else {  
                lua_pushnumber(L, PQFT_UNKNOWN);  
            }  
            return 3;  
        }     
    }  
  
    lua_pushboolean(L, 0);  
    lua_pushnil(L);  
    lua_pushnil(L);   
    return 3;  
}  
  
static int search_dir_finish(lua_State *L)  
{  
    PQDIR *pd = (PQDIR *)lua_touserdata(L, 1);  
    if ( pd ) {  
        closedir(pd->dir);  
        free(pd);  
        pd = NULL;  
    }  
    return 0;  
}  
  
int luaopen_walk(lua_State *L)  
{  
    lua_pushnumber(L, PQFT_UNKNOWN);  
    lua_setglobal(L, "PQFT_UNKNOWN");  
    lua_pushnumber(L, PQFT_FILE);  
    lua_setglobal(L, "PQFT_FILE");  
    lua_pushnumber(L, PQFT_DIR);  
    lua_setglobal(L, "PQFT_DIR");     
  
    lua_register(L, "search_dir_first", search_dir_first);  
    lua_register(L, "search_dir_next", search_dir_next);  
    lua_register(L, "search_dir_finish", search_dir_finish);  
  
    return 0;  
}  
