#include <stdio.h>
#include <iostream>
#include <string.h>
#include "tinyJson.h"
using namespace std;

int main_ret = 0;
int test_pass = 0;
int test_count = 0;
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual)
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
#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true", "false")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == 0, "false", "true")
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
 void test_parse_array() {
	 size_t i, j;
	 tiny_value v;

	 tiny_init(&v);
	 EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, "[ ]"));
	 EXPECT_EQ_INT(TINY_ARRAY, tiny_get_type(&v));
	 EXPECT_EQ_SIZE_T(0, tiny_get_array_size(&v));
	 tiny_free(&v);

	 tiny_init(&v);
	 EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, "[ null , false , true , 123 , \"abc\" ]"));
	 EXPECT_EQ_INT(TINY_ARRAY, tiny_get_type(&v));
	 EXPECT_EQ_SIZE_T(5, tiny_get_array_size(&v));
	 EXPECT_EQ_INT(TINY_NULL, tiny_get_type(tiny_get_array_element(&v, 0)));
	 EXPECT_EQ_INT(TINY_FASLE, tiny_get_type(tiny_get_array_element(&v, 1)));
	 EXPECT_EQ_INT(TINY_TRUE, tiny_get_type(tiny_get_array_element(&v, 2)));
	 EXPECT_EQ_INT(TINY_NUMBER, tiny_get_type(tiny_get_array_element(&v, 3)));
	 EXPECT_EQ_INT(TINY_STRING, tiny_get_type(tiny_get_array_element(&v, 4)));
	 EXPECT_EQ_INT(123.0, tiny_get_number(tiny_get_array_element(&v, 3)));
	 EXPECT_EQ_STRING("abc", tiny_get_string(tiny_get_array_element(&v, 4)), tiny_get_string_length(tiny_get_array_element(&v, 4)));
	 tiny_free(&v);

	 tiny_init(&v);
	 EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
	 EXPECT_EQ_INT(TINY_ARRAY, tiny_get_type(&v));
	 EXPECT_EQ_SIZE_T(4, tiny_get_array_size(&v));
	 for (i = 0; i < 4; i++) {
		 tiny_value* a = tiny_get_array_element(&v, i);
		 EXPECT_EQ_INT(TINY_ARRAY, tiny_get_type(a));
		 EXPECT_EQ_SIZE_T(i, tiny_get_array_size(a));
		 for (j = 0; j < i; j++) {
			 tiny_value* e = tiny_get_array_element(a, j);
			 EXPECT_EQ_INT(TINY_NUMBER, tiny_get_type(e));
			 EXPECT_EQ_INT((double)j, tiny_get_number(e));
		 }
	 }
	 tiny_free(&v);
 }

 static void test_parse_object() {
	 tiny_value v;
	 size_t i;

	 tiny_init(&v);
	 EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, " { } "));
	 EXPECT_EQ_INT(TINY_OBJECT, tiny_get_type(&v));
	 EXPECT_EQ_SIZE_T(0, tiny_get_object_size(&v));
	 tiny_free(&v);

	 tiny_init(&v);
	 EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v,
		 " { "
		 "\"n\" : null , "
		 "\"f\" : false , "
		 "\"t\" : true , "
		 "\"i\" : 123 , "
		 "\"s\" : \"abc\", "
		 "\"a\" : [ 1, 2, 3 ],"
		 "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
		 " } "
	 ));
	 EXPECT_EQ_INT(TINY_OBJECT, tiny_get_type(&v));
	 EXPECT_EQ_SIZE_T(7, tiny_get_object_size(&v));
	 EXPECT_EQ_STRING("n", tiny_get_object_key(&v, 0), tiny_get_object_key_length(&v, 0));
	 EXPECT_EQ_INT(TINY_NULL, tiny_get_type(tiny_get_object_value(&v, 0)));
	 EXPECT_EQ_STRING("f", tiny_get_object_key(&v, 1), tiny_get_object_key_length(&v, 1));
	 EXPECT_EQ_INT(TINY_FASLE, tiny_get_type(tiny_get_object_value(&v, 1)));
	 EXPECT_EQ_STRING("t", tiny_get_object_key(&v, 2), tiny_get_object_key_length(&v, 2));
	 EXPECT_EQ_INT(TINY_TRUE, tiny_get_type(tiny_get_object_value(&v, 2)));
	 EXPECT_EQ_STRING("i", tiny_get_object_key(&v, 3), tiny_get_object_key_length(&v, 3));
	 EXPECT_EQ_INT(TINY_NUMBER, tiny_get_type(tiny_get_object_value(&v, 3)));
	 EXPECT_EQ_INT(123.0, tiny_get_number(tiny_get_object_value(&v, 3)));
	 EXPECT_EQ_STRING("s", tiny_get_object_key(&v, 4), tiny_get_object_key_length(&v, 4));
	 EXPECT_EQ_INT(TINY_STRING, tiny_get_type(tiny_get_object_value(&v, 4)));
	 EXPECT_EQ_STRING("abc", tiny_get_string(tiny_get_object_value(&v, 4)), tiny_get_string_length(tiny_get_object_value(&v, 4)));
	 EXPECT_EQ_STRING("a", tiny_get_object_key(&v, 5), tiny_get_object_key_length(&v, 5));
	 EXPECT_EQ_INT(TINY_ARRAY, tiny_get_type(tiny_get_object_value(&v, 5)));
	 EXPECT_EQ_SIZE_T(3, tiny_get_array_size(tiny_get_object_value(&v, 5)));
	 for (i = 0; i < 3; i++) {
		 tiny_value* e = tiny_get_array_element(tiny_get_object_value(&v, 5), i);
		 EXPECT_EQ_INT(TINY_NUMBER, tiny_get_type(e));
		 EXPECT_EQ_INT(i + 1.0, tiny_get_number(e));
	 }
	 EXPECT_EQ_STRING("o", tiny_get_object_key(&v, 6), tiny_get_object_key_length(&v, 6));
	 {
		 tiny_value* o = tiny_get_object_value(&v, 6);
		 EXPECT_EQ_INT(TINY_OBJECT, tiny_get_type(o));
		 for (i = 0; i < 3; i++) {
			 tiny_value* ov = tiny_get_object_value(o, i);
			 EXPECT_TRUE('1' + i == tiny_get_object_key(o, i)[0]);
			 EXPECT_EQ_SIZE_T(1, tiny_get_object_key_length(o, i));
			 EXPECT_EQ_INT(TINY_NUMBER, tiny_get_type(ov));
			 EXPECT_EQ_INT(i + 1.0, tiny_get_number(ov));
		 }
	 }
	 tiny_free(&v);
 }
static void test_parse() {
	//test_parse_null();
	//test_parse_number();
	//test_parse_string();
	test_parse_array();
	test_parse_object();
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