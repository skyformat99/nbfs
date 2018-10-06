# nbfs
nebula-im里图片、小视频、语音等小文件存储系统。

特点是：

- 每个数据只写一次；
- 读操作频繁；
- 从不修改；
- 很少删除；

要求：

- 高吞吐量和低延迟：获取文件仅一次磁盘操作；
- 容错：多机房备份；
- 高性价比：

# 参考资料

- [Finding a needle in Haystack: Facebook’s photo storage](http://www.importnew.com/3292.html)
- [小文件存储](https://github.com/Terry-Mao/bfs)