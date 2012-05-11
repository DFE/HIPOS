#
# Simple makefile to run HidaV unittests
# 


TESTS = test-blockrom-init

TEST_SRC = ${TESTS:%=%.c}

run-tests: ${TESTS}
		for t in ${<}; do 		\
			./$$t > $$t.log 2>&1;	\
		done
		gcov -fb ${TEST_SRC} > coverage.log 2>&1

.PHONY: clean run-tests

clean:
		rm -f ${TESTS} *.gcda *.gcno *.log

# TODO: This uses OpenEmbedded / bitbake build vars.
# We need to fall back a native CC (alongside with corresponding ***FLAGS) 
# if BUILD_* vars are not set.
test-%: test-%.c
		${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS} -Iinclude --coverage -ftest-coverage -o ${@} ${<}

