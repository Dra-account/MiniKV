#include <check.h>
#include "../include/storage.h"

START_TEST(test_set_and_get) {
    Storage* s = storage_create();
    ck_assert_int_eq(storage_set(s, "hello", "world"), 0);
    ck_assert_str_eq(storage_get(s, "hello"), "world");
    storage_free(s);
}
END_TEST

Suite* storage_suite(void) {
    Suite* s;
    TCase* tc_core;

    s = suite_create("Storage");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_set_and_get);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) {
    int failed = 0;
    Suite* s = storage_suite();
    SRunner* runner = srunner_create(s);
    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (failed == 0) ? 0 : 1;
}
