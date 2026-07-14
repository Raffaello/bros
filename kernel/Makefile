# BUILD_DIR:=${BUILD_DIR}
BIN_DIR:=../${BIN_DIR}
FLOPPY_IMAGE_NAME:=../${FLOPPY_IMAGE_NAME}

SRC_DIR=src

# SRC = $(wildcard \
# 	${SRC_DIR}/*.c \
# 	${SRC_DIR}/lib/*.c \
# 	${SRC_DIR}/arch/x86/*.c \
# 	${SRC_DIR}/arch/x86/mmu/*.c \
# 	${SRC_DIR}/bios/*.c \
# 	${SRC_DIR}/drivers/*.c \
# 	${SRC_DIR}/sys/*.c)

SRC = \
	${SRC_DIR}/_start.c \
	${SRC_DIR}/main.c \
	${SRC_DIR}/lib/bitset.c \
	${SRC_DIR}/lib/conio.c \
	${SRC_DIR}/lib/ctype.c \
	${SRC_DIR}/lib/stdlib.c \
	${SRC_DIR}/lib/string.c \
	${SRC_DIR}/arch/x86/APIC.c \
	${SRC_DIR}/arch/x86/cpuid.c \
	${SRC_DIR}/arch/x86/descriptors.c \
	${SRC_DIR}/arch/x86/io.c \
	${SRC_DIR}/arch/x86/ISR_IRQ.c \
	${SRC_DIR}/arch/x86/PIC.c \
	${SRC_DIR}/arch/x86/PIT.c \
	${SRC_DIR}/arch/x86/mmu/PMM.c \
	${SRC_DIR}/arch/x86/mmu/VMM.c \
	${SRC_DIR}/bios/boot_info.c \
	${SRC_DIR}/bios/vga.c \
	${SRC_DIR}/drivers/ps2.c \
	${SRC_DIR}/drivers/keyboard.c \
	${SRC_DIR}/drivers/fdc.c \
	${SRC_DIR}/sys/panic.c

OBJS = $(SRC:${SRC_DIR}/%.c=${BUILD_DIR}/%.o)

# SRC_S  = $(wildcard \
# 	${SRC_DIR}/arch/x86/lib/*.S \
# 	${SRC_DIR}/arch/x86/*.S \
# 	${SRC_DIR}/arch/x86/mmu/*.S \
# 	${SRC_DIR}/drivers/*.S)

SRC_S = \
	${SRC_DIR}/arch/x86/lib/stdlib.S \
	${SRC_DIR}/arch/x86/lib/string.S \
	${SRC_DIR}/arch/x86/GDT.S \
	${SRC_DIR}/arch/x86/IRQ.S \
	${SRC_DIR}/arch/x86/ISR.S \
	${SRC_DIR}/arch/x86/mmu/VMM.S \


OBJS_S = $(SRC_S:${SRC_DIR}/%.S=${BUILD_DIR}/%.oS)

INCLUDE_DIR=${SRC_DIR}

CFLAGS+=-Wall -Werror #-Wmissing-prototypes
CFLAGS+=-masm=intel
# CFLAGS+=-O2	# optimization are creating issues on the code
# CFLAGS+=-pedantic
CFLAGS+=-std=c17
# CFLAGS+=-m32
CFLAGS+=-c -ffreestanding -I ${INCLUDE_DIR}
CFLAGS+=-fno-pic -mno-red-zone # -fno-stack-protector
CFLAGS+=-fno-exceptions # -fno-rtti # C++ options disabled.
# CFLAGS+= -MMD -MF
CFLAGS+=-nostdlib -nodefaultlibs -nostartfiles
CFLAGS+=-lgcc
CFLAGS+=-DKERNEL_SEG=${KERNEL_SEG} -DKERNEL_HIMEM=${KERNEL_HIMEM}
CFLAGS+=-fno-PIE # Github CI flags
# CFLAGS+=-fno-omit-frame-pointer # -fno-optimize-sibling-calls

CFLAGS.debug+=-g -DDEBUG
CFLAGS.release+=-O2 -DNDEBUG
CFLAGS.reldbg=-g $(CFLAGS.release)

CFLAGS+=$(CFLAGS.$(BUILD_TYPE))


# LFLAGS+=-m elf_i386 # change when starting the kernel in long mode
LFLAGS+=-nostdlib --nmagic
LFLAGS+=-Tkernel.ld # linker option definition file
# LFLAGS+=-Ttext ${KERNEL_SEG}
LFLAGS+=--defsym=KERNEL_SEG=$(KERNEL_SEG) --defsym=KERNEL_HIMEM=${KERNEL_HIMEM}

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
	objcopy -O binary ${BUILD_DIR}/kernel.out ${BIN_DIR}/kernel.sys
	cp ${BUILD_DIR}/kernel.out ${BUILD_DIR}/../kernel.out

image: kernel
	-mattrib -i ${FLOPPY_IMAGE_NAME} -r -h -s -a /${KERNEL_FILENAME}
	mcopy -o -i ${FLOPPY_IMAGE_NAME} -D o ${BIN_DIR}/kernel.sys ::/${KERNEL_FILENAME}
	mattrib -i ${FLOPPY_IMAGE_NAME} +r +h +s -a /${KERNEL_FILENAME}

clean:
	rm ${BUILD_DIR}/* -frv
	rm ${BUILD_DIR}/../kernel.out -fv
