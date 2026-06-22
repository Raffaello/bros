FROM fedora:latest

RUN dnf -y update
RUN dnf -y install \
    gcc \
    gdb \
    mtools \
    qemu \
    bochs

WORKDIR /code

# docker build -t bros .
# docker run --rm -ti -v .:/code bros /bin/bash
# qemu-system-i386 -no-reboot -display curses -drive file=br-dos.img,if=floppy,format=raw

