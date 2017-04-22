#ifndef HELLO_WORLD_TESTS_H
#define HELLO_WORLD_TESTS_H

#include <check.h>

#include "hello_world.h"

START_TEST(hello_world_test) {
    ck_assert_str_eq("hello world", hello_world());
}
END_TEST

Suite *hello_world_suite() {
    Suite *suite = suite_create("hello world");
    TCase *test_case = tcase_create("hello_world_test");

    tcase_add_test(test_case, hello_world_test);
    suite_add_tcase(suite, test_case);

    return suite;
}

#endif
