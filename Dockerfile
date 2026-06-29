FROM fedora:44

RUN dnf -y update
RUN dnf -y install \
    gcc \
    # g++ \
    gdb \
    mtools \
    qemu \
    bochs \
    bochs-debugger \
    iproute \
    ps \
    curl \
    libvncserver-devel

# RUN curl -O -L https://sourceforge.net/projects/bochs/files/bochs/3.0/bochs-3.0.tar.gz && \
#     tar xf bochs-3.0.tar.gz && \
#     cd bochs-3.0 && \
#     ./configure --with-vncsrv && \
#     make && \
#     make install


WORKDIR /code

# docker build -t bros .
# docker run --rm -ti -p 1234:1234 -p 5901:5901 -v .:/code bros /bin/bash

# qemu-system-i386 -no-reboot -display curses -drive file=br-dos.img,if=floppy,format=raw -s -S

### use VNC for PS/2 keyboard, VNC connect to localhost:5901
# qemu-system-i386 -no-reboot -vnc :1 -drive file=br-dos.img,if=floppy,format=raw -s -S

