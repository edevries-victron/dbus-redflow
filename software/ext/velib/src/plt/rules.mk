SRCS-$(CFG_WITH_DBUS_SERIAL) += dbus_serial.c
SRCS-$(CFG_WITH_MK2_SERIAL) += mk2_serial.c
SRCS-$(CFG_WITH_CTX)-${POSIX} += posix_ctx.c
SRCS-$(CFG_WITH_SERIAL)-${POSIX} += posix_serial.c
SRCS-$(CFG_WITH_SERIAL) += serial.c
SRCS-$(CFG_WITH_TIMER) += timer.c
SRCS-$(CFG_WITH_VECAN_SERIAL) += vecan_serial.c
SRCS-$(CFG_WITH_CTX)-${WINDOWS} += win_ctx.c
SRCS-$(CFG_WITH_SERIAL)-${WINDOWS} += win_serial.c

files = $(wildcard $d/$(TARGET)/*)

ifneq ($(files),)
SUBDIRS += $(TARGET)
INCLUDES += $d/$(TARGET)
endif
