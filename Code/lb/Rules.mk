include $(MK)/header.mk

TARGETS := libHemeLbMethod.$(LIBEXT)

SUBDIRS := collisions

$(TARGETS)_DEPS = $(SUBDIRS_TGTS) \
                   lb.o \
                   io.o \
                   LocalLatticeData.o \
                   StabilityTester.o

INCLUDES_$(d) := $(INCLUDES_$(parent))

include $(MK)/footer.mk