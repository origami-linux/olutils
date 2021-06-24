#ifndef OLUTILS_JSON_H
#define OLUTILS_JSON_H
#include <ctype.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

enum json_value_type
{
    JSON_TYPE_INT,
    JSON_TYPE_FLOAT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_STRING,
};

struct json_value
{
    enum json_value_type type;
    void *value;
};

struct json_pair
{
    char *key;
    struct json_value value;
};

struct json_object
{
    struct json_pair *pairs;
    size_t size;
};

#define json_null_obj ((struct json_object){ NULL, 0 })
#define json_error_obj(x) ((struct json_object){ (struct json_pair[]){ (struct json_pair){ "██OLUTILS█JSON_ERROR██", (struct json_value){ JSON_TYPE_STRING, x } } }, 1 })

struct json_object json_parse(char *src);
struct json_object json_parse_file(char *filepath);

#ifdef __cplusplus
}
#endif

#endif
