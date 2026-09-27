#include <unity.h>
#include "logic.h"

void setUp(void) {}
void tearDown(void) {}

/* ---------- Temperature alarm (5 tests) ---------- */

void test_temperature_below_lower_limit_is_low(void)
{
    TEST_ASSERT_EQUAL(ALARM_LOW_TEMPERATURE, evaluateTemperature(17.9f));
}

void test_temperature_exactly_lower_limit_is_normal(void)
{
    TEST_ASSERT_EQUAL(ALARM_NORMAL, evaluateTemperature(18.0f));
}

void test_temperature_normal_value_is_normal(void)
{
    TEST_ASSERT_EQUAL(ALARM_NORMAL, evaluateTemperature(25.0f));
}

void test_temperature_exactly_upper_limit_is_normal(void)
{
    TEST_ASSERT_EQUAL(ALARM_NORMAL, evaluateTemperature(30.0f));
}

void test_temperature_above_upper_limit_is_high(void)
{
    TEST_ASSERT_EQUAL(ALARM_HIGH_TEMPERATURE, evaluateTemperature(30.1f));
}

/* ---------- Display navigation (4 tests) ---------- */

void test_next_from_temperature_is_humidity(void)
{
    TEST_ASSERT_EQUAL(DISPLAY_HUMIDITY, nextDisplayMode(DISPLAY_TEMPERATURE));
}

void test_next_from_motion_wraps_to_temperature(void)
{
    TEST_ASSERT_EQUAL(DISPLAY_TEMPERATURE, nextDisplayMode(DISPLAY_MOTION));
}

void test_previous_from_humidity_is_temperature(void)
{
    TEST_ASSERT_EQUAL(DISPLAY_TEMPERATURE, previousDisplayMode(DISPLAY_HUMIDITY));
}

void test_previous_from_temperature_wraps_to_motion(void)
{
    TEST_ASSERT_EQUAL(DISPLAY_MOTION, previousDisplayMode(DISPLAY_TEMPERATURE));
}

/* ---------- System state (4 tests) ---------- */

void test_active_without_timeout_stays_active(void)
{
    TEST_ASSERT_EQUAL(SYSTEM_ACTIVE,
        evaluateSystemState(SYSTEM_ACTIVE, false, 10000U, INACTIVITY_TIMEOUT_MS));
}

void test_active_after_timeout_becomes_inactive(void)
{
    TEST_ASSERT_EQUAL(SYSTEM_INACTIVE,
        evaluateSystemState(SYSTEM_ACTIVE, false, INACTIVITY_TIMEOUT_MS, INACTIVITY_TIMEOUT_MS));
}

void test_inactive_without_motion_stays_inactive(void)
{
    TEST_ASSERT_EQUAL(SYSTEM_INACTIVE,
        evaluateSystemState(SYSTEM_INACTIVE, false, 60000U, INACTIVITY_TIMEOUT_MS));
}

void test_inactive_with_motion_becomes_active(void)
{
    TEST_ASSERT_EQUAL(SYSTEM_ACTIVE,
        evaluateSystemState(SYSTEM_INACTIVE, true, 60000U, INACTIVITY_TIMEOUT_MS));
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_temperature_below_lower_limit_is_low);
    RUN_TEST(test_temperature_exactly_lower_limit_is_normal);
    RUN_TEST(test_temperature_normal_value_is_normal);
    RUN_TEST(test_temperature_exactly_upper_limit_is_normal);
    RUN_TEST(test_temperature_above_upper_limit_is_high);

    RUN_TEST(test_next_from_temperature_is_humidity);
    RUN_TEST(test_next_from_motion_wraps_to_temperature);
    RUN_TEST(test_previous_from_humidity_is_temperature);
    RUN_TEST(test_previous_from_temperature_wraps_to_motion);

    RUN_TEST(test_active_without_timeout_stays_active);
    RUN_TEST(test_active_after_timeout_becomes_inactive);
    RUN_TEST(test_inactive_without_motion_stays_inactive);
    RUN_TEST(test_inactive_with_motion_becomes_active);

    return UNITY_END();
}