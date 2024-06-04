/**
 * @file kvdb-sdk-api.h
 * @author yezi (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-29
 * 
 * @Copyright (c) 2024.01.01-2025.01.01
 * 
 */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __KVDB_SDK_API_H__
#define __KVDB_SDK_API_H__

/**
 * the kvdb sdk handle
*/
typedef struct kvdb__ kvdb_t;

/**
 * the struct for kvdb_sdk_list_key
*/
typedef struct kvdb_key__
{
    int64_t key_len; /**the key length**/
    char *key;       /**the key buffer**/
}kvdb_key_t;

/**
 * the struct for kvdb_sdk_list_users
*/
typedef struct kvdb_user__
{
    char *username; /**the user's name buffer**/
}kvdb_user_t;

/**
 * kvdb_strerror - get error code description string
 * @kvdb_errno: the sdk api return code
 * @return:
 *      error code description string.
*/
extern const char *kvdb_strerror(const int kvdb_errno);

/**
 * kvdb_sdk_init - initialize the sdk must be call the first
 * @return:
 *      On success, kbdb handle is returned. On error, NULL is returned.
*/
kvdb_t *kvdb_sdk_init(void);

/**
 * kvdb_sdk_release - release the sdk must be call the last
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @return:
*/
void kvdb_sdk_release(kvdb_t *kvdb);
/**
 * kvdb_sdk_connect - connect to kvdb database server
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @username: The kvdb database user name.
 * @password: The kvdb database user's password.
 * @ip: The kvdb database server's ip address.
 * @port: The kvdb database server's port.
 * @return:
 *         On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_connect(kvdb_t *kvdb, const char *username, const char *password, const char *ip, const unsigned short port);

/**
 * kvdb_sdk_release - the heartbeat between kvdb sdk and kvdb database
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @return:
 *         On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_heartbeat(kvdb_t *kvdb);

/**
 * kvdb_sdk_add_user - add user to kvdb database server
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @user: The kvdb database user name.
 * @password: The kvdb database user's password.
 * @return:
 *         On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_add_user(kvdb_t *kvdb, const char *user, const char *password);

/**
 * kvdb_sdk_del_user - delete user to kvdb database server
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @user: The kvdb database user name.
 * @password: The kvdb database user's password.
 * @return:
 *         On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_del_user(kvdb_t *kvdb, const char *user, const char *password);

/**
 * kvdb_sdk_list_users - list user to kvdb database server only user admin has permission
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @users: The kvdb database user name list. the sdk will alloc and free the pointer.
 * Eg.
 *      int err = -1;
 *      int num = 0;
 *      kvdb_user_t *users = NULL;
 *      err = kvdb_sdk_list_users(kvdb, &users, &num);
 *      if(err != 0)
 *      {
 *          fprintf(stderr, "list-user failed. status:(%s, err:%d)\n", kvdb_strerror(err), err);
 *      }
 * 
 * @num: The kvdb database user name list number.
 * @return:
 *         On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_list_users(kvdb_t *kvdb, kvdb_user_t **users, int *num);

/**
 * kvdb_sdk_set_password - reset user's password to kvdb database server
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @user: The kvdb database user name.
 * @oldpassword: The kvdb database user's old password.
 * @oldpassword: The kvdb database user's new password.
 * @return:
 *         On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_set_password(kvdb_t *kvdb, const char *user, const char *oldpassword, const char *newpassword);

/**
 * kvdb_sdk_insert - insert key-vale to kvdb database server
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @user: The kvdb database user name.
 * @key: The key buffer of the insert key.
 * @key_len: The key length
 * @data: The value buffer of the insert data.
 * @data_len: The buffer length
 * @return:
 *         On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_insert(kvdb_t *kvdb, const void *key, const int key_len, const void *data, const int data_len);

/**
 * kvdb_sdk_update - update key-vale to kvdb database server if the key-value not existed it do the kvdb_sdk_insert
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @user: The kvdb database user name.
 * @key: The key buffer of the update key.
 * @key_len: The key length
 * @data: The value buffer of the update data.
 * @data_len: The buffer length
 * @return:
 *         On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_update(kvdb_t *kvdb, const void *key, const int key_len, const void *data, const int data_len);

/**
 * kvdb_sdk_read - read key-vale from kvdb database server
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @user: The kvdb database user name.
 * @key: The key buffer of the read data.
 * @key_len: The key length
 * @buff: The value buffer of the read data.
 * @buflen: The buffer length
 * @return:
 *         On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_read(kvdb_t *kvdb, const void *key, const int key_len, void *buff, const int buflen);

/**
 * kvdb_sdk_delete - delete key-value from the kvdb database
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @key: The key buffer of the delete data.
 * @key_len: The key length
 * @return:
 *      On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_delete(kvdb_t *kvdb, const void *key, const int key_len);

/**
 * kvdb_sdk_insert_file - insert file to the kvdb database
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @key: The key buffer of the data upload data.
 * @key_len: The key length
 * @fname: The insert file name, Eg. key:/home/test.txt fname:/home/test.txt 
 * @return:
 *      On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_insert_file(kvdb_t *kvdb, const void *key, const int key_len, const char *fname);

/**
 * kvdb_sdk_insert_file - rad file from the kvdb database
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @key: The key buffer of the data upload data.
 * @key_len: The key length
 * @fname: The insert file name, Eg. key:/home/test.txt fname:/home/test.txt 
 * @return:
 *      On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_read_file(kvdb_t *kvdb, const void *key, const int key_len, const char *fname);

/**
 * the keys handle for list keys from the kvdb database
*/
typedef void * kvdb_key_handle_t;
/**
 * kvdb_sdk_list_key_start - initialize the keys handle from the kvdb database
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @h__: The key list handle. success return by:kvdb_sdk_list_key_start.
 * @return:
 *      On success, keys handle is returned. On error, NULL is returned.
*/
kvdb_key_handle_t kvdb_sdk_list_key_start(kvdb_t *kvdb);

/**
 * kvdb_sdk_list_key - list keys from the kvdb database
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @h__: The key list handle. success return by:kvdb_sdk_list_key_start.
 * @key_list: The keys list. the sdk will alloc and free the pointer.
 * @key_num: The key num [in]: max list num want to list. [out]: success get keys num
 * Eg.
 *  ......
 *      int i = 0;
 *      int err = -1;
 *      int key_num = 0;
 *      int64_t total_num = 0;
 *      kvdb_key_t *key_list = NULL;
 *      do
 *      {
 *          key_num = 100;
 *          err = kvdb_sdk_list_key(kvdb, kvdb_key, &key_list, &key_num);
 *          if (err < 0)
 *          {
 *              fprintf(stderr, "|%s:%d|list kvdb key failure. err: %d\n", __FILE__, __LINE__, err);
 *              break;
 *          }
 *          total_num += key_num;
 *          for(i = 0; i < key_num; ++i)
 *          {
 *              fprintf(stderr, "|%s:%d|list kvdb key_list[%d].key_len: %ld key_list[%d].key: %s\n", __FILE__, __LINE__, i, key_list[i].key_len, i, key_list[i].key);
 *          }
 *      } while (0 < key_num);
 *  ......
 * @return:
 *      On success, 0 is returned. On error, error code is returned.
*/
int kvdb_sdk_list_key(kvdb_t *kvdb, kvdb_key_handle_t h__, kvdb_key_t **key_list, int *key_num);

/**
 * kvdb_sdk_list_key_completed - release the keys handle
 * @kvdb: The kvdb database client handle. success return by: kvdb_sdk_init.
 * @h__: The key list handle. success return by:kvdb_sdk_list_key_start.
 * @return:
 *
*/
void kvdb_sdk_list_key_completed(kvdb_t *kvdb, kvdb_key_handle_t h__);

#ifdef __cplusplus
}
#endif

#endif /** __KVDB_SDK_API_H__ **/

