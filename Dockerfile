FROM grpc/cxx

# RUN apt-get update && apt-get install -y wget && rm -rf /var/lib/apt/lists/*
# RUN apt-get update && apt install -y build-essential && rm -rf /var/lib/apt/lists/*
# RUN apt-get update && apt install -y python3
# RUN apt-get update && apt-get -y install ninja-build
# RUN apt-get update && apt-get -y install cmake
# RUN apt-get update && apt-get -y install git

####################################################################################################
####################################################################################################
####################################################################################################

ENV IS_FROM_DOCKER 1
# EXPOSE 44322
# EXPOSE 44323

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
