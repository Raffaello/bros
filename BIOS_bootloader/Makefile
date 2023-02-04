
# BUILD_DIR:=${BUILD_DIR}
BIN_DIR:=../${BIN_DIR}
FLOPPY_IMAGE_NAME:=../${FLOPPY_IMAGE_NAME}

SRC=./src
ASFLAGS+=--fatal-warnings -n --32 -march=i386 -I ${SRC}
AS_LFLAGS+=-m elf_i386

# SRC=floppy.asm boot2.asm

FLOPPY_LABEL=BROS
FLOPPY_SIZE=1440
FAT_RESERVED_SECTORS=4

BIOS_BOOT_SEG=0x7C00
BOOT_REL_SEG=0x600
BOOT2_SEG=${BIOS_BOOT_SEG}
FAT_BUFFER_SEG=0x600 # same as BOOT_REL_SEG as it won't be used anymore
SYS_INFO_SEG=0x600 # it will replace the FAT_BUFFER_SEG after the kernel is loaded.

all: image

clean:
# 	rm ${BIN_DIR}/* -fv
	rm ${BUILD_DIR}/* -frv

floppy:
	@mkdir -p ${BUILD_DIR} ${BIN_DIR}
	${AS} ${ASFLAGS} \
		--defsym=BIOS_BOOT_SEG=${BIOS_BOOT_SEG} \
		--defsym=BOOT_RELOCATE_SEG=${BOOT_REL_SEG} \
		--defsym=BOOT2_SEG=${BOOT2_SEG} \
		--defsym=FAT_RESERVED_SECTORS=${FAT_RESERVED_SECTORS} \
		-o ${BUILD_DIR}/boot.o ${SRC}/floppy.asm
	${LD} ${AS_LFLAGS} -o ${BUILD_DIR}/boot.out ${BUILD_DIR}/boot.o -Ttext ${BOOT_REL_SEG}
	objcopy -O binary -j .text ${BUILD_DIR}/boot.out ${BIN_DIR}/boot.bin

boot2:
	@mkdir -p build
	${AS} ${ASFLAGS} \
		--defsym=KERNEL_SEG=${KERNEL_SEG} \
		--defsym=FAT_BUFFER_SEG=${FAT_BUFFER_SEG} \
		--defsym=SYS_INFO_SEG=${SYS_INFO_SEG} \
		--defsym=FAT_RESERVED_SECTORS=${FAT_RESERVED_SECTORS} \
		-o ${BUILD_DIR}/boot2.o ${SRC}/boot2.asm
	${LD} ${AS_LFLAGS} -o ${BUILD_DIR}/boot2.out ${BUILD_DIR}/boot2.o -Ttext ${BOOT2_SEG}
	objcopy -O binary -j .text ${BUILD_DIR}/boot2.out ${BIN_DIR}/boot2.bin

image: floppy boot2
	mformat -i ${FLOPPY_IMAGE_NAME} -v ${FLOPPY_LABEL} -B ${BIN_DIR}/boot.bin -R ${FAT_RESERVED_SECTORS} -f${FLOPPY_SIZE} -C
	dd if=${BIN_DIR}/boot2.bin of=${FLOPPY_IMAGE_NAME} conv=notrunc seek=1
