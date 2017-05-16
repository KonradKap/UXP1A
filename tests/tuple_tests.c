#include "tuple_tests.h"

#include "tuple/tuple.h"
#include "tuple/tuple_element.h"
#include "utility.h"
#include "tests_utility.h"

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


START_TEST(tuple_make_test_3) {
    tuple *got = tuple_make("s", "A very long string that will surely exceed tuple's maximum size "
                                 "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                 "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                 "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                 "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ck_assert_ptr_eq(got, NULL);
}
END_TEST

START_TEST(tuple_typeof_test1) {
    tuple *got = tuple_make("i f", 666, 21.37);
    ck_assert_int_eq(tuple_typeof(got, 0), INT_TYPE);
    ck_assert_int_eq(tuple_typeof(got, 1), FLOAT_TYPE);
    tuple_free(got);
}
END_TEST

START_TEST(tuple_get_test1) {
    tuple *got = tuple_make("i s", 666, "Kappa");
    int got_i;
    char got_s[16];
    int result = 0;
    result |= tuple_get_int(got, 0, &got_i);
    result |= tuple_get_string(got, 1, got_s);
    ck_assert_int_eq(got_i, 666);
    ck_assert_str_eq(got_s, "Kappa");
    ck_assert_int_eq(result, 0);
    tuple_free(got);
}
END_TEST

START_TEST(tuple_get_test2) {
    tuple *got = tuple_make("i s", 666, "Kappa");
    int got_i = 123;
    int result = 0;
    result = tuple_get_int(got, 1, &got_i);
    ck_assert_int_eq(got_i, 123);
    ck_assert_int_eq(result, TUPLE_E_INVALID_TYPE);
    result = tuple_get_int(got, 4, &got_i);
    ck_assert_int_eq(got_i, 123);
    ck_assert_int_eq(result, TUPLE_E_OUT_OF_RANGE);
    tuple_free(got);
}
END_TEST

Suite *tuple_suite() {
    Suite *suite = suite_create("tuple tests");
    TCase *test_case;

    tests_execute(suite, test_case, tuple_make_test_1);
    tests_execute(suite, test_case, tuple_make_test_2);

    IGNORED(tuple_make_test_3);

    tests_execute(suite, test_case, tuple_typeof_test1);
    tests_execute(suite, test_case, tuple_get_test1);
    tests_execute(suite, test_case, tuple_get_test2);

    return suite;
}
