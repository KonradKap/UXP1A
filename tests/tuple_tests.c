#include "tuple_tests.h"

#include "tuple/tuple.h"
#include "tuple/tuple_element.h"

START_TEST(tuple_make_test_1) {
    tuple *got = tuple_make("i", 1);
    ck_assert_uint_eq(got->nelements, 1u);
    ck_assert_uint_eq(got->elements[0].type, INT_TYPE);
    ck_assert_int_eq(got->elements[0].data.i, 1);
    tuple_free(got);
}
END_TEST

START_TEST(tuple_make_test_2) {
    tuple *got = tuple_make("i i s", 9, 11, "Kappa");
    ck_assert_uint_eq(got->nelements, 3u);
    ck_assert_uint_eq(got->elements[0].type, INT_TYPE);
    ck_assert_int_eq(got->elements[0].data.i, 9);
    ck_assert_uint_eq(got->elements[1].type, INT_TYPE);
    ck_assert_int_eq(got->elements[1].data.i, 11);
    ck_assert_uint_eq(got->elements[2].type, STRING_TYPE);
    ck_assert_str_eq(got->elements[2].data.s.string, "Kappa");
    tuple_free(got);
}
END_TEST

Suite *tuple_suite() {
    Suite *suite = suite_create("tuple tests");

    TCase *test_case = tcase_create("tuple_make_test_1");
    tcase_add_test(test_case, tuple_make_test_1);
    suite_add_tcase(suite, test_case);

    test_case = tcase_create("tuple_make_test_2");
    tcase_add_test(test_case, tuple_make_test_2);
    suite_add_tcase(suite, test_case);

    return suite;
}
