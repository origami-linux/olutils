#include <stdint.h>
#include "olutils/bool.h"

// IO
#include "olutils/io.h"

char *read_file(char *path, long *len_ref, bool binary)
{
    char *buffer = 0;
    long length;
    FILE *f;
    if(binary)
        f = fopen(path, "rb");
    else
        f = fopen(path, "r");

    if(!f)
    {
        fprintf(stderr, "Unable to open file '%s'\n", path);
        exit(EXIT_FAILURE);
    }
    
    fseek(f, 0, SEEK_END);
    length = ftell (f);
    fseek(f, 0, SEEK_SET);
    buffer = malloc(length);
    if(!buffer)
    {
        fprintf(stderr, "Unable to allocate buffer\n");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, length, f);
    fclose(f);

    if(len_ref != NULL)
        *len_ref = length;

    return buffer;
}

void write_file(char *path, char *buffer, long length)
{
    FILE *f = fopen(path, "wb");

    if(!f)
    {
        fprintf(stderr, "Unable to open or create file '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    fwrite(buffer, 1, length, f);
    fclose(f);
}

// STR
#include "olutils/str.h"

char *substr(char *src, size_t start_index, size_t length)
{
    char *ret = malloc(length + 1);

    for(size_t i = 0; i < length; i++)
    {
        ret[i] = src[i + start_index];
    }

    ret[length] = '\0';

    return ret;
}

// JSON
#include "olutils/json.h"

long json_obj_find(struct json_object obj, char *key)
{
    for(long i = 0; i < obj.size; i++)
        if(strcmp(obj.pairs[i].key, key) == 0)
            return i;

    return -1;
}

void json_obj_add_pair(struct json_object *obj, struct json_pair pair)
{
    if(json_obj_find(*obj, pair.key) != -1)
        return;

    obj->size++;
    obj->pairs = realloc(obj->pairs, obj->size);
    obj->pairs[obj->size - 1] = pair;
}

void json_obj_add(struct json_object *obj, char *key, struct json_value value)
{
    json_obj_add_pair(obj, json_new_pair(key, value));
}

void json_obj_remove(struct json_object *obj, char *key)
{
    long pos = json_obj_find(*obj, key);
    if(pos == -1)
        return;

    for(; pos < obj->size - 1; pos++)
        obj->pairs[pos] = obj->pairs[pos + 1];

    obj->size--;
    obj->pairs = realloc(obj->pairs, obj->size);
}

struct json_pair json_obj_get_pair(struct json_object obj, char *key)
{
    long pos = json_obj_find(obj, key);
    if(pos == -1)
        return json_new_pair("OLUTILS_JSON_ERROR", json_new_value(JSON_TYPE_ERROR, "no match"));
}

struct json_value json_obj_get(struct json_object obj, char *key)
{
    return json_obj_get_pair(obj, key).value;
}

struct json_object json_parse_obj(char *src, size_t *i)
{
    struct json_object ret = json_null_obj;

    for(; *i < strlen(src); *i += 1)
    {
        char ch = src[*i];
        switch(ch)
        {
            case ' ':
            case '\t':
            case '\v':  // apparently vertical tabs exist
            case '\n':
                break;

            case '"':
            {
                size_t len = 0;
                char *key = NULL;

                for(*i += 1; *i < strlen(src); *i += 1)
                {
                    ch = src[*i];

                    if(ch == '"')
                        break;

                    switch(ch)
                    {
                        case '\v':
                        case '\n':
                        case '\0':
                            return json_error_obj("unexpected end of string");

                        case '\\':
                        {
                            *i += 1;
                            ch = src[*i];

                            switch(ch)
                            {
                                case '"':
                                {
                                    len++;
                                    key = realloc(key, len);
                                    key[len - 1] = '"';
                                } break;

                                case '\\':
                                {
                                    len++;
                                    key = realloc(key, len);
                                    key[len - 1] = '\\';
                                } break;

                                case '/':
                                {
                                    len++;
                                    key = realloc(key, len);
                                    key[len - 1] = '/';
                                } break;

                                case 'b':
                                {
                                    len++;
                                    key = realloc(key, len);
                                    key[len - 1] = '\b';
                                } break;
                                
                                case 'f':
                                {
                                    len++;
                                    key = realloc(key, len);
                                    key[len - 1] = '\f';
                                } break;

                                case 'n':
                                {
                                    len++;
                                    key = realloc(key, len);
                                    key[len - 1] = '\n';
                                } break;

                                case 'r':
                                {
                                    len++;
                                    key = realloc(key, len);
                                    key[len - 1] = '\r';
                                } break;

                                case 't':
                                {
                                    len++;
                                    key = realloc(key, len);
                                    key[len - 1] = '\t';
                                } break;

                                case 'u':
                                {
                                    key = realloc(key, len);

                                    if(*i + 4 < strlen(src))
                                    {
                                        if(isxdigit(src[*i + 1]) && isxdigit(src[*i + 2]) &&
                                           isxdigit(src[*i + 3]) && isxdigit(src[*i + 4]))
                                        {
                                            wchar_t ucode = strtol((char[]){ src[*i + 1], src[*i + 2], src[*i + 3], src[*i + 4] }, NULL, 16);
                                            *i += 4;
 
                                            char mbch[4];
                                            int mblen = wctomb(mbch, ucode);
                                            for(int j = 0; j < mblen; j++)
                                            {
                                                len++;
                                                key[len - 1] = mbch[j];
                                            }
                                        }
                                        else
                                        {
                                            return json_error_obj("expected 4 hex digits");
                                        }
                                    }
                                    else
                                    {
                                        return json_error_obj("expected 4 hex digits");
                                    }
                                    *i += 4;
                                } break;
                            }
                        } break;

                        default:
                        {
                            len++;
                            key = realloc(key, len);
                            key[len - 1] = ch;
                        } break;
                    }
                }

                len++;
                key = realloc(key, len);
                key[len - 1] = '\0';

                for(*i += 1; *i < strlen(src); *i += 1)
                {
                    ch = src[*i];

                    if(ch == ':')
                        break;

                    switch(ch)
                    {
                        case '\v':
                        case '\n':
                            break;

                        default:
                        {
                            return json_error_obj("expected character ':'");   
                        } break;
                    }
                }

            } break;

            default:
                return json_error_obj("expected character '{'");
        }
    }
}

struct json_object json_parse(char *src)
{
    struct json_object ret = json_null_obj;

    for(size_t i = 0; i < strlen(src); i++)
    {
        char ch = src[i];
        switch(ch)
        {
            case ' ':
            case '\t':
            case '\v':  // apparently vertical tabs exist
            case '\n':
                break;

            case '{':
            {
                i++;
                json_parse_obj(src, &i);
            } break;

            default:
                return json_error_obj("expected character '{'");
        }
    }

    return ret;
}

struct json_object json_parse_file(char *filepath)
{
    return json_parse(read_file(filepath, NULL, false));
}
