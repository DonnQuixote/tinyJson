#include "tinyJson.h"
#include <assert.h>  /* assert() */
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */
#include <stdlib.h>  /* NULL, malloc(), realloc(), free(), strtod() */
#include <string.h>  /* memcpy() */
#include <iostream>
using namespace std;
#define EXPECT(c,ch)	do{assert(*c->json==ch);c->json++; }while(0)
#define ISDIGIT1TO9(c)  (c>='1'&&c<='9')
#define ISDIGIT(c)		( c>='0'&&c<='9')

#define PUTC(c,ch) do{*(char*)tiny_context_push(c,sizeof(ch)) = ch;}while(0)
#define STRING_ERROR(ret) do { c->top = head; cout<<"wrong"<<endl;return ret; } while(0)
#ifndef TINY_PRASE_STACK_INIT_SIZE
#define TINY_PRASE_STACK_INIT_SIZE 256
#endif
typedef struct{
	const char* json;
	char* stack;
	size_t size, top;
}tiny_context;

void parse_whitespace(tiny_context* c) {
	const char *p = c->json;
	while (*p == ' ' || *p == '\t' || *p == '\n'|| *p == '\r')
		p++;
	c->json = p;
}

int tiny_parse_ntf(tiny_value* v, tiny_context* c,const char* json,tiny_type type) {
	EXPECT(c, json[0]);
	size_t i;
	for (i = 0; json[i+1]; i++) {
		if (c->json[i] != json[i + 1])
			return TINY_PARSE_INVALID_VALUE;
	}
	c->json += i;
	v->type = type;
	return TINY_PARSE_OK;

}

void* tiny_context_push(tiny_context* c, size_t size) {//按照指定字节装入
	void* ret;
	assert(size > 0);
	if (c->top + size >= c->size) {
		if (c->size == 0)
			c->size = TINY_PRASE_STACK_INIT_SIZE;
		while (c->top+size>=c->size){
			c->size += c->size >> 1;
		}
		c->stack = (char*)realloc(c->stack, c->size);//按照c->size，小于原空间则扩大并复制，大于原空间则缩小内存范围,返回void*指针
	}
	ret = c->stack + c->top;
	c->top += size;
	return ret;
	
}
void* tiny_context_pop(tiny_context* c, size_t size) {
	assert(c->top >= size);
	return c->stack + (c->top -= size);
}
int tiny_parse_number(tiny_value* v, tiny_context* c) {
	const char* t = c->json;
	if (*t == '-') t++;
	if (*t == '0') t++;
	else {
		if (!ISDIGIT1TO9(*t)) return TINY_PARSE_INVALID_VALUE;
		t++;
		for (t; ISDIGIT(*t); t++);
	}
	if (*t == '.') {
		t++;
		if (!ISDIGIT(*t)) return TINY_PARSE_INVALID_VALUE;
		t++;
		for (t; ISDIGIT(*t); t++);
	}
	if (*t == 'e' || *t == 'E') {
		t++; 
		if (*t == '+' || *t == '-') t++;
		if (!ISDIGIT(*t)) return TINY_PARSE_INVALID_VALUE;
		t++;
		for (t; ISDIGIT(*t); t++);
	}
	errno = 0;
	v->u.n = strtod(c->json, NULL);
	if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
		return TINY_PARSE_NUMBER_TOO_BIG;
	v->type = TINY_NUMBER;
	c->json = t;
	return TINY_PARSE_OK;
}

const char* tiny_parse_hex4(const char* p, unsigned* u) {
	int i;
    *u = 0;
    for (i = 0; i < 4; i++) {
        char ch = *p++;
        *u <<= 4;
        if      (ch >= '0' && ch <= '9')  *u |= ch - '0';
        else if (ch >= 'A' && ch <= 'F')  *u |= ch - ('A' - 10);
        else if (ch >= 'a' && ch <= 'f')  *u |= ch - ('a' - 10);
        else return NULL;
    }
    return p;
}

 void tiny_encode_utf8(tiny_context* c, unsigned u) {
	if (u <= 0x7F)
		PUTC(c, u & 0xFF);
	else if (u <= 0x7FF) {
		PUTC(c, 0xC0 | ((u >> 6) & 0xFF));
		PUTC(c, 0x80 | (u & 0x3F));
	}
	else if (u <= 0xFFFF) {
		PUTC(c, 0xE0 | ((u >> 12) & 0xFF));
		PUTC(c, 0x80 | ((u >> 6) & 0x3F));
		PUTC(c, 0x80 | (u & 0x3F));
	}
	else {
		assert(u <= 0x10FFFF);
		PUTC(c, 0xF0 | ((u >> 18) & 0xFF));
		PUTC(c, 0x80 | ((u >> 12) & 0x3F));
		PUTC(c, 0x80 | ((u >> 6) & 0x3F));
		PUTC(c, 0x80 | (u & 0x3F));
	}
}
int tiny_parse_string(tiny_value* v, tiny_context* c) {
	size_t head = c->top, len;
	unsigned u, u2;
	const char* p;
	EXPECT(c, '\"');
	p = c->json;
	for (;;) {
		char ch = *p++;
		switch (ch) {
		case '\"':
			len = c->top - head;
			tiny_set_string(v, (const char*)tiny_context_pop(c, len), len);
			c->json = p;
			return TINY_PARSE_OK;
		case'\\':
			switch (*p++) {
			case '\"': PUTC(c, '\"'); break;
			case '\\': PUTC(c, '\\'); break;
			case '/':  PUTC(c, '/'); break;
			case 'b':  PUTC(c, '\b'); break;
			case 'f':  PUTC(c, '\f'); break;
			case 'n':  PUTC(c, '\n'); break;
			case 'r':  PUTC(c, '\r'); break;
			case 't':  PUTC(c, '\t'); break;
			case 'u':
				//std::cout << "in u" <<std:: endl;
				if (!(p =tiny_parse_hex4(p, &u)))
					STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
				if (u >= 0xD800 && u <= 0xDBFF) { /* surrogate pair */
					if (*p++ != '\\')
						STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
					if (*p++ != 'u')
						STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
					if (!(p = tiny_parse_hex4(p, &u2)))
						STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
					if (u2 < 0xDC00 || u2 > 0xDFFF)
						STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
					u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
				}
				tiny_encode_utf8(c, u);
				break;
			default:
				STRING_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE);
			}break;
		case '\0':
			STRING_ERROR(TINY_PARSE_MISS_QUOTATION_MARK);
		default:
			if ((unsigned char)ch < 0x20) {
				STRING_ERROR(TINY_PARSE_INVALID_STRING_CHAR);
			}
			PUTC(c, ch);
		}
	}

}


int tiny_parse_value(tiny_value* v, tiny_context* c) {
	switch (*(c->json)) {
	case 'n':return tiny_parse_ntf(v, c,"null",TINY_NULL);
	case 't': return tiny_parse_ntf(v, c, "true", TINY_TRUE);
	case 'f': return tiny_parse_ntf(v, c, "false", TINY_FASLE);
	case '\0':return TINY_PARSE_EXPECT_VALUE;
	case '"': return tiny_parse_string(v, c);
	default:
		return tiny_parse_number(v,c);
	}
}
int tiny_parse(tiny_value * v, const char * json)
{
	assert(v != NULL);
	int ret;
	tiny_context c;
	c.stack = NULL;
	c.size = c.top = 0;
	c.json = json;
	tiny_init(v);
	parse_whitespace(&c);
	if ((ret = tiny_parse_value(v, &c)) == TINY_PARSE_OK) {
		parse_whitespace(&c);
		if (*c.json != '\0') {
			v->type = TINY_NULL;
			ret = TINY_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	assert(c.top == 0);
	free(c.stack);
	//cout << ret << endl;
	return ret;
}

tiny_type tiny_get_type(const tiny_value * v)
{
	assert(v != NULL);
	return v->type;
}

double tiny_get_number(const tiny_value * v)
{
	assert(v != NULL&&v->type == TINY_NUMBER);
	return v->u.n;
}

const char * tiny_get_string(const tiny_value * v)
{
	assert(v != NULL && v->type == TINY_STRING);
	return v->u.s.s;
}

size_t tiny_get_string_length(const tiny_value * v)
{
	assert(v != NULL && v->type == TINY_STRING);
	return v->u.s.len;
}

void tiny_set_string(tiny_value * v, const char * c, size_t len)
{
	assert(v != NULL && (c != NULL || len == 0));
	tiny_free(v);
	v->u.s.s = (char*)malloc(len + 1);
	memcpy(v->u.s.s, c, len);
	v->u.s.s[len] = '\0';
	v->u.s.len = len;
	v->type = TINY_STRING;
}

void tiny_free(tiny_value * v)
{
	assert(v != NULL);
	if (v->type == TINY_STRING)
		free(v->u.s.s);
	v->type = TINY_NULL;
}
