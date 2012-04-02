bin_PROGRAMS=bootconfig
bootconfigdir=

bootconfig_SOURCES= \
	main.c 			\
	lowlevel.c 		\
	logging.c

bootconfig_HEADERS= \
	lowlevel.h		\
	logging.h

TESTS = test-lowlevel-init \
		test-lowlevel-init-nodev \
		test-lowlevel-init-nomtd \
		test-lowlevel-init-errmtd \
		test-lowlevel-init-devinfo-err \
		test-lowlevel-init-checkblock-err

# TFM: this needs some rework: BUILD_CC is only available
# when building w/ openembedded, so this should default to
# CC if not set.
# 
# Also, the coverage files generated are not yet post-processed.
#
test-%: tests/test-%.c
		${BUILD_CC} -Itests/include --coverage -ftest-coverage -o ${@} ${<} -lpthread

