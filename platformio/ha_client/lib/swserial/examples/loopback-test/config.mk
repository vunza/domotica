BOARD=d1_mini
UPLOAD_SPEED=460800
MONITOR_SPEED=115200
FLASH_DEF=4M3M
EXCLUDE_DIRS=$(abspath ../../)/pctest

# SWSERIALLIB=SWSERIAL
# # SWSERIALLIB=SOFTWARESERIAL

# ifeq ($(SWSERIALLIB), SWSERIAL)
# $(info IJSI)
# 	BUILD_EXTRA_FLAGS += -DUSE_SWSERIAL
# endif

# ifeq ($(SWSERIALLIB), SOFTWARESERIAL)
# 	BUILD_EXTRA_FLAGS += -DUSE_SOFTWARESERIAL -I$(abspath ../..)
# endif
