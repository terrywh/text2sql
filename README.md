### 
将下述文件内容，生成（导入）到指定 SQLITE 文件中：

```
(18) [2021-07-17 13:00:49.674360] <0x00007fe2bfb3c980> (trace)	[tvui::Controller::Controller():21] start
(19) [2021-07-17 13:00:49.674377] <0x00007fe2bfb3c980> (trace)	[room::Controller::Controller():18] start
(20) [2021-07-17 13:00:49.674391] <0x00007fe2bfb3c980> (trace)	[int main(int, char**):37] started.
(21) [2021-07-17 13:00:49.885109] <0x00007fe2bfb3c980> (trace)	[void media::session::run(xbond::coroutine_handler&):16] session connected: 11.11.11.11:1111
(22) [2021-07-17 13:00:49.885534] <0x00007fe2bfb3c980> (trace)	[void media::session::run(xbond::coroutine_handler&):16] session connected: 33.33.33.33:3333
```

可考虑使用如下 REGEX 表达式：
```
\((?<id>\d+)\) \[(?<timestamp>[^\]]+)\] <(?<thread_id>[^\]]+)> \((?<severity>[^\)]+)\)\s+\[(?<module>[^\]]+)\]\s+(?<message>.+))
```
