# BUILD_DIR:=${BUILD_DIR}
BIN_DIR:=../${BIN_DIR}
FLOPPY_IMAGE_NAME:=../${FLOPPY_IMAGE_NAME}

SRC_DIR=src

# NOTE: When working on fixing compiling optimization issues the below TODO could be resolved.
# TODO: Furher sub-makefiles might be required to define optimizaions/options to compiler/linker
#       also need to be later on expanded, listing each single file,
#       so a further control at file level could be achieved.
#       for now is ok in this way at the moment.
SRC = $(wildcard \
	${SRC_DIR}/*.c \
	${SRC_DIR}/lib/*.c \
	${SRC_DIR}/cpu/*.c \
	${SRC_DIR}/cpu/mmu/*.c \
	${SRC_DIR}/bios/*.c \
	${SRC_DIR}/drivers/*.c)

OBJS = $(SRC:${SRC_DIR}/%.c=${BUILD_DIR}/%.o)

SRC_S  = $(wildcard \
	${SRC_DIR}/lib/*.S \
	${SRC_DIR}/cpu/*.S \
	${SRC_DIR}/cpu/mmu/*.S \
	${SRC_DIR}/drivers/*.S)

OBJS_S = $(SRC_S:${SRC_DIR}/%.S=${BUILD_DIR}/%.oS)

INCLUDE_DIR=${SRC_DIR}

CFLAGS+=-Wall -Werror #-Wmissing-prototypes
CFLAGS+=-masm=intel
# CFLAGS+=-O2	# optimization are creating issues on the code
CFLAGS+=-g
CFLAGS+=-std=c17
CFLAGS+=-m32 -c -ffreestanding -I ${INCLUDE_DIR} -MMD
CFLAGS+=-nostartfiles -nostdlib
CFLAGS+=-lgcc
CFLAGS+=-DKERNEL_SEG=${KERNEL_SEG}

LFLAGS+=-m elf_i386 # change when starting the kernel in long mode
LFLAGS+=-nostdlib --nmagic
LFLAGS+=-Tkernel.ld # linker option definition file
# LFLAGS+=-Ttext ${KERNEL_SEG}
LFLAGS+=--defsym=KERNEL_SEG=$(KERNEL_SEG)

all: image

echos:
	echo ${SRC}
	echo ${OBJS}
	echo ${SRC_S}
	echo ${OBJS_S}

.SECONDEXPANSION:
$(OBJS): $$(patsubst $(BUILD_DIR)/%.o,$(SRC_DIR)/%.c,$$@)
	@mkdir -p ${@D}
	${CC} $(CFLAGS) -c $^ -o $@

$(OBJS_S): $$(patsubst $(BUILD_DIR)/%.oS,$(SRC_DIR)/%.S,$$@)
	@mkdir -p ${@D}
	${CC} $(CFLAGS) $^ -o $@

# Monolithic for now
kernel: $(OBJS) ${OBJS_S}
	@mkdir -p ${BIN_DIR}
	# ${LD} $(LFLAGS) -o ${BUILD_DIR}/kernel.out ${OBJS} ${OBJS_S}
	# objcopy -O binary  -j .text ${BUILD_DIR}/kernel.out ${BIN_DIR}/kernel.sys
	# ${LD} $(LFLAGS) -o ${BIN_DIR}/kernel.sys ${OBJS} ${OBJS_S}
	${LD} $(LFLAGS) -o ${BUILD_DIR}/kernel.out ${OBJS} ${OBJS_S}
	objcopy -O binary  -j .text ${BUILD_DIR}/kernel.out ${BIN_DIR}/kernel.sys

image: kernel
	mcopy -i ${FLOPPY_IMAGE_NAME} ${BIN_DIR}/kernel.sys ::/${KERNEL_FILENAME}
	mattrib -i ${FLOPPY_IMAGE_NAME} +r +h +s -a /${KERNEL_FILENAME}

clean:
	rm ${BUILD_DIR}/* -frv
