#include <stdio.h>
#include <iostream>
#include <string.h>
#include "tinyJson.h"
using namespace std;

int main_ret = 0;
int test_pass = 0;
int test_count = 0;

#define EXPECT_EQ_BASE(equality, expect, actual) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
			cout<<__FILE__<<":"<<__LINE__<<":"<<"expect:"<<expect<<"actual:"<<actual<<endl;\
            main_ret = 1;\
        }\
    } while(0)
#define TEST_NUMBER(expect, json)\
    do {\
        tiny_value v;\
		tiny_init(&v);\
        EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, json));\
		EXPECT_EQ_INT(TINY_NUMBER, tiny_get_type(&v));\
        EXPECT_EQ_INT(expect, tiny_get_number(&v));\
		tiny_free(&v);\
    } while(0)
#define TEST_ERROR(error, json)\
    do {\
        tiny_value v;\
        v.type = TINY_FASLE;\
        EXPECT_EQ_INT(error, tiny_parse(&v, json));\
        EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v));\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect)	 == (actual), expect, actual)
#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE((sizeof(expect) - 1 == alength && memcmp(expect, actual, alength)) == 0, expect, actual)
#define TEST_STRING(expect, json)\
    do {\
        tiny_value v;\
        tiny_init(&v);\
        EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, json));\
        EXPECT_EQ_INT(TINY_STRING, tiny_get_type(&v));\
        EXPECT_EQ_STRING(expect, tiny_get_string(&v), tiny_get_string_length(&v));\
        tiny_free(&v);\
    } while(0)
static void test_parse_null() {
	tiny_value v;
	v.type = TINY_FASLE;
	EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, "null"));
	EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v));
}

void test_parse_expect_value() {
	TEST_ERROR(TINY_PARSE_EXPECT_VALUE, "");
	TEST_ERROR(TINY_PARSE_EXPECT_VALUE, " ");
}
 void test_parse_number() {
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

	TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
	TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}	 
 void test_parse_string() {
	 TEST_STRING("", "\"\"");
	 TEST_STRING("Hello", "\"Hello\"");
	 TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	 TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
	 TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
	 TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
	 TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
	 TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
	 TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
	 TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

static void test_parse() {
	//test_parse_null();
	//test_parse_number();
	test_parse_string();
	//test_parse_true();
	//test_parse_false();
	/*test_parse_expect_value();*/
	//test_parse_invalid_value();
	//test_parse_root_not_singular();
}
int main() {
	test_parse();
	cout << test_pass << "/" << test_count <<" "<< test_pass * 100.0 / test_count << "% passed";
	return 0;
}