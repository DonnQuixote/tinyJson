#include <iostream>
#include <assert.h>
#ifndef TINYJSON_H_
#define TINYJSON_H_

typedef enum{ TINY_NULL=0,TINY_TRUE,TINY_FASLE,TINY_STRING,TINY_ARRAY,TINY_NUMBER,TINY_OBJECT }tiny_type;
enum {
	TINY_PARSE_OK = 0,
	TINY_PARSE_EXPECT_VALUE,//若一个 JSON 只含有空白，传回 LEPT_PARSE_EXPECT_VALUE
	TINY_PARSE_INVALID_VALUE,//若一个值之后，在空白之后还有其他字符，传回 LEPT_PARSE_ROOT_NOT_SINGULAR
	TINY_PARSE_ROOT_NOT_SINGULAR,//若值不是那三种字面值，传回 LEPT_PARSE_INVALID_VALUE
	LEPT_PARSE_NUMBER_TOO_BIG
};
typedef struct {
	tiny_type type;
	double n;
}tiny_value;

	int tiny_parse(tiny_value* v, const char* json);
	tiny_type tiny_get_type(const tiny_value* v);
	double tiny_get_number(const tiny_value* v);


#endif // !TINYJSON_H_
