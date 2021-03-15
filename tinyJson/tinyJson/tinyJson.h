#include <iostream>
#include <assert.h>
#ifndef TINYJSON_H_
#define TINYJSON_H_

typedef enum{ TINY_NULL=0,TINY_TRUE,TINY_FASLE,TINY_STRING,TINY_ARRAY,TINY_NUMBER,TINY_OBJECT }tiny_type;
enum {
	TINY_PARSE_OK = 0,
	TINY_PARSE_EXPECT_VALUE,//若一个 JSON 只含有空白，传回 TINY_PARSE_EXPECT_VALUE
	TINY_PARSE_INVALID_VALUE,//若一个值之后，在空白之后还有其他字符，传回 TINY_PARSE_ROOT_NOT_SINGULAR
	TINY_PARSE_ROOT_NOT_SINGULAR,//若值不是那三种字面值，传回 TINY_PARSE_INVALID_VALUE
	TINY_PARSE_NUMBER_TOO_BIG,//数值过大
	TINY_PARSE_MISS_QUOTATION_MARK,//缺失后引用号
	TINY_PARSE_INVALID_STRING_CHAR,//不合法的字符是 %x00 至 %x1F
	TINY_PARSE_INVALID_UNICODE_SURROGATE,//只有高代理项而欠缺低代理项，或是低代理项不在合法码点范围
	TINY_PARSE_INVALID_UNICODE_HEX,//\u 后不是 4 位十六进位数字
	TINY_PARSE_INVALID_STRING_ESCAPE, //其他无效字符
	TINY_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, //缺失逗号或者右中括号
	TINY_PARSE_MISS_KEY,
	TINY_PARSE_MISS_COLON,
	TINY_PARSE_MISS_COMMA_OR_CURLY_BRACKET,

	TINY_PARSE_STRINGIFY_INIT_SIZE
};
typedef struct tiny_value tiny_value;
typedef struct tiny_member tiny_member;
struct tiny_value{
	union {
		struct { char* s; size_t len; }s;//字符串类型
		struct { tiny_value* e; size_t size; }a;//数组类型,动态数组，用时分配空间
		struct { tiny_member* m; size_t size; }o;//对象类型
		double n;//数字类型
	}u;
	tiny_type type;
};
struct tiny_member {
	char* k; size_t klen;
	tiny_value v;
};

	#define tiny_init(v) do{(v)->type = TINY_NULL;}while(0)
	int tiny_parse(tiny_value* v, const char* json);

	tiny_type tiny_get_type(const tiny_value* v);
	
	double tiny_get_number(const tiny_value* v);
	void tiny_set_number(tiny_value* v, double n);

	int tiny_get_boolean(const tiny_value* v);
	void tiny_set_boolean(const tiny_value* v);

	const char* tiny_get_string(const tiny_value* v);
	size_t tiny_get_string_length(const tiny_value* v);
	void tiny_set_string(tiny_value* v, const char* c, size_t len);

	size_t tiny_get_array_size(const tiny_value* v);
	tiny_value* tiny_get_array_element(const tiny_value* v, size_t index);

	size_t tiny_get_object_size(const tiny_value* v);
	const char* tiny_get_object_key(const tiny_value* v, size_t index);
	size_t tiny_get_object_key_length(const tiny_value* v, size_t index);
	tiny_value* tiny_get_object_value(const tiny_value* v, size_t index);

	char* tiny_stringfy(const tiny_value* v, size_t* length);

	void tiny_free(tiny_value* v);

#endif // !TINYJSON_H_
