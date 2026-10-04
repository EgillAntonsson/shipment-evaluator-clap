#include <stdio.h>
#include <check.h>

int main(void) {
    Suite *s = suite_create("MyClapPlugin");
    SRunner *runner = srunner_create(s);
    
    // Run tests and report results
    srunner_run_all(runner, CK_NORMAL);
    int number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    
    return (number_failed == 0) ? 0 : 1;
}