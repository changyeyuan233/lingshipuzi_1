#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *thread_worker1(void *args);
void *thread_worker2(void *args);
typedef struct flow_ctx_s
{
	int                	shared_var;
	pthread_mutex_t    	lock;
}flow_ctx_t;



int main(int argc, char *argv[])
{
	worker_ctx_t             worker_ctx;
	pthread_t                tid;
	pthread_attr_t           thread_attr;
	worker_ctx_t.shared_var = 10000;


	pthread_mutex_init(&worker_ctx.lock,null);

	if (pthread_attr_init(&thread_attr))
        {
            	printf("%s !init!: %s\n", __FUNCTION__,strerror(errno));
       		return -1;
   	}
	if(pthread_attr_setstacksize(&thread_attr,120*1024))
	{
		printf("%s!setstacksize!: %s\n",__FUNCTION__,strerror(errno));
		return -2;
	}
	if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED))
        {
            printf("%s !setdetachstate!: %s\n",strerror(errno));
	    return -3; 
	}
	pthread_create(&tid,&thread_attr,thread_worker1,&worker_ctx);
	printf("Thread flower1 tid[%ld] created ok\n", tid);
	pthread_create(&tid,&thread_attr,thread_worker2,&worker_ctx);
	printf("Thread flower2 tid[%ld] created ok\n", tid);

	while(1)
	{
		printf("main/control thread shared_var: %d\n,worker_ctx.shared_var);
		sleep(10);
	}
	pthread_mutex_destory(&worker_ctx.lock);
}
void *thread_worker1(void *args)
{
	worker_ctx_t *ctx = (worker_ctx_t *)args;
	if (!args)
       	{
		printf("%s() get invalid arguments\n", __FUNCTION__);
		pthread_exit(NULL);
	}
	printf(" flower 1 [%ld] start running...\n", pthread_self());
	while (1)
       	{
		pthread_mutex_lock(&ctx->lock); //设置阻塞锁
		printf("+++: %s before shared_var++: %d\n", __FUNCTION__, ctx->shared_var);
		ctx->shared_var ++;
		sleep(2);
		printf("+++: %s after sleep shared_var: %d\n", __FUNCTION__, ctx->shared_var);
		pthread_mutex_unlock(&ctx->lock);
		sleep(1);
	}
	printf("flower 1 exit...\n");
	return NULL;
}
void *thread_worker2(void *args)
{
	worker_ctx_t *ctx = (worker_ctx_t *)args;
	if (!args) 
	{
		printf("%s() get invalid arguments\n", __FUNCTION__);
		pthread_exit(NULL);
	}
	printf("Thread flower 2 [%ld] start running...\n", pthread_self());
	while(1)
	{
		if (0 != pthread_mutex_trylock(&ctx->lock)) 
		{
			continue;
		}
		printf("---: %s before shared_var++: %d\n", __FUNCTION__, ctx->shared_var);
        	ctx->shared_var ++;
        	sleep(2);
    		printf("---: %s after sleep shared_var: %d\n", __FUNCTION__, ctx->shared_var);
	       	pthread_mutex_unlock(&ctx->lock);
	       	sleep(1);
     	}
    	printf("flower 2 exit...\n");
    	return NULL;
 }

