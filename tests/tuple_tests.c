#include "tuple_tests.h"

#include "tuple/tuple.h"
#include "tuple/tuple_element.h"
#include "utility.h"
#include "error.h"
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
    ck_assert_str_eq(got->elements[2].data.s, "Kappa");
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
    char *got_s;
    int result = 0;
    result |= tuple_get_int(got, 0, &got_i);
    result |= tuple_get_string(got, 1, &got_s);
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

START_TEST(tuple_buffer_test1) {
    tuple *base = tuple_make("i", 12);
    char buffer[256];
    int written = tuple_to_buffer(base, buffer, 256);
    ck_assert_int_eq(base->nelements, written);
    tuple *got = tuple_from_buffer(buffer);
    ck_assert_uint_eq(got->nelements, base->nelements);
    ck_assert_int_eq(got->elements[0].data.i, 12);
    tuple_free(base);
}
END_TEST

START_TEST(tuple_buffer_test2) {
    tuple *base = tuple_make("sis", "jp2gmd", 1, "UXP1A");
    char buffer[256];
    int written = tuple_to_buffer(base, buffer, 256);
    ck_assert_int_eq(base->nelements, written);
    tuple *got = tuple_from_buffer(buffer);
    ck_assert_uint_eq(got->nelements, base->nelements);
    ck_assert_str_eq(got->elements[0].data.s, "jp2gmd");
    ck_assert_int_eq(got->elements[1].data.i, 1);
    ck_assert_str_eq(got->elements[2].data.s, "UXP1A");
    tuple_free(base);
    tuple_free(got);
}
END_TEST

START_TEST(tuple_buffer_test3) {
    tuple *base = tuple_make("s", "A very long string that will surely exceed buffer's maximum size");
    char buffer[16];
    int result = tuple_to_buffer(base, buffer, 16);
    ck_assert_int_eq(result, 0);
    tuple_free(base);
}
END_TEST

START_TEST(tuple_buffer_test4) {
    tuple *base = tuple_make("is", -1, "UXP1A");
    char buffer[256];
    int written = tuple_to_buffer(base, buffer, 256);
    ck_assert_int_eq(base->nelements, written);
    tuple *got = tuple_from_buffer(buffer);
    ck_assert_uint_eq(got->nelements, base->nelements);
    ck_assert_int_eq(got->elements[0].data.i, -1);
    ck_assert_str_eq(got->elements[1].data.s, "UXP1A");
    tuple_free(base);
    tuple_free(got);
}
END_TEST

START_TEST(tuple_buffer_test5) {
    tuple *base = tuple_make("s", "x");
    char buffer[8];
    int result = tuple_to_buffer(base, buffer, 7);
    ck_assert_int_eq(0, result);
    result = tuple_to_buffer(base, buffer, 8);
    ck_assert_int_eq(1, result);
    tuple *got = tuple_from_buffer(buffer);
    ck_assert_uint_eq(got->nelements, base->nelements);
    ck_assert_str_eq(got->elements[0].data.s, "x");
    tuple_free(base);
    tuple_free(got);
}
END_TEST

START_TEST(tuple_buffer_test6) {
    tuple *base = tuple_make("si", "xa", -1);
    char buffer[14];
    int result = tuple_to_buffer(base, buffer, 15);
    ck_assert_int_eq(2, result);
    tuple *got = tuple_from_buffer(buffer);
    ck_assert_uint_eq(got->nelements, base->nelements);
    ck_assert_str_eq(got->elements[0].data.s, "xa");
    ck_assert_int_eq(got->elements[1].data.i, -1);
    tuple_free(base);
    tuple_free(got);
}
END_TEST

START_TEST(tuple_buffer_test7) {
    tuple *base = tuple_make("si", "xaaa", -1);
    char buffer[14];
    int result = tuple_to_buffer(base, buffer, 15);
    ck_assert_int_eq(1, result);
    tuple *got = tuple_from_buffer(buffer);
    ck_assert_uint_eq(got->nelements, 1);
    ck_assert_str_eq(got->elements[0].data.s, "xaaa");
    tuple_free(base);
    tuple_free(got);
}
END_TEST

START_TEST(tuple_set_test1) {
    tuple *base = tuple_make("i", 4);
    tuple_set_int(base, 0, 6);
    ck_assert_uint_eq(tuple_typeof(base, 0), INT_TYPE);
    ck_assert_int_eq(base->elements[0].data.i, 6);
    tuple_free(base);
}
END_TEST

START_TEST(tuple_set_test2) {
    tuple *base = tuple_make("f", 4.3);
    tuple_set_int(base, 0, 4);
    ck_assert_uint_eq(tuple_typeof(base, 0), INT_TYPE);
    ck_assert_int_eq(base->elements[0].data.i, 4);
    tuple_free(base);
}
END_TEST

START_TEST(tuple_set_test3) {
    tuple *base = tuple_make("is", 12, "Kappa");
    tuple_set_string(base, 1, "asdfghjqwe");
    ck_assert_uint_eq(tuple_typeof(base, 1), STRING_TYPE);
    ck_assert_str_eq(base->elements[1].data.s, "asdfghjqwe");
    ck_assert_uint_eq(tuple_typeof(base, 0), INT_TYPE);
    ck_assert_int_eq(base->elements[0].data.i, 12);
    tuple_free(base);
}
END_TEST

START_TEST(tuple_set_test4) {
    tuple *base = tuple_make("is", 12, "Kappa");
    tuple_set_string(base, 0, "asdfghjqwe");
    ck_assert_uint_eq(tuple_typeof(base, 1), STRING_TYPE);
    ck_assert_str_eq(base->elements[1].data.s, "Kappa");
    ck_assert_uint_eq(tuple_typeof(base, 0), STRING_TYPE);
    ck_assert_str_eq(base->elements[0].data.s, "asdfghjqwe");
    tuple_free(base);
}
END_TEST

START_TEST(tuple_set_op_test1) {
    tuple *base = tuple_make("is", 12, "Kappa");
    tuple_set_string_op(base, 0, "ASDF", OP_ANY);
    ck_assert_int_eq(tuple_typeof(base, 0), STRING_TYPE);
    ck_assert_int_eq(tuple_operator(base, 0), OP_ANY);
    ck_assert_str_eq(base->elements[0].data.s, "");
    tuple_free(base);
}
END_TEST

START_TEST(tuple_compare_test1) {
    tuple *base = tuple_make("i", 12);
    tuple *blueprint = tuple_make_nelements(1);
    tuple_set_int_op(blueprint, 0, 6, OP_GT);
    ck_assert(tuple_compare_to(base, blueprint));
    tuple_free(base);
    tuple_free(blueprint);
}
END_TEST

START_TEST(tuple_compare_test2) {
    tuple *base = tuple_make("i", 12);
    tuple *blueprint = tuple_make_nelements(1);
    tuple_set_int_op(blueprint, 0, 12, OP_LE);
    ck_assert(tuple_compare_to(base, blueprint));
    tuple_free(base);
    tuple_free(blueprint);
}
END_TEST

START_TEST(tuple_compare_test3) {
    tuple *base = tuple_make("i", 12);
    tuple *blueprint = tuple_make_nelements(1);
    tuple_set_int_op(blueprint, 0, 13, OP_LE);
    ck_assert(tuple_compare_to(base, blueprint));
    tuple_free(base);
    tuple_free(blueprint);
}
END_TEST

START_TEST(tuple_compare_test4) {
    tuple *base = tuple_make("iii", 12, 13, 14);
    tuple *blueprint = tuple_make_nelements(3);
    tuple_set_int_op(blueprint, 0, 13, OP_LT);
    tuple_set_int_op(blueprint, 1, 13, OP_EQ);
    tuple_set_int_op(blueprint, 2, 13, OP_GT);
    ck_assert(tuple_compare_to(base, blueprint));
    tuple_free(base);
    tuple_free(blueprint);
}
END_TEST

START_TEST(tuple_compare_test5) {
    tuple *base = tuple_make("isf", 12, "bbb", 3.4);
    tuple *blueprint = tuple_make_nelements(3);
    tuple_set_int_op(blueprint, 0, 13, OP_LT);
    tuple_set_string_op(blueprint, 1, "aaa", OP_ANY);
    tuple_set_float_op(blueprint, 2, 3.4, OP_EQ);
    ck_assert(tuple_compare_to(base, blueprint));
    tuple_free(base);
    tuple_free(blueprint);
}
END_TEST

START_TEST(tuple_compare_test6) {
    tuple *base = tuple_make("s", "bbb");
    tuple *blueprint = tuple_make_nelements(1);
    tuple_set_float_op(blueprint, 0, 3.4, OP_ANY);
    ck_assert(!tuple_compare_to(base, blueprint));
    tuple_free(base);
    tuple_free(blueprint);
}
END_TEST

START_TEST(tuple_compare_test7) {
    tuple *base = tuple_make("s", "bbb");
    tuple *blueprint = tuple_make_nelements(2);
    tuple_set_string_op(blueprint, 0, "", OP_ANY);
    tuple_set_string_op(blueprint, 1, "", OP_ANY);
    ck_assert(!tuple_compare_to(base, blueprint));
    tuple_free(base);
    tuple_free(blueprint);
}
END_TEST

START_TEST(tuple_compare_test8) {
    tuple *base = tuple_make("s", "bbb");
    tuple *blueprint = tuple_make_nelements(1);
    tuple_set_string_op(blueprint, 0, "aaa", OP_GT);
    ck_assert(tuple_compare_to(base, blueprint));
    tuple_free(base);
    tuple_free(blueprint);
}
END_TEST

START_TEST(tuple_compare_test9) {
    tuple *base = tuple_make("s", "bbb");
    tuple *blueprint = tuple_make_nelements(1);
    tuple_set_string_op(blueprint, 0, "bbb", OP_EQ);
    ck_assert(tuple_compare_to(base, blueprint));
    tuple_free(base);
    tuple_free(blueprint);
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

    tests_execute(suite, test_case, tuple_buffer_test1);
    tests_execute(suite, test_case, tuple_buffer_test2);
    tests_execute(suite, test_case, tuple_buffer_test3);
    tests_execute(suite, test_case, tuple_buffer_test4);
    tests_execute(suite, test_case, tuple_buffer_test5);
    tests_execute(suite, test_case, tuple_buffer_test6);
    tests_execute(suite, test_case, tuple_buffer_test7);

    tests_execute(suite, test_case, tuple_set_test1);
    tests_execute(suite, test_case, tuple_set_test2);
    tests_execute(suite, test_case, tuple_set_test3);
    tests_execute(suite, test_case, tuple_set_test4);

    tests_execute(suite, test_case, tuple_set_op_test1);

    tests_execute(suite, test_case, tuple_compare_test1);
    tests_execute(suite, test_case, tuple_compare_test2);
    tests_execute(suite, test_case, tuple_compare_test3);
    tests_execute(suite, test_case, tuple_compare_test4);
    tests_execute(suite, test_case, tuple_compare_test5);
    tests_execute(suite, test_case, tuple_compare_test6);
    tests_execute(suite, test_case, tuple_compare_test7);
    tests_execute(suite, test_case, tuple_compare_test8);
    tests_execute(suite, test_case, tuple_compare_test9);

    return suite;
}
