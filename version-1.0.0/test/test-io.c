/**
 * @file test-io.c
 * @author yezi (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/sendfile.h>
#include <signal.h>

#include <sys/wait.h>

#include "kvdb-sdk-api.h"

char g_username[128] = "admin";
char g_password[128] = "password";
char g_serverip[32] = "127.0.0.1";
unsigned short g_serverport = 666;
uint64_t g_record_count = 100000;

static struct option long_options[] = {
    {"host", required_argument, NULL, 'h'},
    {"port", required_argument, NULL, 'p'},
    {"user", required_argument, NULL, 'u'},
    {"password", required_argument, NULL, 'P'},
    {"number", required_argument, NULL, 'n'},
    {"help", no_argument, NULL, 'H'},
    {"version", no_argument, NULL, 'v'},
    {0, 0, 0, 0}};

uint64_t g_release_version = 10000;
char g_version[256] = "test-io version 1.0.0";
static void SignalShield();
static int args_process(const int argc, char **argv, struct option *long_options);

int main(int argc, char *argv[])
{
    int err = -1;
    FILE *fp = NULL;
    kvdb_t *kvdb = NULL;
    char key[256] = {0};
    char buff[4096] = {0};
    char data[4096] = {0};
    const int datalen = 256;
    time_t m_time = 0;
    time_t eplapsed = 0;
    uint64_t id = 0;
    time_t b_time_0 = 0;
    time_t eplapsed_0 = 0;
    time_t b_time_1 = 0;
    time_t eplapsed_1 = 0;

    SignalShield();
    args_process(argc, argv, long_options);

    fp = fopen("log.txt", "wb");
    if(fp == NULL)
    {
        fprintf(stderr, "|%s:%d|open log.txt failed.\n", __FILE__, __LINE__);
        return 1;
    }

    kvdb = kvdb_sdk_init();
    if(kvdb == NULL)
    {
        fprintf(stderr, "|%s:%d| kvdb sdk init failed.\n", __FILE__, __LINE__);
        fprintf(fp, "|%s:%d| kvdb sdk init failed.", __FILE__, __LINE__);
        fflush(fp);
        return 1;
    }

    err = kvdb_sdk_connect(kvdb, g_username, g_password, g_serverip, g_serverport);
    if(err != 0)
    {
        fprintf(stderr, "|%s:%d|connect to kvdb:[%s:%d] failed. status:(%s, err:%d)\n", __FILE__, __LINE__, g_serverip, g_serverport, kvdb_strerror(err), err);
        fprintf(fp, "|%s:%d|connect to kvdb:[%s:%d] failed. status:(%s, err:%d)", __FILE__, __LINE__, g_serverip, g_serverport, kvdb_strerror(err), err);
        fflush(fp);
        return 1;
    }

    fprintf(stderr, "|%s:%d|total test write/read key-value count: %ld\n", __FILE__, __LINE__, g_record_count);
    fprintf(fp, "|%s:%d|total test write/read key-value count: %ld\n", __FILE__, __LINE__, g_record_count);
    fflush(fp);

    memset(data, 0x00, sizeof(data));
    memset(data, '-', datalen);
    
    b_time_0 = time(NULL);
    m_time = b_time_0;
    for(id = 0; id < g_record_count; ++id)
    {
        memset(key, 0x00, sizeof(key));
        snprintf(key, sizeof(key), "key-%lu", id);
        snprintf(data + datalen, sizeof(data) - datalen, "data-%lu", id);
        err = kvdb_sdk_insert(kvdb, key, strlen(key), data, strlen(data));
        if(err != 0)
        {
            fprintf(stderr, "|%s:%d|insert [%s] failed. status:(%s, err:%d)\n", __FILE__, __LINE__, key, kvdb_strerror(err), err);
            fprintf(fp, "|%s:%d|insert:[\"%s\": \"%s\"] to kvdb:[%s:%d] failed. status:(%s, err:%d)\n", __FILE__, __LINE__, key, data, g_serverip, g_serverport, kvdb_strerror(err), err);
            fflush(fp);
            return 1;
        }
        if(m_time + 3600 <= time(NULL))
        {
            eplapsed = time(NULL) - b_time_0;
            m_time = time(NULL);
            fprintf(fp, "|%s:%d|mid time insert eplapsed:%lds avg:%lu/s\n", __FILE__, __LINE__, eplapsed, id / eplapsed);
            fflush(fp);
        }
    }
    eplapsed_0 = (time(NULL) - b_time_0) ?  time(NULL) - b_time_0 : 1;
    fprintf(stderr, "|%s:%d|insert eplapsed:%lds avg: %ld/s\n", __FILE__, __LINE__, eplapsed_0, g_record_count / eplapsed_0);
    fprintf(fp, "|%s:%d|insert eplapsed:%lds avg: %ld/s\n", __FILE__, __LINE__, eplapsed_0, g_record_count / eplapsed_0);
    fflush(fp);

    b_time_1 = time(NULL);
    m_time = b_time_1;
    for(id = 0; id < g_record_count; ++id)
    {
        memset(key, 0x00, sizeof(key));
        memset(buff, 0x00, sizeof(buff));
        snprintf(key, sizeof(key), "key-%lu", id);
        snprintf(data + datalen, sizeof(data) - datalen, "data-%lu", id);
        err = kvdb_sdk_read(kvdb, key, strlen(key), buff, sizeof(buff));
        if(err != 0 || strcmp(buff, data) != 0)
        {
            fprintf(stderr, "|%s:%d|read [%s] failed. status:(%s, err:%d)\n", __FILE__, __LINE__, key, kvdb_strerror(err), err);
            fprintf(fp, "|%s:%d|read:[\"%s\":%s, %s] from kvdb:[%s:%d] failed. status:(%s, err:%d)\n", __FILE__, __LINE__, key, buff, data, g_serverip, g_serverport, kvdb_strerror(err), err);
            fflush(fp);
            return 1;
        }
        if(m_time + 3600 <= time(NULL))
        {
            eplapsed = time(NULL) - b_time_1;
            m_time = time(NULL);
            fprintf(fp, "|%s:%d|mid time read eplapsed:%lds avg:%lu/s\n", __FILE__, __LINE__, eplapsed, id / eplapsed);
            fflush(fp);
        }
    }
    eplapsed_1 = (time(NULL) - b_time_1) ?  time(NULL) - b_time_1 : 1;
    fprintf(stderr, "|%s:%d|read eplapsed:%lds avg: %ld/s\n", __FILE__, __LINE__, eplapsed_1, g_record_count / eplapsed_1);
    fprintf(fp, "|%s:%d|read eplapsed:%lds avg: %ld/s\n", __FILE__, __LINE__, eplapsed_1, g_record_count / eplapsed_1);
    fflush(fp);

    kvdb_sdk_release(kvdb);
    fclose(fp);
    return 0;
}

static void SignalShield()
{
    sigset_t signal_mask = {0};
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGPIPE);
    sigaddset(&signal_mask, SIGQUIT);
    sigaddset(&signal_mask, SIGUSR1);
    if (pthread_sigmask(SIG_BLOCK, &signal_mask, NULL) != 0)
    {
        fprintf(stderr, "block sigpipe error\n");
        exit(1);
    }
}

static void usage()
{
    fprintf(stderr, "usage: command args ...\n");
    fprintf(stderr, "\t --host:         -h    host ip must be like: (--host=127.0.0.1 or --port 127.0.0.1 default: 127.0.0.1).\n");
    fprintf(stderr, "\t --port:         -p    host port must be like: (--port=666 or -p 666 or default: 666).\n");
    fprintf(stderr, "\t --user:         -u    user name must be like: (--user=admin or --user admin default: admin).\n");
    fprintf(stderr, "\t --password:     -P    password must be like: (--password=password or --password password default: password).\n");
    fprintf(stderr, "\t --number:       -n    number must be like: (--number=100000 or --number 100000 default: 100000).\n");
    fprintf(stderr, "\t --help:         -H    usage help\n");
    fprintf(stderr, "\t --version:      -v    version information\n");
    exit(0);
}

static int args_process(const int argc, char **argv, struct option *long_options)
{
    int c = 0;
    int long_index = 0;

    while ((c = getopt_long(argc, argv, "h:p:u:P:H::v::", long_options, &long_index)) != -1)
    {
        switch (c)
        {
        case 'h':
        {
            strcpy(g_serverip, optarg);
            break;
        }
        case 'p':
        {
            g_serverport = atoi(optarg);
            break;
        }
        case 'u':
        {
            strcpy(g_username, optarg);
            break;
        }
        case 'P':
        {
            strcpy(g_password, optarg);
            break;
        }
        case 'n':
        {
            g_record_count = atoll(optarg);;
            break;
        }
        case 'H':
        {
            usage();
            exit(0);
            break;
        }
        case 'v':
        {
            fprintf(stderr, "%s: %lu\n", g_version, g_release_version);
            exit(0);
            break;
        }
        default:
        {
            fprintf(stderr, "bad opt: %d\n", c);
            usage();
            exit(0);
            break;
        }
        }
    }
    optind = 0;
    return 0;
}

