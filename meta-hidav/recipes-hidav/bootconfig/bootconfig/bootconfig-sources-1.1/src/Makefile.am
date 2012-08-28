bin_PROGRAMS=bootconfig
bootconfigdir=

bootconfig_SOURCES= \
	main.c 			\
	lowlevel.c 		\
	logging.c

bootconfig_HEADERS= \
	lowlevel.h		\
	logging.h

TEST_BIN =  test-lowlevel-init \
			test-lowlevel-init-nodev \
			test-lowlevel-init-nomtd \
			test-lowlevel-init-errmtd \
			test-lowlevel-init-devinfo-err \
			test-lowlevel-init-checkblock-err \
			test-lowlevel-reread \
			test-lowlevel-reread-noinit \
			test-lowlevel-reread-badnand \
			test-lowlevel-reread-eccerrnand \
			test-lowlevel-getcurrent \
			test-lowlevel-getcurrent-noinit \
			test-lowlevel-setpartition \
			test-lowlevel-setpartition-noblock \
			test-lowlevel-setpartition-allempty \
			test-lowlevel-setpartition-noinit	\
			test-lowlevel-setpartition-illegalpart \
			test-lowlevel-sethealthy \
			test-lowlevel-alloc-bootblock-fail	\
			test-lowlevel-write-block-alloc-fail	\
			test-lowlevel-double-epoch

TEST_SRC = ${TEST_BIN:%=tests/%.c}

# TFM: this needs some rework: BUILD_CC is only available
# when building w/ openembedded, so this should default to
# CC if not set.
# 
# Also, the coverage files generated are not yet post-processed.
#
check: ${TEST_SRC}
		for bin in ${TEST_BIN}; do \
			${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS} -Itests/include --coverage -ftest-coverage -o $$bin tests/$$bin.c && \
			./$$bin > $$bin.log; \
		done

