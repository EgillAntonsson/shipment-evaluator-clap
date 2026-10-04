#include <check.h>
#include "../src/DSPProcessor.cpp"  // Include implementation for testing

START_TEST(test_gain_processing_zero_db)
{
    float result = DSPProcessor::process_gain(1.0f, 0.0f);
    ck_assert_float_eq(result, 1.0f);
}
END_TEST

START_TEST(test_gain_processing_positive_db)
{
    float result = DSPProcessor::process_gain(1.0f, 6.0f);
    // 6 dB should double the amplitude (approx 1.995)
    ck_assert_float_eq_tol(result, 1.995f, 0.01f);
}
END_TEST

START_TEST(test_gain_processing_negative_db)
{
    float result = DSPProcessor::process_gain(1.0f, -6.0f);
    // -6 dB should halve the amplitude (approx 0.501)  
    ck_assert_float_eq_tol(result, 0.501f, 0.01f);
}
END_TEST

START_TEST(test_higher_order_function)
{
    auto processor = DSPProcessor::make_gain_processor(12.0f);
    float result = processor(1.0f);
    // 12 dB should give approximately 3.981 times the input
    ck_assert_float_eq_tol(result, 3.981f, 0.01f);
}
END_TEST

Suite* dsp_suite(void) {
    Suite *s = suite_create("DSPProcessor");
    
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_gain_processing_zero_db);
    tcase_add_test(tc_core, test_gain_processing_positive_db);
    tcase_add_test(tc_core, test_gain_processing_negative_db);
    tcase_add_test(tc_core, test_higher_order_function);
    
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {
    Suite *s = dsp_suite();
    SRunner *runner = srunner_create(s);
    
    srunner_run_all(runner, CK_NORMAL);
    int number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    
    return (number_failed == 0) ? 0 : 1;
}