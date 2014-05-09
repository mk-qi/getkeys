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

#include "redis.h"
//#include "sha1.h"   /* SHA1 is used for DEBUG DIGEST */
#include "crc64.h"

#include <arpa/inet.h>
#include <signal.h>

/* =========================== Crash handling  ============================== */

void _redisAssert(char *estr, char *file, int line) {
    redisLog(REDIS_WARNING,"=== ASSERTION FAILED ===");
    redisLog(REDIS_WARNING,"==> %s:%d '%s' is not true",file,line,estr);
    *((char*)-1) = 'x';
}

void _redisAssertPrintClientInfo(redisClient *c) {
    int j;

    redisLog(REDIS_WARNING,"=== ASSERTION FAILED CLIENT CONTEXT ===");
    redisLog(REDIS_WARNING,"client->flags = %d", c->flags);
    redisLog(REDIS_WARNING,"client->fd = %d", c->fd);
    redisLog(REDIS_WARNING,"client->argc = %d", c->argc);
    for (j=0; j < c->argc; j++) {
        char buf[128];
        char *arg;

        if (c->argv[j]->type == REDIS_STRING && sdsEncodedObject(c->argv[j])) {
            arg = (char*) c->argv[j]->ptr;
        } else {
            snprintf(buf,sizeof(buf),"Object type: %d, encoding: %d",
                c->argv[j]->type, c->argv[j]->encoding);
            arg = buf;
        }
        redisLog(REDIS_WARNING,"client->argv[%d] = \"%s\" (refcount: %d)",
            j, arg, c->argv[j]->refcount);
    }
}

void redisLogObjectDebugInfo(robj *o) {
    redisLog(REDIS_WARNING,"Object type: %d", o->type);
    redisLog(REDIS_WARNING,"Object encoding: %d", o->encoding);
    redisLog(REDIS_WARNING,"Object refcount: %d", o->refcount);
    if (o->type == REDIS_STRING && sdsEncodedObject(o)) {
        redisLog(REDIS_WARNING,"Object raw string len: %zu", sdslen(o->ptr));
        if (sdslen(o->ptr) < 4096) {
            sds repr = sdscatrepr(sdsempty(),o->ptr,sdslen(o->ptr));
            redisLog(REDIS_WARNING,"Object raw string content: %s", repr);
            sdsfree(repr);
        }
    } else if (o->type == REDIS_LIST) {
        redisLog(REDIS_WARNING,"List length: %d", (int) listTypeLength(o));
    } else if (o->type == REDIS_SET) {
        redisLog(REDIS_WARNING,"Set size: %d", (int) setTypeSize(o));
    } else if (o->type == REDIS_HASH) {
        redisLog(REDIS_WARNING,"Hash size: %d", (int) hashTypeLength(o));
    } else if (o->type == REDIS_ZSET) {
        redisLog(REDIS_WARNING,"Sorted set size: %d", (int) zsetLength(o));
        if (o->encoding == REDIS_ENCODING_SKIPLIST)
            redisLog(REDIS_WARNING,"Skiplist level: %d", (int) ((zset*)o->ptr)->zsl->level);
    }
}

void _redisAssertPrintObject(robj *o) {
    redisLog(REDIS_WARNING,"=== ASSERTION FAILED OBJECT CONTEXT ===");
    redisLogObjectDebugInfo(o);
}

void _redisAssertWithInfo(redisClient *c, robj *o, char *estr, char *file, int line) {
    if (c) _redisAssertPrintClientInfo(c);
    if (o) _redisAssertPrintObject(o);
    _redisAssert(estr,file,line);
}

void _redisPanic(char *msg, char *file, int line) {
    redisLog(REDIS_WARNING,"------------------------------------------------");
    redisLog(REDIS_WARNING,"!!! Software Failure. Press left mouse button to continue");
    redisLog(REDIS_WARNING,"Guru Meditation: %s #%s:%d",msg,file,line);
#ifdef HAVE_BACKTRACE
    redisLog(REDIS_WARNING,"(forcing SIGSEGV in order to print the stack trace)");
#endif
    redisLog(REDIS_WARNING,"------------------------------------------------");
    *((char*)-1) = 'x';
}

void bugReportStart(void) {
    if (server.bug_report_start == 0) {
        redisLog(REDIS_WARNING,
            "\n\n=== REDIS BUG REPORT START: Cut & paste starting from here ===");
        server.bug_report_start = 1;
    }
}
/* ==================== Logging functions for debugging ===================== */

void redisLogHexDump(int level, char *descr, void *value, size_t len) {
    char buf[65], *b;
    unsigned char *v = value;
    char charset[] = "0123456789abcdef";

    redisLog(level,"%s (hexdump):", descr);
    b = buf;
    while(len) {
        b[0] = charset[(*v)>>4];
        b[1] = charset[(*v)&0xf];
        b[2] = '\0';
        b += 2;
        len--;
        v++;
        if (b-buf == 64 || len == 0) {
            redisLogRaw(level|REDIS_LOG_RAW,buf);
            b = buf;
        }
    }
    redisLogRaw(level|REDIS_LOG_RAW,"\n");
}

