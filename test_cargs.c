#define CARGS_MAX_FLAGS 50
#define CARGS_MAX_SUBCOMMANDS 20
#define CARGS_IMPLEMENTATION
#include "cargs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// --- Framework ---

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(condition, msg) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "\n  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            tests_failed++; \
            return; \
        } \
    } while(0)

#define ASSERT_EQ_INT(expected, actual) \
    ASSERT((expected) == (actual), "Integer mismatch: " #expected " != " #actual)

#define ASSERT_EQ_FLOAT(expected, actual) \
    ASSERT(fabs((expected) - (actual)) < 0.0001, "Float mismatch")

#define ASSERT_EQ_STR(expected, actual) \
    ASSERT(strcmp((expected), (actual)) == 0, "String mismatch")

#define TEST(name) static void test_##name(void)

#define RUN_TEST(name) \
    do { \
        fprintf(stderr, "RUN  %-30s ", #name); \
        int initial_fails = tests_failed; \
        test_##name(); \
        tests_run++; \
        if (tests_failed == initial_fails) { \
            fprintf(stderr, "[OK]\n"); \
            tests_passed++; \
        } \
    } while(0)

#define PARSE_MOCK(...) \
    cargs_parse(sizeof((char *[]){"test_bin", __VA_ARGS__}) / sizeof(char *), (char *[]){"test_bin", __VA_ARGS__})


// --- Data Type Parsing & Boundaries ---

TEST(type_boolean)
{
    cargs_reset();
    bool b1, b2, b3, b4, b5;
    cargs_bool("b1", NULL, &b1, false, NULL, NULL, "-");
    cargs_bool("b2", NULL, &b2, true, NULL, NULL, "-");
    cargs_bool("b3", NULL, &b3, true, NULL, NULL, "-");
    cargs_bool("b4", NULL, &b4, false, NULL, NULL, "-");
    cargs_bool("b5", "i", &b5, false, NULL, NULL, "-");

    int res = PARSE_MOCK("--b1=true", "--b2=false", "--b3=0", "--b4=1", "-i");

    ASSERT(res != -1, "Parsing failed");
    ASSERT(b1 == true, "b1");
    ASSERT(b2 == false, "b2");
    ASSERT(b3 == false, "b3");
    ASSERT(b4 == true, "b4");
    ASSERT(b5 == true, "b5");

    cargs_reset();
    cargs_bool("err", NULL, &b1, false, NULL, NULL, "-");
    res = PARSE_MOCK("--err=yes");
    ASSERT_EQ_INT(-1, res);
    ASSERT_EQ_INT(CARGS_INVALID_BOOL, cargs_get_error());
}

TEST(type_int_signed)
{
    cargs_reset();
    int val;
    cargs_int("val", NULL, &val, 0, NULL, NULL, "-");

    ;
    ASSERT(PARSE_MOCK("--val=2147483647") != -1, "Max int");
    ASSERT_EQ_INT(2147483647, val);

    cargs_reset();
    cargs_int("val", NULL, &val, 0, NULL, NULL, "-");
    ASSERT(PARSE_MOCK("--val=-2147483648") != -1, "Min int");
    ASSERT_EQ_INT(-2147483648, val);

    cargs_reset();
    cargs_int("val", NULL, &val, 0, NULL, NULL, "-");
    ASSERT(PARSE_MOCK("--val=2147483648") == -1, "Max int overflow");
    ASSERT_EQ_INT(CARGS_INVALID_INT, cargs_get_error());

    cargs_reset();
    cargs_int("val", NULL, &val, 0, NULL, NULL, "-");
    ASSERT(PARSE_MOCK("--val=123x") == -1, "Invalid int");
}

TEST(type_int_unsigned)
{
    cargs_reset();
    unsigned int val;
    cargs_uint("val", NULL, &val, 0, NULL, NULL, "-");

    ASSERT(PARSE_MOCK("--val=4294967295") != -1, "Max uint");
    ASSERT(val == 4294967295U, "Value match");

    cargs_reset();
    cargs_uint("val", NULL, &val, 0, NULL, NULL, "-");
    ASSERT(PARSE_MOCK("--val=-1") == -1, "Invalid uint");
    ASSERT_EQ_INT(CARGS_INVALID_UINT, cargs_get_error());
}

TEST(type_float)
{
    cargs_reset();
    float val;
    cargs_float("val", NULL, &val, 0.0f, NULL, NULL, "-");

    ASSERT(PARSE_MOCK("--val=-3.14159") != -1, "Valid float");
    ASSERT_EQ_FLOAT(-3.14159f, val);

    cargs_reset();
    cargs_float("val", NULL, &val, 0.0f, NULL, NULL, "-");
    ASSERT(PARSE_MOCK("--val=1e39") == -1, "Overflow 32-bit float");
    ASSERT_EQ_INT(CARGS_INVALID_FLOAT, cargs_get_error());
}

TEST(type_char_and_string)
{
    cargs_reset();
    char c;
    const char *s;
    cargs_char("char", NULL, &c, 0, NULL, NULL, "-");
    cargs_string("str", NULL, &s, NULL, NULL, NULL, "-");

    ASSERT(PARSE_MOCK("--char=X", "--str", "test_string") != -1, "Valid types");
    ASSERT(c == 'X', "Char match");
    ASSERT_EQ_STR("test_string", s);

    cargs_reset();
    cargs_char("char", NULL, &c, 0, NULL, NULL, "-");
    ASSERT(PARSE_MOCK("--char=XY") == -1, "Invalid char");
    ASSERT_EQ_INT(CARGS_INVALID_CHAR, cargs_get_error());
}


// --- Syntax & Delimiters ---

TEST(syntax_long_assignment)
{
    cargs_reset();
    const char *s1;
    const char *s2;
    const char *s3;
    cargs_string("s1", NULL, &s1, NULL, "V", NULL, "-");
    cargs_string("s2", NULL, &s2, NULL, "V", NULL, "-");
    cargs_string("s3", NULL, &s3, "default", "V", NULL, "-");

    ASSERT(PARSE_MOCK("--s1=v1", "--s2", "v2", "--s3=") != -1, "Parse");
    ASSERT_EQ_STR("v1", s1);
    ASSERT_EQ_STR("v2", s2);
    ASSERT_EQ_STR("", s3);
}

TEST(syntax_short_cluster)
{
    cargs_reset();
    bool a, b, c;
    const char *str1;
    const char *str2;
    cargs_bool(NULL, "a", &a, false, NULL, NULL, "-");
    cargs_bool(NULL, "b", &b, false, NULL, NULL, "-");
    cargs_bool(NULL, "c", &c, false, NULL, NULL, "-");
    cargs_string(NULL, "f", &str1, NULL, "V", NULL, "-");
    cargs_string(NULL, "g", &str2, NULL, "V", NULL, "-");

    ASSERT(PARSE_MOCK("-ab", "-cfval1", "-g", "val2") != -1, "Parse cluster");
    ASSERT(a && b && c, "Booleans true");
    ASSERT_EQ_STR("val1", str1); // Inline argument parsing
    ASSERT_EQ_STR("val2", str2); // Next token argument parsing
}

TEST(syntax_prefix_resolution)
{
    cargs_reset();
    bool verb, verbose;
    cargs_bool("verb", NULL, &verb, false, NULL, NULL, "-");
    cargs_bool("verbose", NULL, &verbose, false, NULL, NULL, "-");

    ASSERT(PARSE_MOCK("--verb") != -1, "Prefix isolate");
    ASSERT(verb == true, "Verb true");
    ASSERT(verbose == false, "Verbose false");
}


// --- Scope & Demarcation ---

TEST(scope_isolation)
{
    cargs_reset();
    bool g, l1, l2;
    cargs_bool("g", NULL, &g, false, NULL, NULL, "-");

    cargs_subcommand_start("c1", "-");
        cargs_bool("l1", NULL, &l1, false, NULL, NULL, "-");
    cargs_subcommand_end();

    cargs_subcommand_start("c2", "-");
        cargs_bool("l2", NULL, &l2, false, NULL, NULL, "-");
    cargs_subcommand_end();

    ASSERT(PARSE_MOCK("--g", "c1", "--l1") != -1, "Scope c1");
    ASSERT(g && l1 && !l2, "States correct");

    cargs_reset();
    cargs_bool("g", NULL, &g, false, NULL, NULL, "-");
    cargs_subcommand_start("c1", "-"); cargs_subcommand_end();

    ASSERT(PARSE_MOCK("c1", "--g") == -1, "Global flag AFTER subcommand entry");
    ASSERT_EQ_INT(CARGS_UNKNOWN_FLAG, cargs_get_error());
}

TEST(positional_demarcation)
{
    cargs_reset();
    bool a;
    cargs_bool("a", NULL, &a, false, NULL, NULL, "-");

    cargs_subcommand_start("cmd", "-"); cargs_subcommand_end();

    int res = PARSE_MOCK("--a", "arg1", "cmd");
    ASSERT(res != -1, "Parse");
    ASSERT_EQ_INT(2, res); // argv[2] == "arg1"
    ASSERT(cargs_get_active_subcommand() == NULL, "Subcmd treated as positional");

    cargs_reset();
    cargs_bool("a", NULL, &a, false, NULL, NULL, "-");
    res = PARSE_MOCK("-", "arg2"); // Fix: Supply standard positional token
    ASSERT(res != -1, "Parse stdin dash");
    ASSERT_EQ_INT(1, res); // argv[1] == "-"

    cargs_reset();
    cargs_bool("a", NULL, &a, false, NULL, NULL, "-");
    res = PARSE_MOCK("--", "--a");
    ASSERT(res != -1, "Parse terminator");
    ASSERT_EQ_INT(2, res);
    ASSERT(a == false, "--a bypassed");
}


// --- Hooks & State ---

static bool mock_validator(const char *name, const char *value) {
    (void)name;
    if (strcmp(value, "reject") == 0) {
        cargs_set_error(CARGS_INVALID_ARGUMENT, "validation rejected");
        return false;
    }
    return true;
}

TEST(custom_validation)
{
    cargs_reset();
    const char *str;
    cargs_string("str", NULL, &str, NULL, "V", mock_validator, "-");

    int res = PARSE_MOCK("--str", "pass");
    ASSERT(res != -1, "Valid hook");

    cargs_reset();
    cargs_string("str", NULL, &str, NULL, "V", mock_validator, "-");
    res = PARSE_MOCK("--str", "reject");
    ASSERT_EQ_INT(-1, res);
    ASSERT_EQ_INT(CARGS_INVALID_ARGUMENT, cargs_get_error());
}

TEST(reset_persistence)
{
    cargs_reset();
    bool a;
    cargs_bool("a", NULL, &a, false, NULL, NULL, "-");
    int res = PARSE_MOCK("--a");
    ASSERT(res != -1, "Parse");
    ASSERT(a == true, "a is true"); // Fix: Assert mutation

    cargs_reset(); // Nuke

    // Ensure 'a' is not recognized
    res = PARSE_MOCK("--a");
    ASSERT_EQ_INT(-1, res);
    ASSERT_EQ_INT(CARGS_UNKNOWN_FLAG, cargs_get_error());
}

// --- Execution ---

int main(void)
{
    RUN_TEST(type_boolean);
    RUN_TEST(type_int_signed);
    RUN_TEST(type_int_unsigned);
    RUN_TEST(type_float);
    RUN_TEST(type_char_and_string);
    RUN_TEST(syntax_long_assignment);
    RUN_TEST(syntax_short_cluster);
    RUN_TEST(syntax_prefix_resolution);
    RUN_TEST(scope_isolation);
    RUN_TEST(positional_demarcation);
    RUN_TEST(custom_validation);
    RUN_TEST(reset_persistence);

    fprintf(stderr, "\nPass: %d | Fail: %d\n", tests_passed, tests_failed);
    return tests_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
