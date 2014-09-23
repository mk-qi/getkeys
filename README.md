rediskeysusage
==============

从redis 的RDB文件分析redis 里的key的数量及占用内存情况,key以:分割，聚合连续的数字为$id，根据最小聚合字符的长度，将大于最小聚合字符长度的连续字符聚合为$seq，并输出KEY的类型，用于分析该redis KEY的分布情况。
