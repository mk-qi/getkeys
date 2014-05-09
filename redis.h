/*
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __REDIS_H
#define __REDIS_H

#include "fmacros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <syslog.h>
#include <netinet/in.h>
#include <signal.h>
//#include "ae.h"      /* Event driven programming library */
#include "sds.h"     /* Dynamic safe strings */
#include "dict.h"    /* Hash tables */
#include "adlist.h"  /* Linked lists */
#include "zmalloc.h" /* total memory usage aware version of malloc/free */
#include "ziplist.h" /* Compact list data structure */
#include "intset.h"  /* Compact integer set structure */
#include "util.h"    /* Misc functions useful in many places */


/* Error codes */
#define REDIS_OK                0
#define REDIS_ERR               -1

/* Static server configuration */
#define REDIS_DEFAULT_HZ        10      /* Time interrupt calls/sec. */
#define REDIS_MIN_HZ            1
#define REDIS_MAX_HZ            500 
#define REDIS_SERVERPORT        6379    /* TCP port */
#define REDIS_TCP_BACKLOG       511     /* TCP listen backlog */
#define REDIS_MAXIDLETIME       0       /* default client timeout: infinite */
#define REDIS_DEFAULT_DBNUM     16
#define REDIS_CONFIGLINE_MAX    1024
#define REDIS_DBCRON_DBS_PER_CALL 16
#define REDIS_MAX_WRITE_PER_EVENT (1024*64)
#define REDIS_SHARED_SELECT_CMDS 10
#define REDIS_SHARED_INTEGERS 10000
#define REDIS_SHARED_BULKHDR_LEN 32
#define REDIS_MAX_LOGMSG_LEN    1024 /* Default maximum length of syslog messages */
#define REDIS_AOF_REWRITE_PERC  100
#define REDIS_AOF_REWRITE_MIN_SIZE (64*1024*1024)
#define REDIS_AOF_REWRITE_ITEMS_PER_CMD 64
#define REDIS_SLOWLOG_LOG_SLOWER_THAN 10000
#define REDIS_SLOWLOG_MAX_LEN 128
#define REDIS_MAX_CLIENTS 10000
#define REDIS_AUTHPASS_MAX_LEN 512
#define REDIS_DEFAULT_SLAVE_PRIORITY 100
#define REDIS_REPL_TIMEOUT 60
#define REDIS_REPL_PING_SLAVE_PERIOD 10
#define REDIS_RUN_ID_SIZE 40
#define REDIS_OPS_SEC_SAMPLES 16
#define REDIS_DEFAULT_REPL_BACKLOG_SIZE (1024*1024)    /* 1mb */
#define REDIS_DEFAULT_REPL_BACKLOG_TIME_LIMIT (60*60)  /* 1 hour */
#define REDIS_REPL_BACKLOG_MIN_SIZE (1024*16)          /* 16k */
#define REDIS_BGSAVE_RETRY_DELAY 5 /* Wait a few secs before trying again. */
#define REDIS_DEFAULT_PID_FILE "/var/run/redis.pid"
#define REDIS_DEFAULT_SYSLOG_IDENT "redis"
#define REDIS_DEFAULT_CLUSTER_CONFIG_FILE "nodes.conf"
#define REDIS_DEFAULT_DAEMONIZE 0
#define REDIS_DEFAULT_UNIX_SOCKET_PERM 0
#define REDIS_DEFAULT_TCP_KEEPALIVE 0
#define REDIS_DEFAULT_LOGFILE ""
#define REDIS_DEFAULT_SYSLOG_ENABLED 0
#define REDIS_DEFAULT_STOP_WRITES_ON_BGSAVE_ERROR 1
#define REDIS_DEFAULT_RDB_COMPRESSION 1
#define REDIS_DEFAULT_RDB_CHECKSUM 1
#define REDIS_DEFAULT_RDB_FILENAME "dump.rdb"
#define REDIS_DEFAULT_SLAVE_SERVE_STALE_DATA 1
#define REDIS_DEFAULT_SLAVE_READ_ONLY 1
#define REDIS_DEFAULT_REPL_DISABLE_TCP_NODELAY 0
#define REDIS_DEFAULT_MAXMEMORY 0
#define REDIS_DEFAULT_MAXMEMORY_SAMPLES 5
#define REDIS_DEFAULT_AOF_FILENAME "appendonly.aof"
#define REDIS_DEFAULT_AOF_NO_FSYNC_ON_REWRITE 0
#define REDIS_DEFAULT_ACTIVE_REHASHING 1
#define REDIS_DEFAULT_AOF_REWRITE_INCREMENTAL_FSYNC 1
#define REDIS_DEFAULT_MIN_SLAVES_TO_WRITE 0
#define REDIS_DEFAULT_MIN_SLAVES_MAX_LAG 10
#define REDIS_IP_STR_LEN INET6_ADDRSTRLEN
#define REDIS_PEER_ID_LEN (REDIS_IP_STR_LEN+32) /* Must be enough for ip:port */
#define REDIS_BINDADDR_MAX 16
#define REDIS_MIN_RESERVED_FDS 32

#define ACTIVE_EXPIRE_CYCLE_LOOKUPS_PER_LOOP 20 /* Loopkups per loop. */
#define ACTIVE_EXPIRE_CYCLE_FAST_DURATION 1000 /* Microseconds */
#define ACTIVE_EXPIRE_CYCLE_SLOW_TIME_PERC 25 /* CPU max % for keys collection */
#define ACTIVE_EXPIRE_CYCLE_SLOW 0
#define ACTIVE_EXPIRE_CYCLE_FAST 1

/* Protocol and I/O related defines */
#define REDIS_MAX_QUERYBUF_LEN  (1024*1024*1024) /* 1GB max query buffer. */
#define REDIS_IOBUF_LEN         (1024*16)  /* Generic I/O buffer size */
#define REDIS_REPLY_CHUNK_BYTES (16*1024) /* 16k output buffer */
#define REDIS_INLINE_MAX_SIZE   (1024*64) /* Max size of inline reads */
#define REDIS_MBULK_BIG_ARG     (1024*32)
#define REDIS_LONGSTR_SIZE      21          /* Bytes needed for long -> str */
#define REDIS_AOF_AUTOSYNC_BYTES (1024*1024*32) /* fdatasync every 32MB */
/* When configuring the Redis eventloop, we setup it so that the total number
 * of file descriptors we can handle are server.maxclients + RESERVED_FDS + FDSET_INCR
 * that is our safety margin. */
#define REDIS_EVENTLOOP_FDSET_INCR (REDIS_MIN_RESERVED_FDS+96)

/* Hash table parameters */
#define REDIS_HT_MINFILL        10      /* Minimal hash table fill 10% */

/* Object types */
#define REDIS_STRING 0
#define REDIS_LIST 1
#define REDIS_SET 2
#define REDIS_ZSET 3
#define REDIS_HASH 4

/* Objects encoding. Some kind of objects like Strings and Hashes can be
 * internally represented in multiple ways. The 'encoding' field of the object
 * is set to one of this fields for this object. */
#define REDIS_ENCODING_RAW 0     /* Raw representation */
#define REDIS_ENCODING_INT 1     /* Encoded as integer */
#define REDIS_ENCODING_HT 2      /* Encoded as hash table */
#define REDIS_ENCODING_ZIPMAP 3  /* Encoded as zipmap */
#define REDIS_ENCODING_LINKEDLIST 4 /* Encoded as regular linked list */
#define REDIS_ENCODING_ZIPLIST 5 /* Encoded as ziplist */
#define REDIS_ENCODING_INTSET 6  /* Encoded as intset */
#define REDIS_ENCODING_SKIPLIST 7  /* Encoded as skiplist */
#define REDIS_ENCODING_EMBSTR 8  /* Embedded sds string encoding */

/* Defines related to the dump file format. To store 32 bits lengths for short
 * keys requires a lot of space, so we check the most significant 2 bits of
 * the first byte to interpreter the length:
 *
 * 00|000000 => if the two MSB are 00 the len is the 6 bits of this byte
 * 01|000000 00000000 =>  01, the len is 14 byes, 6 bits + 8 bits of next byte
 * 10|000000 [32 bit integer] => if it's 10, a full 32 bit len will follow
 * 11|000000 this means: specially encoded object will follow. The six bits
 *           number specify the kind of object that follows.
 *           See the REDIS_RDB_ENC_* defines.
 *
 * Lengths up to 63 are stored using a single byte, most DB keys, and may
 * values, will fit inside. */
#define REDIS_RDB_6BITLEN 0
#define REDIS_RDB_14BITLEN 1
#define REDIS_RDB_32BITLEN 2
#define REDIS_RDB_ENCVAL 3
#define REDIS_RDB_LENERR UINT_MAX

/* When a length of a string object stored on disk has the first two bits
 * set, the remaining two bits specify a special encoding for the object
 * accordingly to the following defines: */
#define REDIS_RDB_ENC_INT8 0        /* 8 bit signed integer */
#define REDIS_RDB_ENC_INT16 1       /* 16 bit signed integer */
#define REDIS_RDB_ENC_INT32 2       /* 32 bit signed integer */
#define REDIS_RDB_ENC_LZF 3         /* string compressed with FASTLZ */

/* AOF states */
#define REDIS_AOF_OFF 0             /* AOF is off */
#define REDIS_AOF_ON 1              /* AOF is on */
#define REDIS_AOF_WAIT_REWRITE 2    /* AOF waits rewrite to start appending */




/* List related stuff */
#define REDIS_HEAD 0
#define REDIS_TAIL 1

/* Sort operations */
#define REDIS_SORT_GET 0
#define REDIS_SORT_ASC 1
#define REDIS_SORT_DESC 2
#define REDIS_SORTKEY_MAX 1024

/* Log levels */
#define REDIS_DEBUG 0
#define REDIS_VERBOSE 1
#define REDIS_NOTICE 2
#define REDIS_WARNING 3
#define REDIS_LOG_RAW (1<<10) /* Modifier to log without timestamp */
#define REDIS_DEFAULT_VERBOSITY REDIS_NOTICE

/* Anti-warning macro... */
#define REDIS_NOTUSED(V) ((void) V)

#define ZSKIPLIST_MAXLEVEL 32 /* Should be enough for 2^32 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */

/* Append only defines */
#define AOF_FSYNC_NO 0
#define AOF_FSYNC_ALWAYS 1
#define AOF_FSYNC_EVERYSEC 2
#define REDIS_DEFAULT_AOF_FSYNC AOF_FSYNC_EVERYSEC

/* Zip structure related defaults */
#define REDIS_HASH_MAX_ZIPLIST_ENTRIES 512
#define REDIS_HASH_MAX_ZIPLIST_VALUE 64
#define REDIS_LIST_MAX_ZIPLIST_ENTRIES 512
#define REDIS_LIST_MAX_ZIPLIST_VALUE 64
#define REDIS_SET_MAX_INTSET_ENTRIES 512
#define REDIS_ZSET_MAX_ZIPLIST_ENTRIES 128
#define REDIS_ZSET_MAX_ZIPLIST_VALUE 64

/* HyperLogLog defines */
#define REDIS_DEFAULT_HLL_SPARSE_MAX_BYTES 3000

/* Sets operations codes */
#define REDIS_OP_UNION 0
#define REDIS_OP_DIFF 1
#define REDIS_OP_INTER 2

/* Redis maxmemory strategies */
#define REDIS_MAXMEMORY_VOLATILE_LRU 0
#define REDIS_MAXMEMORY_VOLATILE_TTL 1
#define REDIS_MAXMEMORY_VOLATILE_RANDOM 2
#define REDIS_MAXMEMORY_ALLKEYS_LRU 3
#define REDIS_MAXMEMORY_ALLKEYS_RANDOM 4
#define REDIS_MAXMEMORY_NO_EVICTION 5
#define REDIS_DEFAULT_MAXMEMORY_POLICY REDIS_MAXMEMORY_NO_EVICTION

/* Scripting */
#define REDIS_LUA_TIME_LIMIT 5000 /* milliseconds */

/* Units */
#define UNIT_SECONDS 0
#define UNIT_MILLISECONDS 1

/* SHUTDOWN flags */
#define REDIS_SHUTDOWN_SAVE 1       /* Force SAVE on SHUTDOWN even if no save
                                       points are configured. */
#define REDIS_SHUTDOWN_NOSAVE 2     /* Don't SAVE on SHUTDOWN. */

/* Command call flags, see call() function */
#define REDIS_CALL_NONE 0
#define REDIS_CALL_SLOWLOG 1
#define REDIS_CALL_STATS 2
#define REDIS_CALL_PROPAGATE 4
#define REDIS_CALL_FULL (REDIS_CALL_SLOWLOG | REDIS_CALL_STATS | REDIS_CALL_PROPAGATE)

/* Command propagation flags, see propagate() function */
#define REDIS_PROPAGATE_NONE 0
#define REDIS_PROPAGATE_AOF 1
#define REDIS_PROPAGATE_REPL 2

/* Keyspace changes notification classes. Every class is associated with a
 * character for configuration purposes. */
#define REDIS_NOTIFY_KEYSPACE (1<<0)    /* K */
#define REDIS_NOTIFY_KEYEVENT (1<<1)    /* E */
#define REDIS_NOTIFY_GENERIC (1<<2)     /* g */
#define REDIS_NOTIFY_STRING (1<<3)      /* $ */
#define REDIS_NOTIFY_LIST (1<<4)        /* l */
#define REDIS_NOTIFY_SET (1<<5)         /* s */
#define REDIS_NOTIFY_HASH (1<<6)        /* h */
#define REDIS_NOTIFY_ZSET (1<<7)        /* z */
#define REDIS_NOTIFY_EXPIRED (1<<8)     /* x */
#define REDIS_NOTIFY_EVICTED (1<<9)     /* e */
#define REDIS_NOTIFY_ALL (REDIS_NOTIFY_GENERIC | REDIS_NOTIFY_STRING | REDIS_NOTIFY_LIST | REDIS_NOTIFY_SET | REDIS_NOTIFY_HASH | REDIS_NOTIFY_ZSET | REDIS_NOTIFY_EXPIRED | REDIS_NOTIFY_EVICTED)      /* A */

/* Using the following macro you can run code inside serverCron() with the
 * specified period, specified in milliseconds.
 * The actual resolution depends on server.hz. */
#define redisAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (_redisAssertWithInfo(_c,_o,#_e,__FILE__,__LINE__),_exit(1)))

/* We can print the stacktrace, so our assert is defined this way: */
#define redisAssert(_e) ((_e)?(void)0 : (_redisAssert(#_e,__FILE__,__LINE__),_exit(1)))
#define redisPanic(_e) _redisPanic(#_e,__FILE__,__LINE__),_exit(1)

/*-----------------------------------------------------------------------------
 * Data types
 *----------------------------------------------------------------------------*/

typedef long long mstime_t; /* millisecond time type. */

/* A redis object, that is a type able to hold a string / list / set */

/* The actual Redis Object */
#define REDIS_LRU_BITS 24
#define REDIS_LRU_CLOCK_MAX ((1<<REDIS_LRU_BITS)-1) /* Max value of obj->lru */
#define REDIS_LRU_CLOCK_RESOLUTION 1000 /* LRU clock resolution in ms */
typedef struct redisObject {
    unsigned type:4;
    unsigned encoding:4;
    unsigned lru:REDIS_LRU_BITS; /* lru time (relative to server.lruclock) */
    int refcount;
    void *ptr;
} robj;



/* Macro used to obtain the current LRU clock.
 * If the current resolution is lower than the frequency we refresh the
 * LRU clock (as it should be in production servers) we return the
 * precomputed value, otherwise we need to resort to a function call. */
#define LRU_CLOCK() ((1000/10 <= REDIS_LRU_CLOCK_RESOLUTION) ? server.lruclock : getLRUClock())




/* Client MULTI/EXEC state */
typedef struct multiCmd {
    robj **argv;
    int argc;
    struct redisCommand *cmd;
} multiCmd;

typedef struct multiState {
    multiCmd *commands;     /* Array of MULTI commands */
    int count;              /* Total number of MULTI commands */
    int minreplicas;        /* MINREPLICAS for synchronous replication */
    time_t minreplicas_timeout; /* MINREPLICAS timeout as unixtime. */
} multiState;

/* This structure holds the blocking operation state for a client.
 * The fields used depend on client->btype. */
typedef struct blockingState {
    /* Generic fields. */
    mstime_t timeout;       /* Blocking operation timeout. If UNIX current time
                             * is > timeout then the operation timed out. */

    /* REDIS_BLOCK_LIST */
    dict *keys;             /* The keys we are waiting to terminate a blocking
                             * operation such as BLPOP. Otherwise NULL. */
    robj *target;           /* The key that should receive the element,
                             * for BRPOPLPUSH. */

    /* REDIS_BLOCK_WAIT */
    int numreplicas;        /* Number of replicas we are waiting for ACK. */
    long long reploffset;   /* Replication offset to reach. */
} blockingState;



/* With multiplexing we need to take per-client state.
 * Clients are taken in a liked list. */
typedef struct redisClient {
    int fd;
    int dictid;
    robj *name;             /* As set by CLIENT SETNAME */
    sds querybuf;
    size_t querybuf_peak;   /* Recent (100ms or more) peak of querybuf size */
    int argc;
    robj **argv;
    struct redisCommand *cmd, *lastcmd;
    int reqtype;
    int multibulklen;       /* number of multi bulk arguments left to read */
    long bulklen;           /* length of bulk argument in multi bulk request */
    list *reply;
    unsigned long reply_bytes; /* Tot bytes of objects in reply list */
    int sentlen;            /* Amount of bytes already sent in the current
                               buffer or object being sent. */
    time_t ctime;           /* Client creation time */
    time_t lastinteraction; /* time of the last interaction, used for timeout */
    time_t obuf_soft_limit_reached_time;
    int flags;              /* REDIS_SLAVE | REDIS_MONITOR | REDIS_MULTI ... */
    int authenticated;      /* when requirepass is non-NULL */
    int replstate;          /* replication state if this is a slave */
    int repldbfd;           /* replication DB file descriptor */
    off_t repldboff;        /* replication DB file offset */
    off_t repldbsize;       /* replication DB file size */
    sds replpreamble;       /* replication DB preamble. */
    long long reploff;      /* replication offset if this is our master */
    long long repl_ack_off; /* replication ack offset, if this is a slave */
    long long repl_ack_time;/* replication ack time, if this is a slave */
    char replrunid[REDIS_RUN_ID_SIZE+1]; /* master run id if this is a master */
    int slave_listening_port; /* As configured with: SLAVECONF listening-port */
    multiState mstate;      /* MULTI/EXEC state */
    int btype;              /* Type of blocking op if REDIS_BLOCKED. */
    blockingState bpop;     /* blocking state */
    long long woff;         /* Last write global replication offset. */
    list *watched_keys;     /* Keys WATCHED for MULTI/EXEC CAS */
    dict *pubsub_channels;  /* channels a client is interested in (SUBSCRIBE) */
    list *pubsub_patterns;  /* patterns a client is interested in (SUBSCRIBE) */

    /* Response buffer */
    int bufpos;
    char buf[REDIS_REPLY_CHUNK_BYTES];
} redisClient;

struct saveparam {
    time_t seconds;
    int changes;
};

struct sharedObjectsStruct {
    robj *crlf, *ok, *err, *emptybulk, *czero, *cone, *cnegone, *pong, *space,
    *colon, *nullbulk, *nullmultibulk, *queued,
    *emptymultibulk, *wrongtypeerr, *nokeyerr, *syntaxerr, *sameobjecterr,
    *outofrangeerr, *noscripterr, *loadingerr, *slowscripterr, *bgsaveerr,
    *masterdownerr, *roslaveerr, *execaborterr, *noautherr, *noreplicaserr,
    *oomerr, *plus, *messagebulk, *pmessagebulk, *subscribebulk,
    *unsubscribebulk, *psubscribebulk, *punsubscribebulk, *del, *rpop, *lpop,
    *lpush, *emptyscan, *minstring, *maxstring,
    *select[REDIS_SHARED_SELECT_CMDS],
    *integers[REDIS_SHARED_INTEGERS],
    *mbulkhdr[REDIS_SHARED_BULKHDR_LEN], /* "*<value>\r\n" */
    *bulkhdr[REDIS_SHARED_BULKHDR_LEN];  /* "$<value>\r\n" */
};

/* ZSETs use a specialized version of Skiplists */
typedef struct zskiplistNode {
    robj *obj;
    double score;
    struct zskiplistNode *backward;
    struct zskiplistLevel {
        struct zskiplistNode *forward;
        unsigned int span;
    } level[];
} zskiplistNode;

typedef struct zskiplist {
    struct zskiplistNode *header, *tail;
    unsigned long length;
    int level;
} zskiplist;

typedef struct zset {
    dict *dict;
    zskiplist *zsl;
} zset;






struct redisServer {
    /* General */
    int hz;                     /* serverCron() calls frequency in hertz */
    unsigned lruclock:REDIS_LRU_BITS; /* Clock for LRU eviction */
 
 
    /* RDB / AOF loading information */
    int loading;                /* We are loading data from disk if true */
    off_t loading_total_bytes;
    off_t loading_loaded_bytes;
    time_t loading_start_time;
    off_t loading_process_events_interval_bytes;



    int active_expire_enabled;      /* Can be disabled for testing purposes. */
 
    int rdb_compression;            /* Use compression in RDB? */
    int rdb_checksum;               /* Use RDB checksum? */
    time_t lastsave;                /* Unix time of last successful save */
    time_t lastbgsave_try;          /* Unix time of last attempted bgsave */
    time_t rdb_save_time_last;      /* Time used by last RDB save run. */
    time_t rdb_save_time_start;     /* Current RDB save start time. */
    int lastbgsave_status;          /* REDIS_OK or REDIS_ERR */
    int stop_writes_on_bgsave_err;  /* Don't allow writes if can't BGSAVE */
    /* Logging */
    char *logfile;                  /* Path of log file */
 
    /* Replication (slave) */
    char *masterhost;               /* Hostname of master */
 
    /* Limits */
    int maxclients;                 /* Max number of simultaneous clients */
    unsigned long long maxmemory;   /* Max number of memory bytes to use */
    int maxmemory_policy;           /* Policy for key eviction */
    int maxmemory_samples;          /* Pricision of random sampling */
    /* Blocked clients */
    unsigned int bpop_blocked_clients; /* Number of clients blocked by lists */
    list *unblocked_clients; /* list of clients to unblock before next loop */
    list *ready_keys;        /* List of readyList structures for BLPOP & co */
   
    /* Zip structure config, see redis.conf for more information  */
    size_t hash_max_ziplist_entries;
    size_t hash_max_ziplist_value;
    size_t list_max_ziplist_entries;
    size_t list_max_ziplist_value;
    size_t set_max_intset_entries;
    size_t zset_max_ziplist_entries;
    size_t zset_max_ziplist_value;
    size_t hll_sparse_max_bytes;
    time_t unixtime;        /* Unix time sampled every cron cycle. */
    long long mstime;       /* Like 'unixtime' but with milliseconds resolution. */

    /* Assert & bug reporting */
    char *assert_failed;
    char *assert_file;
    int assert_line;
    int bug_report_start; /* True if bug report header was already logged. */
    int watchdog_period;  /* Software watchdog period in ms. 0 = off */
};

/* Structure to hold list iteration abstraction. */
typedef struct {
    robj *subject;
    unsigned char encoding;
    unsigned char direction; /* Iteration direction */
    unsigned char *zi;
    listNode *ln;
} listTypeIterator;

/* Structure for an entry while iterating over a list. */
typedef struct {
    listTypeIterator *li;
    unsigned char *zi;  /* Entry in ziplist */
    listNode *ln;       /* Entry in linked list */
} listTypeEntry;

/* Structure to hold set iteration abstraction. */
typedef struct {
    robj *subject;
    int encoding;
    int ii; /* intset iterator */
    dictIterator *di;
} setTypeIterator;

/* Structure to hold hash iteration abstraction. Note that iteration over
 * hashes involves both fields and values. Because it is possible that
 * not both are required, store pointers in the iterator to avoid
 * unnecessary memory allocation for fields/values. */
typedef struct {
    robj *subject;
    int encoding;

    unsigned char *fptr, *vptr;

    dictIterator *di;
    dictEntry *de;
} hashTypeIterator;

#define REDIS_HASH_KEY 1
#define REDIS_HASH_VALUE 2

/*-----------------------------------------------------------------------------
 * Extern declarations
 *----------------------------------------------------------------------------*/

extern struct redisServer server;
extern struct sharedObjectsStruct shared;
extern dictType setDictType;
extern dictType zsetDictType;
extern dictType clusterNodesDictType;
extern dictType clusterNodesBlackListDictType;
extern dictType dbDictType;
extern dictType shaScriptObjectDictType;
extern double R_Zero, R_PosInf, R_NegInf, R_Nan;
extern dictType hashDictType;
extern dictType replScriptCacheDictType;

/*-----------------------------------------------------------------------------
 * Functions prototypes
 *----------------------------------------------------------------------------*/

/* Utils */
long long ustime(void);
long long mstime(void);
void getRandomHexChars(char *p, unsigned int len);
uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l);
void exitFromChild(int retcode);
size_t redisPopcount(void *s, long count);
void redisSetProcTitle(char *title);

/* networking.c -- Networking and Client related operations */
redisClient *createClient(int fd);
void closeTimedoutClients(void);
void freeClient(redisClient *c);
void freeClientAsync(redisClient *c);
void resetClient(redisClient *c);
void addReply(redisClient *c, robj *obj);
void *addDeferredMultiBulkLength(redisClient *c);
void setDeferredMultiBulkLength(redisClient *c, void *node, long length);



/* List data type */
void listTypeTryConversion(robj *subject, robj *value);
void listTypePush(robj *subject, robj *value, int where);
robj *listTypePop(robj *subject, int where);
unsigned long listTypeLength(robj *subject);
listTypeIterator *listTypeInitIterator(robj *subject, long index, unsigned char direction);
void listTypeReleaseIterator(listTypeIterator *li);
int listTypeNext(listTypeIterator *li, listTypeEntry *entry);
robj *listTypeGet(listTypeEntry *entry);
void listTypeInsert(listTypeEntry *entry, robj *value, int where);
int listTypeEqual(listTypeEntry *entry, robj *o);
void listTypeDelete(listTypeEntry *entry);
void listTypeConvert(robj *subject, int enc);


/* Redis object implementation */
void decrRefCount(robj *o);
void decrRefCountVoid(void *o);
void incrRefCount(robj *o);
robj *resetRefCount(robj *obj);
void freeStringObject(robj *o);
void freeListObject(robj *o);
void freeSetObject(robj *o);
void freeZsetObject(robj *o);
void freeHashObject(robj *o);
robj *createObject(int type, void *ptr);
robj *createStringObject(char *ptr, size_t len);
robj *createRawStringObject(char *ptr, size_t len);
robj *createEmbeddedStringObject(char *ptr, size_t len);
robj *dupStringObject(robj *o);
int isObjectRepresentableAsLongLong(robj *o, long long *llongval);
robj *tryObjectEncoding(robj *o);
robj *getDecodedObject(robj *o);
size_t stringObjectLen(robj *o);
robj *createStringObjectFromLongLong(long long value);
robj *createStringObjectFromLongDouble(long double value);
robj *createListObject(void);
robj *createZiplistObject(void);
robj *createSetObject(void);
robj *createIntsetObject(void);
robj *createHashObject(void);
robj *createZsetObject(void);
robj *createZsetZiplistObject(void);

int checkType(redisClient *c, robj *o, int type);
char *strEncoding(int encoding);
int compareStringObjects(robj *a, robj *b);
int equalStringObjects(robj *a, robj *b);
#define sdsEncodedObject(objptr) (objptr->encoding == REDIS_ENCODING_RAW || objptr->encoding == REDIS_ENCODING_EMBSTR)

/* Generic persistence functions */
void startLoading(FILE *fp);
void loadingProgress(off_t pos);
void stopLoading(void);
void _redisAssertWithInfo(redisClient *c, robj *o, char *estr, char *file, int line);
unsigned int getLRUClock(void);
void redisLog(int level, const char *fmt, ...);

/* RDB persistence */
#include "rdb.h"

/* Sorted sets data type */

/* Struct to hold a inclusive/exclusive range spec by score comparison. */
typedef struct {
    double min, max;
    int minex, maxex; /* are min or max exclusive? */
} zrangespec;

/* Struct to hold an inclusive/exclusive range spec by lexicographic comparison. */
typedef struct {
    robj *min, *max;  /* May be set to shared.(minstring|maxstring) */
    int minex, maxex; /* are min or max exclusive? */
} zlexrangespec;

zskiplist *zslCreate(void);
void zslFree(zskiplist *zsl);
zskiplistNode *zslInsert(zskiplist *zsl, double score, robj *obj);
unsigned int zsetLength(robj *zobj);
void zsetConvert(robj *zobj, int encoding);


/* Set data type */
void setTypeConvert(robj *subject, int enc);
unsigned long setTypeSize(robj *subject);

/* Hash data type */
void hashTypeConvert(robj *o, int enc);
unsigned long hashTypeLength(robj *o);

/* db.c -- Keyspace access API */
/* Debugging stuff */
void _redisAssert(char *estr, char *file, int line);
void _redisPanic(char *msg, char *file, int line);

int htNeedsResize(dict *dict);
void updateCachedTime();
void redisLog(int level, const char *fmt, ...);
void redisLogFromHandler(int level, const char *msg);
void redisLogHexDump(int level, char *descr, void *value, size_t len);
void redisLogRaw(int level, const char *msg);



#endif


