#include "tinyJson.h"
#define EXPECT(c,ch)	do{assert(*c->json==ch);c->json++; }while(0)
#define ISDIGIT1TO9(c)  (c>='1'&&c<='9')
#define ISDIGIT(c)		( c>='0'&&c<='9')
typedef struct{
	const char* json;
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
	v->n = strtod(c->json, NULL);
	if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
		return LEPT_PARSE_NUMBER_TOO_BIG;
	v->type = TINY_NUMBER;
	c->json = t;
	return TINY_PARSE_OK;
}

 int tiny_parse_value(tiny_value* v, tiny_context* c) {
	switch (*(c->json)) {
	case 'n':return tiny_parse_ntf(v, c,"null",TINY_NULL);
	case 't':; return tiny_parse_ntf(v, c, "true", TINY_TRUE);
	case 'f':; return tiny_parse_ntf(v, c, "false", TINY_FASLE);
	case '\0':return TINY_PARSE_EXPECT_VALUE;
	default:
		return tiny_parse_number(v,c);
	}
}
int tiny_parse(tiny_value * v, const char * json)
{
	assert(v != NULL);
	int ret;
	tiny_context c;
	c.json = json;
	v->type = TINY_NULL;
	parse_whitespace(&c);
	if ((ret = tiny_parse_value(v, &c)) == TINY_PARSE_OK) {
		parse_whitespace(&c);
		if (*c.json != '\0')
			ret = TINY_PARSE_ROOT_NOT_SINGULAR;
	}
	tiny_parse_value(v, &c);
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
	return v->n;
}

