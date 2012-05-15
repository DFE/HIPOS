/**
 * Test Harness : KISS unit testing "framework".
 *
 * Copyright (C) 2011, 2012 DResearch Fahrzeugelektronik GmbH   
 * Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
 *                                                               
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License  
 * as published by the Free Software Foundation; either version 
 * 2 of the License, or (at your option) any later version.     
 *
 */

#ifndef _TEST_HARNESS_H
#define _TEST_HARNESS_H

#include <stdlib.h>
#include <stdio.h>

/*
 * * Unit Test example
 * -----------------------
 *  Let's assume we mocked the following one-argument function returning an Int:
 *
      int coredump(void * snuff);
 *
 *  by providing the following line in a Mock file:
 *
       MOCK_1(int, coredump, void*)
 *
 *  then the macro will produce a function body and some globals:
 *
        long    _coredump_called_count = -1;
        int     _coredump_ret [MOCK_MAX_NUM_FUNC_CALL];
        void    (*_coredump_cb) (void *) = 0;
        void    *_coredump_exp_arg0[MOCK_MAX_NUM_FUNC_CALL];

        int coredump( void * arg0 ) { ... }
            
 *  These data structures are filled by the Unit Test using the following wrapper defines.
 *  This data controls how a mocked function will behave when it is called by the
 *  harnessed code - in the first call, the second call in succession, and so on.
 *  Let's assume the harnessed code is supposed to call coredump() twice. Then type:

        MOCK_1_CALL( 12, coredump, 0xff);
        MOCK_1_CALL( 19, coredump, 0xbaba);

 *  The harnessed code's function can then be called by the unit test. 
 *  The first time the harnessed code calls coredump() it is supposed to do it like this:

        coredump(0xff);

 *  - i.e. with 0xff being the parameter value - or the test will fail. 
 *  The mocked coredump() will return 12 to the harnessed code. 
 *  The second time the harnessed code calls coredump() it is supposed to do so:

        coredump(0xbaba);

 *  or the test will fail. coredump() will return 19 to the harnessed code. 
 *
 *  IMPORTANT NOTE : If the unit test needs to run the harnessed function AGAIN,
 *   then it MUST re-initialize the mock framework by calling MOCK_RESET on every
 *   mocked function:

        MOCK_RESET ( coredump );

 *  This needs to be done for every mocked function called by the harnessed code
 *  if multiple tests are to be run in a single unit test application.
 
 */

#define ERR_PREPARATION_FAILED  99
#define ERR_TEST_FAILED 100

/**
 * Assert macro for easy test condition checks.
 * This macro will check for exp == got, and print a descriptive error
 *  message if the check fails.
 * \param exp expected value.
 * \param got value received. May be a function call which will be evaluated
 *          by the macro.
 * \param type type of the exp / got values.
 */
#define TEST_ASSERT( exp, got, type )                       \
{                                                       \
    void *e=NULL, *g=NULL;                              \
    type _got = ( got );                                \
    type _exp = ( exp );                                \
    memcpy(&e, &_exp, sizeof( type ));                   \
    memcpy(&g, &_got, sizeof( type ));                   \
    if ( (_exp) != (_got)  ){                             \
        printf("%s:%s, line %u: " #got " should be %p (%s), is %p\n", \
         __FILE__, __func__, __LINE__, e, #exp, g);  \
        exit (ERR_TEST_FAILED);                         \
    }                                                   \
}
/**
 * Convenience assert macro for checking whether a function returns non-NULL.
 * This macro will check for NULL != got, and print a descriptive error
 *  message if the check fails.
 * \param ret_val Variable to store return value of ( got ) in, 
 *   in case ( got ) is a function call.
 * \param got value received. May be a function call which will be evaluated
 *          by the macro.
 */
#define TEST_ASSERT_NOTNULL( ret_val, got )       \
{                                                       \
    if ( (NULL) == ( ret_val = (got) ) ){                    \
        printf("%s:%s, line %u returned NULL. TEST FAIL. \n", \
         __FILE__, __func__, __LINE__);  \
        exit (ERR_TEST_FAILED);                         \
    }                                                   \
}

/**
 * Convenience assert macro for checking whether a mocked function was called N times.
 * \param func name of the mocked function.
 * \param count expected call count of the function.
 */
#define TEST_ASSERT_CALLED( func, count )                                       \
{                                                                               \
    if ( _##func##_called_count != count -1 ) {                                 \
        printf("%s:%s, line %u function " #func                                \
                " called %d times, expected %ld times. TEST_FAIL.\n",            \
         __FILE__, __func__, __LINE__, count, _##func##_called_count + 1);      \
    }                                                                           \
}

#define TEST_FAIL( msg )                                \
{                                                       \
        printf("%s:%s, line %u: %s. TEST FAIL.\n",      \
         __FILE__, __func__, __LINE__, msg);            \
        exit (ERR_TEST_FAILED);                         \
}

/**
 * Fail condition callback to be implemented by an unit test.
 * This callback will be run by the test bed mock functions when an error occurs
 * (e.g. precondition not met).
 * \ref _default_fail and \ref _default_pass provide default implementations
 *  which can be switched by calling \ref set_default_fail.
 * \param function description of the test bed function the error occured in.
 * \param reason_string description of what went wrong.
 * \param exp Expected value (e.g. when a parameter check failed)
 * \param got Value received (instead of the expected value)
 * \return != 0 if the error is fatal (and the test run should be aborted),
 *   0 otherwise. 
 */
typedef int (*fail_condition_cb_t)(char * function, char * reason_string, 
                void * exp, void * got);

/**
 * Default fail callback function providing an error message upon parameter 
 * check fail, then return 23 (causing the test to abort). 
 * Parameter description see \ref fail_condition_cb_t.
 */
static int _default_fail ( char * where, char * what, void * exp, void * got) {
    printf("FAIL in %s: %s Exp:%p, got: %p. ABORTING TEST.\n", 
        where, what, exp, got);
    return 23;
}

/**
 * Default pass callback function providing an error message upon parameter 
 * check fail, then return 0 (causing the test to continue). 
 * Parameter description see \ref fail_condition_cb_t.
 */
static int _default_pass ( char * where, char * what, void * exp, void * got) {
    printf("FAIL in %s: %s Exp:%p, got: %p. (ignored)\n",
        where, what, exp, got);
    return 0;
}

/**
 * Global fail callback store used by the test harness.
 * Can be set to a custom fail function imple
 */
fail_condition_cb_t fail_callback = _default_fail;

/**
 * Set fail behaviour of default implementations (i.e. when
 * the unit test does not provide a custom fail_callback).
 * \param mode To fail or not to fail, that is the question. 
                    Whether 'tis nobler in the mind to suffer...
 */
enum fail_switch_t { fail, pass };
void set_default_fail( enum fail_switch_t mode ) {
    switch (mode) {
        case fail: fail_callback = _default_fail; break;
        case pass: fail_callback = _default_pass; break;
        default  : fail_callback = _default_fail; break;
}}

/**
 * Parameter check macro. This macro will check a function parameter
 * against its supposed value (see MOCK_x definitions), calling the
 * \ref fail_callback upon check failure, and exit()ing with
 *  the fail callback's return value.
 * \param func name of the function whose parameter is to be checked
 * \param param name of the parameter, e.g. arg3.
 */

static void * DONT_CHECK_PARAM = "Hey, don't check me. It's OK, really.";
#include <string.h>

#ifndef MOCK_MAX_NUM_FUNC_CALL   
#define MOCK_MAX_NUM_FUNC_CALL   50
#endif

#define CHECK_PARAM_P(func, param, type)                                        \
{                                                                               \
    int ret;                                                                    \
    void *exp=NULL, *got=NULL;                                                  \
    memcpy(&exp, &_##func##_exp_##param[_##func##_called_count], sizeof(type)); \
    memcpy(&got, &param, sizeof(type));                                         \
    if (_##func##_called_count + 1 >= MOCK_MAX_NUM_FUNC_CALL) {                      \
        printf("INTERNAL TEST ERROR: Maximum number of mock function calls "    \
                 "(%u) reached.\n", MOCK_MAX_NUM_FUNC_CALL);                         \
        exit(ERR_PREPARATION_FAILED);                                           \
    }                                                                           \
    if (_##func##_called_count > _##func##_configured_calls) {                  \
        printf("TEST ERROR: Mocked function " #func " called more often (%ld) than it has been set up to by the test (%ld)!\n",\
                _##func##_called_count, _##func##_configured_calls);        \
        exit(ERR_PREPARATION_FAILED);                                       \
    }                                                                       \
    if (    (exp != DONT_CHECK_PARAM)                                       \
        &&  (exp != got)                                                    \
        &&  (ret = fail_callback(#func,                                     \
                        "Parameter " #param " failed expectations.",        \
                                exp, got),                                  \
                   printf (" --- >>> mock wrapper for " #func ": at call #%ld (%ld calls configured totally):\n", \
                          _##func##_called_count + 1, _##func##_configured_calls + 1))    \
            )                  \
        exit(ret);                                                          \
}

/**
 * The Mock definitions
 * --------------------
 * These preprocessor macros provide easy and automated function mocking.
 * Mocked functions are the "back end" of this test harness. They need to
 * be set up properly by the unit test. They are called by the harnessed code
 * (i.e. the business logic), emulating all the library functions the
 * harnessed code uses.
 *
 * Assuming a C header file function declaration in a "real" header like e.g.

       int coredump(void * snuff);

 * this function can easily be auto-mocked in the corresponding mock header:

       MOCK_1(int, coredump, void*)
 
 * The macro naming depends on the number of parameters the function to be 
 * mocked would take (MOCK_x) as well as the function's return type 
 * (either "void" or anything else: MOCK_xV vs. MOCK_x). To e.g. mock a 
 * function taking 3 arguments and returning void, one would use the 
 * \ref MOCK_3V macro.
 *
 * Mock macros follow this parameter scheme:
 *  MOCK_x(return_type, function_name, arg0, ..., argn)
 * Macros mocking void functions will have no return_type parameter.
 *
 * The macros will perform these actions:
 * - create a global counter variable _[function]_called_count.
 *    This variable is initialized to -1 and increased each time
 *    the harnessed code calls the mocked function.
 *    NOTE: This variable MUST be re-set by the unit test code
 *    when multiple tests are run (using MOCK_RESET( <func> )!
 * - create global arrays named _[function]_exp_arg<n>[MOCK_MAX_NUM_FUNC_CALL]
 *    depending on the number of arguments the function takes.
 *    Unit tests should set these to the expected argument values
 *    the harnessed code is supposed to call the mocked function with
 *    before calling the function(s) to be tested.
 *    If the argument should not be checked by the harness the unit test
 *    should set it to \ref DONT_CHECK_PARAM.
 * - the non-void macros will also creata a global return 
 *    array _[function]_ret[MOCK_MAX_NUM_FUNC_CALL]. Unit tests should store the 
 *    values the mocked function is supposed to return in successive calls
 *    to the harnessed code.
 * - create a function body with the return type and the argument types
 *    provided in the macro parameter. The argument naming will be
 *    arg0, arg1, and so on.
 *    The generated function will check all its arguments using 
 *    \ref CHECK_PARAM_P against the expected values stored in 
 *    _[function]_exp_arg[n]. It will then return the value stored in
 *    _[function]_ret. 
 * - create a global function pointer variable _[function]_cb, which
 *    will be initialized to 0. Unit tests may set this callback to their
 *    own function body implementation by using MOCK_CB_SET( func, cb ). If set, 
 *    the callback will be run by the harness after checking the input 
 *    arguments and before returning the result.
 *    NOTE: the callback's return value WILL NOT BE RETURNED by the harness.
 *    _[function]_ret will always be returned. So if the unit test callback
 *    is supposed to return anything useful to the harnessed business logic,
 *    it must set _[function]_ret appropriately.
 *
 *
*/

/**
 * Convenience macro to set a custom callback for a mocked function.
 * \param func name of the mocked function.
 * \param cb callback to be installed
 */
#define MOCK_CB_SET( func, cb )                 \
{                                               \
    _##func##_cb = cb;                          \
}

/**
 * Convenience macro to remove a custom callback from a mocked function.
 * \param func name of the mocked function.
 */
#define MOCK_CB_CLEAR( func )                   \
{                                               \
    _##func##_cb = 0;                           \
}


#define MOCK_MAX_NUM_FUNC_CALL   50

#define _MOCK_COMMON_V( func )                 \
long _##func##_configured_calls = -1; \
long _##func##_called_count = -1;

#define _MOCK_COMMON( ret_type, func )        \
_MOCK_COMMON_V(func)                          \
ret_type    _##func##_ret[MOCK_MAX_NUM_FUNC_CALL];


#define MOCK_RESET( func )              \
{                                       \
    _##func##_configured_calls = -1;    \
    _##func##_called_count= -1;         \
    _##func##_cb = 0;                   \
}

/**
 * Mock definitions for 0 argument functions
 */
#define MOCK_0(ret_type, func)       \
_MOCK_COMMON( ret_type, func )       \
void    (*_##func##_cb)() = 0;       \
ret_type func(void) {                \
    _##func##_called_count++;        \
    if (_##func##_cb)                \
        _##func##_cb();              \
    return _##func##_ret[_##func##_called_count]; \
}

#define MOCK_0V(func)                \
_MOCK_COMMON_V(func)                 \
void    (*_##func##_cb)() = 0;       \
void func(void) {                    \
    _##func##_called_count++;        \
    if (_##func##_cb)                \
        _##func##_cb();              \
}

#define MOCK_0V_CALL( func )        \
{                                   \
    _##func##_configured_calls ++; \
}

#define MOCK_0_CALL( ret, func )    \
{                                   \
    MOCK_0V_CALL( func )            \
    _##func##_ret[ _##func##_configured_calls] = ret; \
}


/**
 * Mock definitions for 1 argument functions
 */
#define MOCK_1(ret_type, func, arg0_type)       \
_MOCK_COMMON( ret_type, func )                  \
void (*_##func##_cb)(arg0_type) = 0;            \
arg0_type   _##func##_exp_arg0[MOCK_MAX_NUM_FUNC_CALL];\
ret_type func(arg0_type arg0) {                 \
    _##func##_called_count++;                   \
    CHECK_PARAM_P( func, arg0, arg0_type);      \
    if (_##func##_cb)                           \
        _##func##_cb(arg0);                     \
    return _##func##_ret[_##func##_called_count]; \
}

#define MOCK_1V(func, arg0_type)                \
_MOCK_COMMON_V(func)                            \
void (*_##func##_cb)(arg0_type) = 0;            \
arg0_type   _##func##_exp_arg0[MOCK_MAX_NUM_FUNC_CALL]; \
void    func(arg0_type arg0) {                  \
    _##func##_called_count++;                   \
    CHECK_PARAM_P( func, arg0, arg0_type);      \
    if (_##func##_cb)                           \
        _##func##_cb(arg0);                     \
}

#define MOCK_1V_CALL( func, arg0 )  \
{                                   \
    _##func##_configured_calls ++; \
    _##func##_exp_arg0[ _##func##_configured_calls ] = arg0;     \
}

#define MOCK_1_CALL( ret, func, arg0 )    \
{                                         \
    MOCK_1V_CALL( func, arg0 )            \
    _##func##_ret[ _##func##_configured_calls ] = ret; \
}


/**
 * Mock definitions for 2 argument functions
 */
#define MOCK_2(ret_type, func, arg0_type, arg1_type)     \
_MOCK_COMMON( ret_type, func )                           \
void (*_##func##_cb)(arg0_type, arg1_type) = 0;          \
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
ret_type func(arg0_type arg0, arg1_type arg1) {          \
    _##func##_called_count++;                            \
    CHECK_PARAM_P( func, arg0, arg0_type);               \
    CHECK_PARAM_P( func, arg1, arg1_type);               \
    if (_##func##_cb)                                    \
        _##func##_cb(arg0, arg1);                        \
    return _##func##_ret[_##func##_called_count];        \
}

#define MOCK_2V(func, arg0_type, arg1_type)             \
_MOCK_COMMON_V(func)                                    \
void (*_##func##_cb)(arg0_type, arg1_type) = 0;         \
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
void func(arg0_type arg0, arg1_type arg1) {             \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1);                       \
}

#define MOCK_2V_CALL( func, arg0, arg1 )  \
{                                   \
    _##func##_configured_calls ++; \
    _##func##_exp_arg0[ _##func##_configured_calls ] = arg0;     \
    _##func##_exp_arg1[ _##func##_configured_calls ] = arg1;     \
}

#define MOCK_2_CALL( ret, func, arg0, arg1 )    \
{                                               \
    MOCK_2V_CALL( func, arg0, arg1 )            \
    _##func##_ret[ _##func##_configured_calls] = ret; \
}
/**
 * Mock definitions for 3 argument functions
 */
#define MOCK_3(ret_type, func, arg0_type, arg1_type, arg2_type) \
_MOCK_COMMON( ret_type, func )                                  \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type) = 0;      \
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
ret_type func(arg0_type arg0, arg1_type arg1, arg2_type arg2) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2);                 \
    return _##func##_ret[_##func##_called_count];       \
}

#define MOCK_3V(func, arg0_type, arg1_type, arg2_type) \
_MOCK_COMMON_V(func)                                   \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type) = 0;  \
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
void func(arg0_type arg0, arg1_type arg1, arg2_type arg2) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2);                 \
}

#define MOCK_3V_CALL( func, arg0, arg1, arg2 )  \
{                                   \
    _##func##_configured_calls ++; \
    _##func##_exp_arg0[ _##func##_configured_calls ] = arg0;     \
    _##func##_exp_arg1[ _##func##_configured_calls ] = arg1;     \
    _##func##_exp_arg2[ _##func##_configured_calls ] = arg2;     \
}

#define MOCK_3_CALL( ret, func, arg0, arg1, arg2 )      \
{                                                       \
    MOCK_3V_CALL( func, arg0, arg1, arg2 )              \
    _##func##_ret[ _##func##_configured_calls] = ret; \
}


/**
 * Mock definitions for 4 argument functions
 */
#define MOCK_4(ret_type, func, arg0_type, arg1_type, arg2_type, arg3_type) \
_MOCK_COMMON( ret_type, func )                                             \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type, arg3_type) = 0;      \
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
arg3_type   _##func##_exp_##arg3[MOCK_MAX_NUM_FUNC_CALL];\
ret_type func(arg0_type arg0, arg1_type arg1, arg2_type arg2, arg3_type arg3) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    CHECK_PARAM_P( func, arg3, arg3_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2, arg3);           \
    return _##func##_ret[_##func##_called_count];       \
}

#define MOCK_4V(func, arg0_type, arg1_type, arg2_type, arg3_type)       \
_MOCK_COMMON_V(func)                                                    \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type, arg3_type) = 0;   \
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
arg3_type   _##func##_exp_##arg3[MOCK_MAX_NUM_FUNC_CALL];\
void func(arg0_type arg0, arg1_type arg1, arg2_type arg2, arg3_type arg3) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    CHECK_PARAM_P( func, arg3, arg3_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2, arg3);           \
}

#define MOCK_4V_CALL( func, arg0, arg1, arg2, arg3 )  \
{                                   \
    _##func##_configured_calls ++; \
    _##func##_exp_arg0[ _##func##_configured_calls ] = arg0;     \
    _##func##_exp_arg1[ _##func##_configured_calls ] = arg1;     \
    _##func##_exp_arg2[ _##func##_configured_calls ] = arg2;     \
    _##func##_exp_arg3[ _##func##_configured_calls ] = arg3;     \
}

#define MOCK_4_CALL( ret, func, arg0, arg1, arg2, arg3 )      \
{                                                             \
    MOCK_4V_CALL( func, arg0, arg1, arg2, arg3 )              \
    _##func##_ret[ _##func##_configured_calls] = ret;       \
}

/**
 * Mock definitions for 5 argument functions
 */
#define MOCK_5(ret_type, func, arg0_type, arg1_type, arg2_type, arg3_type, arg4_type) \
_MOCK_COMMON( ret_type, func )                                                        \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type) = 0;      \
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
arg3_type   _##func##_exp_##arg3[MOCK_MAX_NUM_FUNC_CALL];\
arg4_type   _##func##_exp_##arg4[MOCK_MAX_NUM_FUNC_CALL];\
ret_type func(arg0_type arg0, arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    CHECK_PARAM_P( func, arg3, arg3_type);              \
    CHECK_PARAM_P( func, arg4, arg4_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2, arg3, arg4);     \
    return _##func##_ret[_##func##_called_count];       \
}

#define MOCK_5V(func, arg0_type, arg1_type, arg2_type, arg3_type, arg4_type)    \
_MOCK_COMMON_V(func)                                                            \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type) = 0;\
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
arg3_type   _##func##_exp_##arg3[MOCK_MAX_NUM_FUNC_CALL];\
arg4_type   _##func##_exp_##arg4[MOCK_MAX_NUM_FUNC_CALL];\
void func(arg0_type arg0, arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    CHECK_PARAM_P( func, arg3, arg3_type);              \
    CHECK_PARAM_P( func, arg4, arg4_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2, arg3, arg4);     \
}


#define MOCK_5V_CALL( func, arg0, arg1, arg2, arg3, arg4 )  \
{                                   \
    _##func##_configured_calls ++; \
    _##func##_exp_arg0[ _##func##_configured_calls ] = arg0;     \
    _##func##_exp_arg1[ _##func##_configured_calls ] = arg1;     \
    _##func##_exp_arg2[ _##func##_configured_calls ] = arg2;     \
    _##func##_exp_arg3[ _##func##_configured_calls ] = arg3;     \
    _##func##_exp_arg4[ _##func##_configured_calls ] = arg4;     \
}

#define MOCK_5_CALL( ret, func, arg0, arg1, arg2, arg3, arg4 )      \
{                                                                   \
    MOCK_5V_CALL( func, arg0, arg1, arg2, arg3, arg4 )              \
    _##func##_ret[ _##func##_configured_calls] = ret;             \
}

/**
 * Mock definitions for 6 argument functions
 */
#define MOCK_6(ret_type, func, arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type) \
_MOCK_COMMON( ret_type, func )                                                                   \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type) = 0;      \
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
arg3_type   _##func##_exp_##arg3[MOCK_MAX_NUM_FUNC_CALL];\
arg4_type   _##func##_exp_##arg4[MOCK_MAX_NUM_FUNC_CALL];\
arg5_type   _##func##_exp_##arg5[MOCK_MAX_NUM_FUNC_CALL];\
ret_type func(arg0_type arg0, arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4, arg5_type arg5) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    CHECK_PARAM_P( func, arg3, arg3_type);              \
    CHECK_PARAM_P( func, arg4, arg4_type);              \
    CHECK_PARAM_P( func, arg5, arg5_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2, arg3, arg4, arg5);   \
    return _##func##_ret[_##func##_called_count];           \
}

#define MOCK_6V(func, arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type)    \
_MOCK_COMMON_V(func)                                                                       \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type) = 0;\
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
arg3_type   _##func##_exp_##arg3[MOCK_MAX_NUM_FUNC_CALL];\
arg4_type   _##func##_exp_##arg4[MOCK_MAX_NUM_FUNC_CALL];\
arg5_type   _##func##_exp_##arg5[MOCK_MAX_NUM_FUNC_CALL];\
void func(arg0_type arg0, arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4, arg5_type arg5) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    CHECK_PARAM_P( func, arg3, arg3_type);              \
    CHECK_PARAM_P( func, arg4, arg4_type);              \
    CHECK_PARAM_P( func, arg5, arg5_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2, arg3, arg4, arg5);     \
}


#define MOCK_6V_CALL( func, arg0, arg1, arg2, arg3, arg4, arg5 )  \
{                                   \
    _##func##_configured_calls ++; \
    _##func##_exp_arg0[ _##func##_configured_calls ] = arg0;     \
    _##func##_exp_arg1[ _##func##_configured_calls ] = arg1;     \
    _##func##_exp_arg2[ _##func##_configured_calls ] = arg2;     \
    _##func##_exp_arg3[ _##func##_configured_calls ] = arg3;     \
    _##func##_exp_arg4[ _##func##_configured_calls ] = arg4;     \
    _##func##_exp_arg5[ _##func##_configured_calls ] = arg5;     \
}

#define MOCK_6_CALL( ret, func, arg0, arg1, arg2, arg3, arg4, arg5 )    \
{                                                                       \
    MOCK_6V_CALL( func, arg0, arg1, arg2, arg3, arg4, arg5 )            \
    _##func##_ret[ _##func##_configured_calls] = ret;                   \
}






/**
 * Mock definitions for 10 argument functions
 */
#define MOCK_10(ret_type, func, arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type) \
_MOCK_COMMON( ret_type, func )                                                                   \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type) = 0;      \
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
arg3_type   _##func##_exp_##arg3[MOCK_MAX_NUM_FUNC_CALL];\
arg4_type   _##func##_exp_##arg4[MOCK_MAX_NUM_FUNC_CALL];\
arg5_type   _##func##_exp_##arg5[MOCK_MAX_NUM_FUNC_CALL];\
arg6_type   _##func##_exp_##arg6[MOCK_MAX_NUM_FUNC_CALL];\
arg7_type   _##func##_exp_##arg7[MOCK_MAX_NUM_FUNC_CALL];\
arg8_type   _##func##_exp_##arg8[MOCK_MAX_NUM_FUNC_CALL];\
arg9_type   _##func##_exp_##arg9[MOCK_MAX_NUM_FUNC_CALL];\
ret_type func(arg0_type arg0, arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4, arg5_type arg5, arg6_type arg6, arg7_type arg7, arg8_type arg8, arg9_type arg9) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    CHECK_PARAM_P( func, arg3, arg3_type);              \
    CHECK_PARAM_P( func, arg4, arg4_type);              \
    CHECK_PARAM_P( func, arg5, arg5_type);              \
    CHECK_PARAM_P( func, arg6, arg6_type);              \
    CHECK_PARAM_P( func, arg7, arg7_type);              \
    CHECK_PARAM_P( func, arg8, arg8_type);              \
    CHECK_PARAM_P( func, arg9, arg9_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);   \
    return _##func##_ret[_##func##_called_count];           \
}

#define MOCK_10V(func, arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type)    \
_MOCK_COMMON_V(func)                                                                       \
void (*_##func##_cb)(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type) = 0;\
arg0_type   _##func##_exp_##arg0[MOCK_MAX_NUM_FUNC_CALL];\
arg1_type   _##func##_exp_##arg1[MOCK_MAX_NUM_FUNC_CALL];\
arg2_type   _##func##_exp_##arg2[MOCK_MAX_NUM_FUNC_CALL];\
arg3_type   _##func##_exp_##arg3[MOCK_MAX_NUM_FUNC_CALL];\
arg4_type   _##func##_exp_##arg4[MOCK_MAX_NUM_FUNC_CALL];\
arg5_type   _##func##_exp_##arg5[MOCK_MAX_NUM_FUNC_CALL];\
arg6_type   _##func##_exp_##arg6[MOCK_MAX_NUM_FUNC_CALL];\
arg7_type   _##func##_exp_##arg7[MOCK_MAX_NUM_FUNC_CALL];\
arg8_type   _##func##_exp_##arg8[MOCK_MAX_NUM_FUNC_CALL];\
arg9_type   _##func##_exp_##arg9[MOCK_MAX_NUM_FUNC_CALL];\
void func(arg0_type arg0, arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4, arg5_type arg5, arg6_type arg6, arg7_type arg7, arg8_type arg8, arg9_type arg9) { \
    _##func##_called_count++;                           \
    CHECK_PARAM_P( func, arg0, arg0_type);              \
    CHECK_PARAM_P( func, arg1, arg1_type);              \
    CHECK_PARAM_P( func, arg2, arg2_type);              \
    CHECK_PARAM_P( func, arg3, arg3_type);              \
    CHECK_PARAM_P( func, arg4, arg4_type);              \
    CHECK_PARAM_P( func, arg5, arg5_type);              \
    CHECK_PARAM_P( func, arg6, arg6_type);              \
    CHECK_PARAM_P( func, arg7, arg7_type);              \
    CHECK_PARAM_P( func, arg8, arg8_type);              \
    CHECK_PARAM_P( func, arg9, arg9_type);              \
    if (_##func##_cb)                                   \
        _##func##_cb(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);   \
}


#define MOCK_10V_CALL( func, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 )  \
{                                   \
    _##func##_configured_calls ++; \
    _##func##_exp_arg0[ _##func##_configured_calls ] = arg0;     \
    _##func##_exp_arg1[ _##func##_configured_calls ] = arg1;     \
    _##func##_exp_arg2[ _##func##_configured_calls ] = arg2;     \
    _##func##_exp_arg3[ _##func##_configured_calls ] = arg3;     \
    _##func##_exp_arg4[ _##func##_configured_calls ] = arg4;     \
    _##func##_exp_arg5[ _##func##_configured_calls ] = arg5;     \
    _##func##_exp_arg6[ _##func##_configured_calls ] = arg6;     \
    _##func##_exp_arg7[ _##func##_configured_calls ] = arg7;     \
    _##func##_exp_arg8[ _##func##_configured_calls ] = arg8;     \
    _##func##_exp_arg9[ _##func##_configured_calls ] = arg9;     \
}

#define MOCK_10_CALL( ret, func, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 )    \
{                                                                       \
    MOCK_10V_CALL( func, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 );           \
    _##func##_ret[ _##func##_configured_calls] = ret;                   \
}




/*
 * 
 * Additional, "expert level" helpers 
 * 
 */

#define MOCK_RETVAL_OF( func )   _##func##_ret[ _##func##_called_count ]








#endif /* defined _TEST_HARNESS_H */
