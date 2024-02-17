<h4 align="center">
    <p>
        <b>English</b> |
        <a href="https://github.com/f0cii/moxt-cpp/blob/main/README_zh.md">简体中文</a>
    </p>
</h4>

# moxt-cpp
moxt-cpp is a part of the quantitative trading library [moxt](https://github.com/f0cii/moxt) (a quantitative trading library based on the programming language Mojo), with its underlying implementation in C++.

## Installation and Usage
First, you need to install the xmake tool. Then, you can compile using the following commands:


```bash
sudo apt install pkg-config
sudo apt install e2fslibs-dev
sudo apt install zlib1g-dev
sudo apt install libssl-dev libcurl4-openssl-dev libaio-dev

xmake -vD
```

## Clone the Project
git clone --recursive https://github.com/f0cii/moxt-cpp.git

## Update the Code
git pull --recurse-submodules

## Update Submodules
git submodule update --init --recursive

## Compile with Docker
docker build -t moxt-cpp -f moxt-cpp.Dockerfile .

[![Docker Image CI](https://github.com/f0cii/moxt-cpp/actions/workflows/docker-image.yml/badge.svg)](https://github.com/f0cii/moxt-cpp/actions/workflows/docker-image.yml)

Run the Docker container after compiling
docker run -it moxt-cpp

View the compilation results
cd /root/moxt-cpp/build/linux/x86_64/release
ls
This will display the following files, the so file is the final result of our project compilation
libmoxt.so

## License

This project is licensed under the MIT License - see the [LICENSE] file for more details.

---

Disclaimer: This project is for learning and research purposes only and does not constitute any trading or investment advice. Please use this project cautiously for actual trading.
