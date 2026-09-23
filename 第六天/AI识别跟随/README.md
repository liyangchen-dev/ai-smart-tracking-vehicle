
## 小车
```shell
python3.10 main.py 192.168.1.1:2002 http://192.168.1.1:8080/?action=stream
```







## 电脑本机测试

```shell
# 开启tcp server
python3.10 ./test_module/tcp_server.py

# 运行主函数
# 第一个参数是TCP服务地址，第二个参数是视频流，0代表本机摄像头
python3.10 main.py 127.0.0.1:10001 0
```

