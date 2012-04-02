bin_PROGRAMS=bootconfig
bootconfigdir=

bootconfig_SOURCES= \
	main.c 			\
	lowlevel.c 		\
	logging.c

bootconfig_HEADERS= \
	lowlevel.h		\
	logging.h

TESTS = test_lowlevel_init

# TFM: this needs some rework: BUILD_CC is only available
# when building w/ openembedded, so this should default to
# CC if not set.
# 
# Also, the coverage files generated are not yet post-processed.
#
test_lowlevel_init: tests/test-lowlevel-init.c
		${BUILD_CC} -Itests/include --coverage -o ${@} ${<}
		./${@}
		mv ${@} ${@}.bin

