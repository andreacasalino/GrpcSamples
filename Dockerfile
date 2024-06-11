FROM ubuntu

# https://github.com/plusangel/grpc-cplusplus-ubuntu18/blob/master/Dockerfile

RUN apt-get update && apt-get install -y \
build-essential \
gdb \
autoconf \
libtool \
git \
pkg-config \
libz-dev \
curl \
automake \
make \
unzip \
cmake \
python3 \
ninja-build \
&& apt-get clean

RUN git clone --recurse-submodules -b v1.64.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc 
WORKDIR "/grpc"
RUN mkdir -p cmake/build
WORKDIR "/grpc/cmake/build"
RUN cmake \
-G Ninja \
-DgRPC_INSTALL=ON \
-DgRPC_BUILD_TESTS=OFF \
../..
RUN ninja -j 4
RUN ninja install
WORKDIR "/"

ENV PATH="/usr/local/bin:${PATH}"

####################################################################################################
####################################################################################################
####################################################################################################

ENV IS_FROM_DOCKER 1
ENV ECHO_SERVER_PORT 44332
ENV ORDER_BOOK_SERVER_PORT 44333

####################################################################################################
####################################################################################################
####################################################################################################

# COPY ./src  ./src

# ARG CMAKE_INSTALL_PREFIX=/usr/bin/apps

# WORKDIR "/src"
# RUN cmake -B./build -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX} -G Ninja -DBUILD_TESTS=OFF -DCMAKE_CONFIGURATION_TYPES="Release" -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
# RUN cmake --build ./build --config Release
# RUN cmake --install ./build --config Release
# WORKDIR "/"
# RUN rm -rf "/src"
# ENV VOLI_CONFIG_FOLDER=${CMAKE_INSTALL_PREFIX}/config
# ENV PATH="${CMAKE_INSTALL_PREFIX}/bin:${PATH}"
