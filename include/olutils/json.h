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
    JSON_TYPE_ERROR
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

#define json_new_value(type, value) ((struct json_value){ type, value })
#define json_new_pair(key, value) ((struct json_pair){ key, value })
#define json_null_obj ((struct json_object){ NULL, 0 })
#define json_error_obj(msg) ((struct json_object){ (struct json_pair[]){ (struct json_pair){ "██OLUTILS█JSON_ERROR██", (struct json_value){ JSON_TYPE_STRING, msg } } }, 1 })

long json_obj_find(struct json_object obj, char *key);
void json_obj_add_pair(struct json_object *obj, struct json_pair pair);
void json_obj_add(struct json_object *obj, char *key, struct json_value value);
void json_obj_remove(struct json_object *obj, char *key);
struct json_pair json_obj_get_pair(struct json_object obj, char *key);
struct json_value json_obj_get(struct json_object obj, char *key);

struct json_object json_parse(char *src);
struct json_object json_parse_file(char *filepath);

#ifdef __cplusplus
}
#endif

#endif
