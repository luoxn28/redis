/**
 * redis-balance
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "../../deps/hiredis/hiredis.h"
#include "../sds.h"
#include "../adlist.h"
#include "../ae.h"
#include "balance.h"

static struct redisBalance config;
static redisContext *context;

static void configInit(void)
{
    config.pid = getpid();
    config.hostip = sdsnew("127.0.0.1");
    config.hostport = 6379;
    config.serverport = 8080;

    config.eventloop = aeCreateEventLoop(64);

    //config.clients = listCreate();
}

static void configFinish(void)
{
    sdsfree(config.hostip);
    aeDeleteEventLoop(config.eventloop);
}

static void readEvent(aeEventLoop *el, int fd, void *privdata, int mask)
{
    printf("hi, readEvent...\n");
}

static void beforeSleep(struct aeEventLoop *eventLoop)
{
    printf("hi, beforeSleep...\n");
}

static void balancePrintContextError(void) {
    if (context == NULL) return;
    fprintf(stderr,"Error: %s\n",context->errstr);
}


static int balanceReadReply(int output_raw_strings)
{
    void *_reply;
    redisReply *reply;
    sds out = NULL;
    int output = 1;

    if (redisGetReply(context, &_reply) != REDIS_OK) {
        balancePrintContextError();
        exit(1);
        return REDIS_ERR; /* avoid compiler warning */
    }
    
    reply = (redisReply*)_reply;
    
    if (output) {
        printf("%s\n", reply->str);
    }
    //freeReplyObject(reply);
    return REDIS_OK;
}

static int balanceSendCommand(int argc, char **argv, int repeat)
{
    size_t *argvlen;
    int j, output_raw;

    argvlen = (size_t *) zmalloc(argc * sizeof(size_t));
    for (j = 0; j < argc; j++) {
        argvlen[j] = sdslen(argv[j]);
    }

    output_raw = 0;
    while(repeat--) {
        redisAppendCommandArgv(context, argc,(const char**)argv, argvlen);
    
        if (balanceReadReply(output_raw) != REDIS_OK) {
            zfree(argvlen);
            return REDIS_ERR;
        } else {
            // ...
        }
    }

    zfree(argvlen);
    return REDIS_OK;
}
/**
 * redis-balance [server-ip] [server-port]
 */
int main(int argc, char **argv)
{
    configInit();

    /* parse argv configuration */
    if (argc == 4) {
        sdsfree(config.hostip);
        config.hostip = sdsnew(argv[1]);
        config.hostport = atoi(argv[2]);
        config.serverport = atoi(argv[3]);
    } else {
        fprintf(stderr,"Usage: redis-balance <master-ip> <master-port> <server-ip>\n");
        return -1;
    }

    /* connection redis master server */
    context = redisConnect(config.hostip,config.hostport);
    if (context->err) {
        fprintf(stderr,"Could not connect to Redis at ");
        fprintf(stderr,"%s:%d: %s\n",config.hostip,config.hostport,context->errstr);
        redisFree(context);
        context = NULL;
        return REDIS_ERR;
    }

    int num = 2;
    sds array[2];
    array[0] = sdsnew("get");
    array[1] = sdsnew("name");
    if (balanceSendCommand(num, array, 1) != REDIS_OK) {
        fprintf(stderr,"Send message error");
    }

    /* add event */
    if (aeCreateFileEvent(config.eventloop, context->fd, AE_READABLE,
         readEvent, NULL) == AE_ERR)
    {
        redisFree(context);
        return -1;
    }

    aeSetBeforeSleepProc(config.eventloop, beforeSleep);
    aeMain(config.eventloop);
    
    redisFree(context);
    configFinish();
    return 0;
}

/*------------------- util function --------------------------------*/


