/*
 *  
 * keysusageredis.c
 *
 */

#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>

/*
* Constants 
*/

/* Lengths, keys or values */
#define 	BITS_6  0
#define 	BITS_14  1
#define    	BITS_32  2
#define    	ENCODED  3
#define 	LENERR UINT_MAX


/* special encoding for the objects */
#define  	INT8  0
#define     INT16  1
#define     INT32  2
#define     LZF  3

/* Special RDB opcodes (rdbLoadType). */
#define		EXPIRETIME_MS  252
#define   	EXPIRETIME  253
#define   	SELECTDB  254
#define   	EOF  255

/* RDB object types */
#define		STRING  0
#define	    LIST  1
#define	    SET  2
#define	    ZSET  3
#define	    HASH  4

/* Object types for encoded objects. */
#define	    HASH_ZIPMAP  9
#define	    LIST_ZIPLIST  10
#define	    SET_INTSET  11
#define	    ZSET_ZIPLIST  12
#define	    HASH_ZIPLIST  13

/* Test if a type is an object type. */
#define rdbCheckType(t) ((t >= 0 && t <= 4) || (t >= 9 && t <= 13))

/*
 * function define
 */

int rdbLoadType();

/*
 *  data strctures and types
 */


