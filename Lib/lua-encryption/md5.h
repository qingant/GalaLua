/*
 * md5.h
 *
 *  Created on: 2013-5-9
 *      Author: ocaml
 */

#ifndef _LIB_MD5_H_
#define _LIB_MD5_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HASHSIZE	16

/*
 *  md5 hash function.
 *  @param src: aribtary string.
 *  @param len: message length.
 *  @param des: buffer to receive the hash value. Its size must be (at least) HASHSIZE (default 16 BYTES).
 *
 */
extern void md5(const char *src, unsigned long len, unsigned char *des);

#ifdef __cplusplus
}
#endif

#endif /* _LIB_MD5_H_ */
