/**
 * redis-balance
 */

struct redisBalance
{
    /* General */
    pid_t pid;                  /* Main process pid. */
    char *hostip;               /* master server ip */
    int   hostport;            /* master server port */
    int   serverport;          /* balance server port */

    aeEventLoop *eventloop;
    
    list *clients;              /* List of active clients */
};

