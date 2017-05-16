#include <check.h>

#include "tuple_tests.h"

int main() {
    SRunner *test_runner = srunner_create(NULL);

    srunner_add_suite(test_runner, tuple_suite());

    srunner_run_all(test_runner, CK_NORMAL);
    int failed_tests_cases = srunner_ntests_failed(test_runner);
    srunner_free(test_runner);
    return failed_tests_cases != 0;
}
