#include <stdio.h>
#include <iostream>
#include <string>
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
        EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, json));\
		EXPECT_EQ_INT(TINY_NUMBER, tiny_get_type(&v));\
        EXPECT_EQ_INT(expect, tiny_get_number(&v));\
    } while(0)
#define TEST_ERROR(error, json)\
    do {\
        tiny_value v;\
        v.type = TINY_FASLE;\
        EXPECT_EQ_INT(error, tiny_parse(&v, json));\
        EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v));\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual)
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
static void test_parse_number() {
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
static void test_parse() {
	//test_parse_null();
	test_parse_number();
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