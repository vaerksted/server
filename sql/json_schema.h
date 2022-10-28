#ifndef JSON_SCHEMA_INCLUDED
#define JSON_SCHEMA_INCLUDED

/* Copyright (c) 2016, 2021, MariaDB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */


/* This file defines all json schema classes. */

#include "sql_class.h"
#include "sql_type_json.h"
#include "json_schema_helper.h"

class Json_schema_keyword : public Sql_alloc
{
  public:
    virtual bool validate(json_engine_t *je) {return false;}
    virtual bool handle_keyword(THD *thd, json_engine_t *je,
                                List<HASH> *hash_list,
                                const char* key_start,
                                const char* key_end,
                                List<Json_schema_keyword> *all_keywords)
    {
      return false;
    }
    virtual ~Json_schema_keyword() = default;
    virtual void cleanup() { return; }
};

class Json_schema_annotation : public Json_schema_keyword
{
  public:
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_format : public Json_schema_keyword
{
  public:
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

typedef List<Json_schema_keyword> List_schema_keyword;

class Json_schema_type : public Json_schema_keyword
{
  public:
    enum json_value_types type;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_const : public Json_schema_keyword
{
  public:
    char *const_json_value;
    enum json_value_types type;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
    Json_schema_const()
    {
      const_json_value= NULL;
    }
};

enum enum_scalar_values {
                         HAS_NO_VAL= 0, HAS_TRUE_VAL= 2,
                         HAS_FALSE_VAL= 4, HAS_NULL_VAL= 8
                        };
class Json_schema_enum : public  Json_schema_keyword
{
  public:
    HASH enum_values;
    uint enum_scalar;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
    Json_schema_enum()
    {
      enum_scalar= HAS_NO_VAL;
    }

};

class Json_schema_maximum : public Json_schema_keyword
{
  public:
    double maximum;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_minimum : public Json_schema_keyword
{
  public:
    double minimum;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_multiple_of : public Json_schema_keyword
{
  public:
    double multiple_of;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_ex_maximum : public Json_schema_keyword
{
  public:
    double ex_max;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_ex_minimum : public Json_schema_keyword
{
  public:
    double ex_min;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_max_len : public Json_schema_keyword
{
  public:
    uint max_len;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_min_len : public Json_schema_keyword
{
  public:
    uint min_len;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_pattern : public Json_schema_keyword
{
  public:
    Regexp_processor_pcre re;
    Item *pattern;
    Item_string *str;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
    Json_schema_pattern()
    {
      str= NULL;
      pattern= NULL;
    }
    void cleanup() { re.cleanup(); }
};

class Json_schema_max_items : public Json_schema_keyword
{
  public:
    uint max_items;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_min_items : public Json_schema_keyword
{
  public:
    uint min_items;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

enum array_property_flag
{ HAS_NO_ARRAY_FLAG= 0, HAS_MAX_CONTAINS=8, HAS_MIN_CONTAINS= 16};

/*
  contains, maxContains and mincontains are all kept in same class
  instead of separate classes like other keywords. This is because
  minContains and maxContains are depenedent on value of contains
  keywords.
*/
class Json_schema_contains : public Json_schema_keyword
{
  public:
    enum json_value_types contains_type;
    uint max_contains;
    uint min_contains;
    uint contains_flag;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
    Json_schema_contains()
    {
      contains_flag= HAS_NO_ARRAY_FLAG;
      contains_type= JSON_VALUE_UNINITIALIZED;
    }
};

class Json_schema_unique_items : public Json_schema_keyword
{
  public:
    bool is_unique;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
    Json_schema_unique_items()
    {
      is_unique= false;
    }
};

/*
  prefixItems and allow_extra_items are all kept in same class
  because if result of schema validation if extra items
  are disallowed depends on the prefix items. Exception is made here,
  just like contains, minContains and maxContains.
*/
class Json_schema_items_details : public Json_schema_keyword
{
  public:
    List <List_schema_keyword> prefix_items;
    bool allow_extra_items;
    enum json_value_types item_type;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
    Json_schema_items_details()
    {
      item_type= JSON_VALUE_UNINITIALIZED;
      allow_extra_items= true;
    }
};

typedef struct property
{
  List<Json_schema_keyword> *curr_schema;
  char *key_name;
} st_property;

class Json_schema_properties : public Json_schema_keyword
{
  public:
    HASH properties;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_max_prop : public Json_schema_keyword
{
  public:
    uint max_prop;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_min_prop : public Json_schema_keyword
{
  public:
    uint min_prop;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

class Json_schema_required : public Json_schema_keyword
{
  public:
    List <String> required_properties;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

typedef struct dependent_keyowrds
{
  String *property;
  List <String> dependents;
} st_dependent_keywords;

class Json_schema_dependent_prop : public Json_schema_keyword
{
  public:
    List<st_dependent_keywords> dependent_required;
    bool validate(json_engine_t *je);
    bool handle_keyword(THD *thd, json_engine_t *je,
                        List<HASH> *hash_list,
                        const char* key_start,
                        const char* key_end,
                        List<Json_schema_keyword> *all_keywords);
};

bool create_object_and_handle_keyword(THD *thd, json_engine_t *je,
                                      List<Json_schema_keyword> *keyword_list,
                                      List<HASH> *hash_list,
                                      List<Json_schema_keyword> *all_keywords);
uchar* get_key_name_for_property(const char *key_name, size_t *length,
                    my_bool /* unused */);

#endif
