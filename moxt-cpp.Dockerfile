# 使用基于 Ubuntu 22.04 的基础镜像
FROM ubuntu:22.04

# 更新软件包列表并安装基本工具
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    wget

# 安装 GCC 11
RUN apt-get install -y software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
    apt-get update && \
    apt-get install -y gcc-11 g++-11 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 90 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 90

# 安装 xmake 依赖项目
RUN apt-get install -y p7zip-full

# 安装 xmake
RUN wget https://xmake.io/shget.text -O - | bash

# 设置环境变量
ENV XMAKE_PROFILE_PATH /root/.xmake/profile

# 将 source 命令添加到 .bashrc
RUN echo 'source /root/.xmake/profile' >> ~/.bashrc

COPY . /root/moxt-cpp

# 克隆 moxt-cpp 项目
# RUN git clone https://github.com/f0cii/moxt-cpp.git /root/moxt-cpp

# 安装 moxt-cpp 项目依赖
RUN apt-get install -y pkg-config e2fslibs-dev zlib1g-dev libssl-dev libcurl4-openssl-dev libaio-dev

# 切换到 moxt-cpp 目录并执行 xmake 编译
RUN /bin/bash -c "source $XMAKE_PROFILE_PATH && cd /root/moxt-cpp && xmake f -y --root && xmake --root"

# 设置容器启动命令
CMD ["/bin/bash"]
