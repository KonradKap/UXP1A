#ifndef TESTS_UTILITY_H
#define TESTS_UTILITY_H

#define tests_execute(test_suite, test_case, test_function) \
    do{\
        (test_case) = tcase_create("test_function");\
        tcase_add_test((test_case), (test_function));\
        suite_add_tcase((test_suite), (test_case));\
    } while(0)


#endif //TESTS_UTILITY_H
