#include <iostream>
#include <assert.h>
#ifndef TINYJSON_H_
#define TINYJSON_H_

typedef enum{ TINY_NULL=0,TINY_TRUE,TINY_FASLE,TINY_STRING,TINY_ARRAY,TINY_NUMBER,TINY_OBJECT }tiny_type;
enum {
	TINY_PARSE_OK = 0,
	TINY_PARSE_EXPECT_VALUE,//��һ�� JSON ֻ���пհף����� LEPT_PARSE_EXPECT_VALUE
	TINY_PARSE_INVALID_VALUE,//��һ��ֵ֮���ڿհ�֮���������ַ������� LEPT_PARSE_ROOT_NOT_SINGULAR
	TINY_PARSE_ROOT_NOT_SINGULAR,//��ֵ��������������ֵ������ LEPT_PARSE_INVALID_VALUE
	TINY_PARSE_NUMBER_TOO_BIG,//��ֵ����
	TINY_PARSE_MISS_QUOTATION_MARK,//ȱʧ�����ú�
	TINY_PARSE_INVALID_STRING_CHAR,//���Ϸ����ַ��� %x00 �� %x1F
	TINY_PARSE_INVALID_STRING_ESCAPE,//��ʱ�Ȳ����� ת���ַ� '\u'
	LEPT_PARSE_INVALID_UNICODE_SURROGATE,//ֻ�иߴ������Ƿȱ�ʹ�������ǵʹ�����ںϷ���㷶Χ
	LEPT_PARSE_INVALID_UNICODE_HEX,//\u ���� 4 λʮ����λ����
	LEPT_PARSE_INVALID_STRING_ESCAPE //������Ч�ַ�

};
typedef struct tiny_value tiny_value;
typedef struct {
	union {
		struct { char* s; size_t len; }s;//�ַ�������
		struct { tiny_value* e; size_t size; }a;//��������,��̬���飬��ʱ����ռ�
		double n;//��������
	}u;
	tiny_type type;
}tiny_value;

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

	void tiny_free(tiny_value* v);

#endif // !TINYJSON_H_
