#include "unity.h"
#include "../include/packet_capture.h"
#include "../include/error_handler.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

void test_get_default_device(void) {
    char *device = get_default_device();
    TEST_ASSERT_NOT_NULL(device);
    TEST_ASSERT_TRUE(strlen(device) > 0);
    free(device);
}

void test_initialize_capture_invalid_device(void) {
    pcap_t *handle = initialize_capture("invalid_device", NULL);
    TEST_ASSERT_NULL(handle);
}

void test_list_devices_no_crash(void) {
    // Just call to ensure no crash
    list_devices();
    TEST_PASS();
}

void test_error_handler_functions(void) {
    // Test print_info (void, but ensure no crash)
    print_info("Test message");
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_get_default_device);
    RUN_TEST(test_initialize_capture_invalid_device);
    RUN_TEST(test_list_devices_no_crash);
    RUN_TEST(test_error_handler_functions);
    return UNITY_END();
}