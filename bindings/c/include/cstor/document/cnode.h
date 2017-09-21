//
// Created by efelici on 8/30/2017.
//

#ifndef CSTOR_CSTOR_NODE_H
#define CSTOR_CSTOR_NODE_H

#include <stdbool.h>
#include <stddef.h>
#include <cstor/errors/errors.h>
#include <cstor/utility/export.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct esft_stor_node_t esft_stor_node_t;

typedef struct esft_stor_json_t {
    size_t len; /** in case json contains unescaped \0 **/
    char * value;
} esft_stor_json_t;

/**
 * @brief checks whether underlying value of the node is a number
 * (either int, long or double)
 *
 * @return true if number, false if not
 */
CSTOR_API bool esft_stor_node_is_num(esft_stor_node_t *node);

/**
 * @brief checks whether underlying value of the node is an int
 *
 * @return true if int, false if not
 */
CSTOR_API bool esft_stor_node_is_int(esft_stor_node_t *node);

/**
 * @brief returns the int representation of the node.
 * The @p node must be a value node, such that
 * esft_stor_node_is_int(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 * 
 * 
 * @return int value of node
 */
CSTOR_API int esft_stor_node_as_int(esft_stor_node_t *node);

/**
 * @brief checks whether underlying value of the node is a long
 *
 * @return true if long, false if not
 */
CSTOR_API bool esft_stor_node_is_long(esft_stor_node_t *node);

/**
 * @brief returns the long representation of the node.
 * The @p node must be a value node, such that
 * esft_stor_node_is_long(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 * 
 * 
 * @return long value of node
 */
CSTOR_API long long esft_stor_node_as_long(esft_stor_node_t *node);

/**
 * @brief checks whether underlying value of the node is a double
 *
 * @return true if double, false if not
 */
CSTOR_API bool esft_stor_node_is_double(esft_stor_node_t *node);

/**
 * @brief returns the double representation of the node.
 * The @p node must be a value node, such that
 * esft_stor_node_is_double(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 * 
 * 
 * @return double value of node
 */
CSTOR_API double esft_stor_node_as_double(esft_stor_node_t *node);

/**
 * @brief checks whether underlying value of the node is a bool
 *
 * @return true if bool, false if not
 */
CSTOR_API bool esft_stor_node_is_bool(esft_stor_node_t *node);

/**
 * @brief returns the bool representation of the node.
 * The @p node must be a value node, such that
 * esft_stor_node_is_bool(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 * 
 * 
 * @return bool value of node
 */
CSTOR_API bool esft_stor_node_as_bool(esft_stor_node_t *node);

/**
 * @brief checks whether underlying value of the node is a string
 *
 * @return true if string, false if not
 */
CSTOR_API bool esft_stor_node_is_string(esft_stor_node_t *node);


/**
 * @brief returns the C string representation of the node.
 *
 * The @p node must be a value node, such that
 * esft_stor_node_is_string(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 *
 * TRANSFER OWNERSHIP ON SUCCESS: NO
 *
 * @return string value of node
 */
CSTOR_API const char *esft_stor_node_as_string(esft_stor_node_t *node);


/**
 * @brief returns the string representation of the node and sets
 * @p str_len to the actual length of the string.
 *
 * The actual length refers to the entire string, which may be exceed the first
 * \0 character encountered. Having this length returned is necessary,
 * as JSON can contain \0 (escaped as \u0000, different from \\0 required by C),
 * according to RFC 4627.
 * If you know that the returned string will be a normal C string, then
 * just invoke esft_stor_node_as_string
 *
 * The @p node must be a value node, such that
 * esft_stor_node_is_string(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 * 
 *
 * TRANSFER OWNERSHIP ON SUCCESS: NO
 *
 * @return string value of node
 */
CSTOR_API const char *esft_stor_node_as_stringl(esft_stor_node_t *node, size_t *str_len);

/**
 * @brief checks whether underlying value of the node is a null object
 *
 * @return true if null, false if not
 */
CSTOR_API bool esft_stor_node_is_null(esft_stor_node_t *node);

/**
 * @brief checks whether underlying value of the node is an object
 *
 * @return true if object, false if not
 */
CSTOR_API bool esft_stor_node_is_object(esft_stor_node_t *node);

/**
 * @brief returns the node mapped to key @p key in the object node @p node.
 * If there isn't such node, returns NULL
 *
 * The @p node must be an object node, such that
 * esft_stor_node_is_object(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 *
 * TRANSFER OWNERSHIP ON SUCCESS: YES
 * RESOURCE RELEASE FUNCTION: esft_stor_node_delete
 *
 * @return node at key @p key or NULL
 */
CSTOR_API esft_stor_node_t *esft_stor_node_object_get(esft_stor_node_t *node,
                                            const char *key,
                                            esft_stor_error_t *err);

/**
 * @brief checks whether object node @p node has a mamber mapped to 
 * key @p key.
 *
 * The @p node must be an object node, such that
 * esft_stor_node_is_object(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 * 
 *
 * @return true if object contains member with key @p key, false if not
 */
CSTOR_API bool esft_stor_node_object_has(esft_stor_node_t *node,
                               const char *key);

/**
 * @brief Returns newly allocated array of strings to hold the keys for object node @p node
 * and sets the number of keys in @p out_len.
 *
 * If the operation fails, a value of 0 is set into @p out_len and NULL is returned
 *
 * TRANSFER OWNERSHIP ON SUCCESS: YES
 * RESOURCE RELEASE FUNCTION: esft_stor_node_object_keys_delete
 *
 * @returns array of string representing an object node members' keys.
 */
CSTOR_API char ** esft_stor_node_object_keys(esft_stor_node_t *node,
                                                 size_t *out_len);

/**
 * @brief deletes the @p len strings contained in keys and
 * then deletes the array itself
 */
CSTOR_API void esft_stor_node_object_keys_delete(char **keys, size_t len);

/**
 * @brief checks whether underlying value of the node is an array
 *
 * @return true if array, false if not
 */
CSTOR_API bool esft_stor_node_is_array(esft_stor_node_t *node);

/**
 * @brief returns the size of the array node @p node.
 *
 * The @p node must be an array node, such that
 * esft_stor_node_is_array(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 * 
 */
CSTOR_API size_t esft_stor_node_size(esft_stor_node_t *node);

/**
 * @brief returns the node at index @p index in the node array @p node.
 * Returns NULL if index is out of range.
 *
 * The @p node must be an array node, such that
 * esft_stor_node_is_array(node *) == true. Calling this
 * function otherwise results in undefined behaviour
 *
 * TRANSFER OWNERSHIP ON SUCCESS: YES
 * RESOURCE RELEASE FUNCTION: esft_stor_node_delete
 *
 * @return node at key @p key or NULL
 */
CSTOR_API esft_stor_node_t *esft_stor_node_array_get(esft_stor_node_t *node,
                                           unsigned int index,
                                           esft_stor_error_t *err);

/**
 * @brief returns the JSON representation of the node @p node
 * If operation fails, returns an empty json object (len == 0).
 *
 * @return JSON representation of node
 */
CSTOR_API esft_stor_json_t esft_stor_node_json(esft_stor_node_t *node);

/**
 * @brief sets this node @p node to the node parsed from the the JSON @p json.
 * This is analogous to deep copying the node resulting from parsing the JSON
 * into the node @p node.
 * If operation fails, sets error accordingly
 *
 */
CSTOR_API void esft_stor_node_from_json(esft_stor_node_t *node,
                              const char *json,
                              esft_stor_error_t *err);

/**
 * @brief sets the node @p node to the node @p copy.
 * This is analogous to deep copying the node @ copy
 * into the node @p node.
 * If operation fails, sets error accordingly
 *
 */
CSTOR_API void esft_stor_node_from_node(esft_stor_node_t *node,
                              esft_stor_node_t *copy,
                              esft_stor_error_t *err);

/**
 * @brief disposes of a json object
 */
CSTOR_API void esft_stor_json_dispose(esft_stor_json_t *json);


/**
 * @brief adds an empty object into object node @p node with key @p key
 */
CSTOR_API void esft_stor_node_object_put_obj(esft_stor_node_t *node,
                                   const char *key);

/**
 * @brief adds an empty array into object node @p node with key @p key
 */
CSTOR_API void  esft_stor_node_object_put_array(esft_stor_node_t *node,
                                      const char *key);

/**
 * @brief puts a <key:value> pair into object node @p node,
 * with key @p key and value @p value of type int
 */
CSTOR_API void esft_stor_node_object_put_int(esft_stor_node_t *node,
                                   const char *key, 
                                   int value);

/**
 * @brief puts a <key:value> pair into object node @p node,
 * with key @p key and value @p value of type long
 */
CSTOR_API void esft_stor_node_object_put_long(esft_stor_node_t *node,
                                   const char *key,
                                   long long value);

/**
 * @brief puts a <key:value> pair into object node @p node,
 * with key @p key and value @p value of type double
 */
CSTOR_API void esft_stor_node_object_put_double(esft_stor_node_t *node,
                                   const char *key,
                                   double value);

/**
 * @brief puts a <key:value> pair into object node @p node,
 * with key @p key and value @p value of type bool
 */
CSTOR_API void esft_stor_node_object_put_bool(esft_stor_node_t *node,
                                   const char *key,
                                   bool value);

/**
 * @brief puts a <key:value> pair into object node @p node,
 * with key @p key and value @p value of type const char *
 */
CSTOR_API void esft_stor_node_object_put_string(esft_stor_node_t *node,
                                   const char *key,
                                   const char * value);

/**
 * @brief adds an empty object into array node @p node.
 */
CSTOR_API void esft_stor_node_array_add_obj(esft_stor_node_t *node);


/**
 * @brief adds an empty array into node array @p node.
 */
CSTOR_API void esft_stor_node_array_add_array(esft_stor_node_t *node);


/**
 * @brief adds int value @p value into array node @p node,
 */
CSTOR_API void esft_stor_node_array_add_int(esft_stor_node_t *node,
                                   int value);

/**
 * @brief adds long value @p value into array node @p node,
 */
CSTOR_API void esft_stor_node_array_add_long(esft_stor_node_t *node,
                                    long long value);

/**
 * @brief adds double value @p value into array node @p node,
*/
CSTOR_API void esft_stor_node_array_add_double(esft_stor_node_t *node,
                                      double value);

/**
 * @brief adds bool value @p value into array node @p node,
*/
CSTOR_API void esft_stor_node_array_add_bool(esft_stor_node_t *node,
                                    bool value);

/**
 * @brief adds string value @p value into array node @p node,
*/
CSTOR_API void esft_stor_node_array_add_string(esft_stor_node_t *node,
                                      const char * value);

/**
 * @brief deletes the object esft_stor_node_t
 */
CSTOR_API void esft_stor_node_delete(esft_stor_node_t *node);

#ifdef __cplusplus
}
#endif

#endif //CSTOR_CSTOR_NODE_H
