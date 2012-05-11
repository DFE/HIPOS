#
# Simple makefile to run HidaV unittests
# 


TESTS = test-blockrom-init-exit		\
	test-blockrom-add		\
	test-blockrom-add-threaded	\
	test-blockrom-add-kzalloc-fail	\
	test-blockrom-readwritesect	\
	test-blockrom-remove


TEST_SRC = ${TESTS:%=%.c}

run-tests: ${TESTS}
		@for t in ${^}; do 				\
			./$$t >$$t.log 2>&1 && echo "OK  : $$t" || 	\
				(echo "FAIL: $$t"; cat $$t.log; false; break)	\
		done
		@gcov -fb ${TEST_SRC}  1>coverage.log 2>&1 
		@grep "File 'blockrom.c'" coverage.log -A 4

.PHONY: clean run-tests

clean:
		rm -f ${TESTS} *.gcda *.gcno *.log *.gcov

# TODO: This uses OpenEmbedded / bitbake build vars.
# We need to fall back a native CC (alongside with corresponding ***FLAGS) 
# if BUILD_* vars are not set.
test-%: test-%.c
		${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS} -Iinclude --coverage -ftest-coverage -o ${@} ${<}

