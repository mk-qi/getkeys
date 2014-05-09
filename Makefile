objects = adlist.o  dict.o redis.o sds.o zmalloc.o lzf_c.o lzf_d.o  zipmap.o  ziplist.o  util.o object.o  rdb.o t_list.o t_set.o t_zset.o t_hash.o  debug.o intset.o  crc16.o endianconv.o rio.o  crc64.o 
cc = gcc

rdb : $(objects)
	cc -o rdb $(objects)

sds.o: sds.c sds.h zmalloc.h
adlist.o: adlist.c adlist.h zmalloc.h
crc16.o: crc16.c redis.h fmacros.h    sds.h dict.h adlist.h zmalloc.h  ziplist.h intset.h  util.h rdb.h rio.h
crc64.o: crc64.c
debug.o: debug.c redis.h fmacros.h   sds.h dict.h adlist.h zmalloc.h  \
 ziplist.h intset.h  util.h rdb.h rio.h  crc64.h

dict.o: dict.c fmacros.h dict.h zmalloc.h redisassert.h
endianconv.o: endianconv.c

intset.o: intset.c intset.h zmalloc.h endianconv.h 
lzf_c.o: lzf_c.c lzfP.h
lzf_d.o: lzf_d.c lzfP.h
object.o: object.c redis.h fmacros.h    sds.h dict.h adlist.h zmalloc.h  \
 ziplist.h intset.h  util.h rdb.h rio.h
rdb.o: rdb.c redis.h fmacros.h   sds.h dict.h adlist.h zmalloc.h  \
 ziplist.h intset.h  util.h rdb.h rio.h lzf.h zipmap.h \
 endianconv.h
redis.o: redis.c redis.h fmacros.h   sds.h dict.h adlist.h zmalloc.h  \
 ziplist.h intset.h  util.h rdb.h rio.h 
 
rio.o: rio.c fmacros.h rio.h sds.h util.h crc64.h    dict.h adlist.h \
 zmalloc.h  ziplist.h intset.h  rdb.h
util.o: util.c fmacros.h util.h sds.h
ziplist.o: ziplist.c zmalloc.h util.h sds.h ziplist.h endianconv.h \
  redisassert.h
zipmap.o: zipmap.c zmalloc.h endianconv.h 
zmalloc.o: zmalloc.c  zmalloc.h	
t_hash.o: t_hash.c redis.h fmacros.h    sds.h dict.h adlist.h zmalloc.h  \
 ziplist.h intset.h  util.h rdb.h rio.h
t_list.o: t_list.c redis.h fmacros.h   sds.h dict.h adlist.h zmalloc.h  \
 ziplist.h intset.h  util.h rdb.h rio.h
t_set.o: t_set.c redis.h fmacros.h   sds.h dict.h adlist.h zmalloc.h  \
 ziplist.h intset.h  util.h rdb.h rio.h
t_zset.o: t_zset.c redis.h fmacros.h    sds.h dict.h adlist.h zmalloc.h  \
 ziplist.h intset.h  util.h rdb.h rio.h
.PHONY : clean
clean :
	rm rdb $(objects)


	