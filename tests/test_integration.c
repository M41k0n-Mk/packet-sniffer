#include "unity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

void setUp(void) {
}

void tearDown(void) {
}

void test_help_output(void) {
    int status = system("./packet_sniffer -h > /tmp/help_output.txt 2>&1");
    TEST_ASSERT_EQUAL(0, WEXITSTATUS(status));

    FILE *fp = fopen("/tmp/help_output.txt", "r");
    TEST_ASSERT_NOT_NULL(fp);

    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[bytes_read] = '\0';
    fclose(fp);

    TEST_ASSERT_TRUE(strstr(buffer, "Usage") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "Packet Sniffer") != NULL);

    // Clean up
    remove("/tmp/help_output.txt");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_help_output);
    return UNITY_END();
}