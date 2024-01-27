# moxt-cpp

moxt-cpp是量化交易库[moxt](https://github.com/f0cii/moxt) (基于编程语言mojo的量化交易库)的一个部分，其是底层C++实现。

## 安装和使用

首先，你需要安装xmake工具。然后，你可以使用以下命令进行编译：

```bash
sudo apt install pkg-config
sudo apt install e2fslibs-dev
sudo apt install zlib1g-dev
sudo apt install libssl-dev libcurl4-openssl-dev libaio-dev

xmake -vD
```

## 克隆项目
git clone --recursive https://github.com/f0cii/moxt-cpp.git

## 更新代码
git pull --recurse-submodules
更新子模块
git submodule update --init --recursive

## 使用 Docker 编译
docker build -t moxt-cpp -f moxt-cpp.Dockerfile .

[![Docker Image CI](https://github.com/f0cii/moxt-cpp/actions/workflows/docker-image.yml/badge.svg)](https://github.com/f0cii/moxt-cpp/actions/workflows/docker-image.yml)

编译后运行docker容器
docker run -it moxt-cpp
查看编译结果
cd /root/moxt-cpp/build/linux/x86_64/release
ls
将显示如下文件，此so文件即为我们项目编译的最终结果
libmoxt.so

## 文档


## 贡献


## 授权

本项目采用 MIT 授权许可 - 请查看 [LICENSE] 文件了解更多细节。

---

**免责声明：** 本项目仅供学习和研究使用，不构成任何交易建议或投资建议。请谨慎使用该项目进行实际交易。

[文档链接]: #  # TODO: 添加文档链接
[贡献指南]: CONTRIBUTING.md # TODO: 创建贡献指南文件
[LICENSE]: LICENSE