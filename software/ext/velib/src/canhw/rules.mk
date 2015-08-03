
#SRCS += canhw_console.c
SRCS-${CFG_WITH_CANHW_DRIVER} += canhw_driver.c
SRCS-${CFG_WITH_CANHW_DRIVER_IMPLEMENATION} += canhw_driver_dynamic.c
SRCS-${CFG_WITH_CANHW_ECAN} += canhw_ecan1.c
SRCS-${CFG_WITH_CANHW_ECAN_INTERRUPT} += canhw_ecan1_int.c
#SRCS += canhw_flexcan1.c
#SRCS += canhw_general.c
SRCS-$(CFG_WITH_CANHW_I527) += canhw_i527.c
#SRCS += canhw_kvaser.c
#SRCS += canhw_lincan.c
SRCS-${CFG_WITH_CANHW_LOOPBACK} += canhw_loopback.c
SRCS-$(CFG_WITH_CANHW_MSCAN) += canhw_mscan.c
SRCS-${CFG_WITH_CANHW_SOCKETCAN} += canhw_socketcan.c
#SRCS += canhw_stats.c

ifneq ($(CFG_WITH_CANHW_DRIVER),)
ifeq ($(TARGET_OS), windows)
LDLIBS += -lShlwapi
endif
endif
