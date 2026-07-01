FROM fedora:44 AS build

RUN dnf -y update
RUN dnf -y install \
    gcc \
    g++ \
    bison \
    flex \
    gmp-devel \
    libmpc-devel \
    mpfr-devel \
    texinfo \
    isl-devel \
    gdb \
    diffutils \
    curl

WORKDIR /src

ENV PREFIX="/usr/local/cross"
ENV TARGET=i686-elf
ENV PATH="${PREFIX}/bin:${PATH}"

ARG BINUTILS=binutils-2.46.1
ARG BINUTILS_EXT=.tar.xz
ARG BINUTILS_URL=https://mirrors.dotsrc.org/gnu/binutils/${BINUTILS}${BINUTILS_EXT}

ARG GCC=gcc-16.1.0
ARG GCC_EXT=.tar.xz
ARG GCC_URL=https://mirror.koddos.net/gcc/releases/${GCC}/${GCC}${GCC_EXT}

RUN curl -O -L ${BINUTILS_URL} && \
    curl -O -L ${GCC_URL} 

RUN tar xf ${BINUTILS}${BINUTILS_EXT} && \
    tar xf ${GCC}${GCC_EXT} && \
    rm ${BINUTILS}${BINUTILS_EXT} && \
    rm ${GCC}${GCC_EXT}

RUN mkdir build-binutils && \
    cd build-binutils && \
    ../${BINUTILS}/configure --target=${TARGET} --prefix="${PREFIX}" --with-sysroot --disable-nls --disable-werror --enable-default-execstack=no && \
    make -j4 && \
    make install

RUN mkdir build-gcc && \
    cd build-gcc && \
    ../${GCC}/configure --target=${TARGET} --prefix="${PREFIX}" --disable-nls --enable-languages=c,c++ --without-headers --enable-initfini-array --disable-hosted-libstdcxx && \
    make all-gcc -j4 && \
    make all-target-libgcc -j4 && \
    make all-target-libstdc++-v3 -j4 && \
    make install-gcc && \
    make install-target-libgcc && \
    make install-target-libstdc++-v3


FROM fedora:44

RUN dnf -y update
RUN dnf -y install \
    # gcc \
    # g++ \
    # bison \
    # flex \
    # gmp-devel \
    # libmpc-devel \
    # mpfr-devel \
    # texinfo \
    # isl-devel \
    gdb \
    mtools \
    qemu \
    bochs \
    bochs-debugger
# iproute \
# ps \
# curl \
# libvncserver-devel

# WORKDIR /src

# ENV PREFIX="/usr/local/cross"
# ENV TARGET=i686-elf
# ENV PATH="${PREFIX}/bin:${PATH}"

# ARG BINUTILS=binutils-2.46.1
# ARG BINUTILS_EXT=.tar.xz
# ARG BINUTILS_URL=https://mirrors.dotsrc.org/gnu/binutils/${BINUTILS}${BINUTILS_EXT}

# ARG GCC=gcc-16.1.0
# ARG GCC_EXT=.tar.xz
# ARG GCC_URL=https://mirror.koddos.net/gcc/releases/${GCC}/${GCC}${GCC_EXT}


# RUN curl -O -L ${BINUTILS_URL} && \
#     curl -O -L ${GCC_URL} 

# RUN tar xf ${BINUTILS}${BINUTILS_EXT} && \
#     tar xf ${GCC}${GCC_EXT} && \
#     rm ${BINUTILS}${BINUTILS_EXT} && \
#     rm ${GCC}${GCC_EXT}

# RUN mkdir build-binutils && \
#     cd build-binutils && \
#     ../${BINUTILS}/configure --target=${TARGET} --prefix="${PREFIX}" --with-sysroot --disable-nls --disable-werror --enable-default-execstack=no && \
#     make && \
#     make install

# RUN mkdir build-gcc && \
#     cd build-gcc && \
#     ../${GCC}/configure --target=${TARGET} --prefix="${PREFIX}" --disable-nls --enable-languages=c,c++ --without-headers --enable-initfini-array --disable-hosted-libstdcxx && \
#     make all-gcc && \
#     make all-target-libgcc && \
#     make all-target-libstdc++-v3 && \
#     make install-gcc && \
#     make install-target-libgcc && \
#     make install-target-libstdc++-v3


# RUN curl -O -L https://sourceforge.net/projects/bochs/files/bochs/3.0/bochs-3.0.tar.gz && \
#     tar xf bochs-3.0.tar.gz && \
#     cd bochs-3.0 && \
#     ./configure --with-vncsrv && \
#     make && \
#     make install

COPY --from=build "/usr/local/cross" "/usr/local/cross"

WORKDIR /code

# docker build -t bros .
# docker run --rm -ti -p 1234:1234 -p 5901:5901 -v .:/code bros /bin/bash
# docker run --rm -p 1234:1234 -p 5901:5901 -p5800:5800  -ti -v .:/code:Z bros /bin/bash

# qemu-system-i386 -no-reboot -display curses -drive file=br-dos.img,if=floppy,format=raw -s -S

### use VNC for PS/2 keyboard, VNC connect to localhost:5901
# qemu-system-i386 -no-reboot -vnc :1 -drive file=br-dos.img,if=floppy,format=raw -s -S
