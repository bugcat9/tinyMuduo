# tinyMuduo

## 环境

建议使用Ubuntu相关的环境，需要安装一下g++、cmake、make、boost库、boost测试库

```sh
sudo apt install g++ cmake make libboost-dev libboost-test-dev libmysqlclient-dev
```

还需要安装mysql，这个可以使用docker进行简单安装

## 编译

```
 ./build.sh
```

## 运行

```sh
./build/bin/main
```

需要将resources移到到/build/bin目录下

