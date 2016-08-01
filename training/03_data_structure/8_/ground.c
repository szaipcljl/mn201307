#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

char data[4] = "abcd";

int a[4][4] ={
	1,1,0,1,
	1,1,1,1,
	0,1,1,0,
	1,1,0,1
};

int get_frist_adj(int (*a)[4],int v)
{
	int i;

	for(i = 0;i < 4;i ++){
		if(1 == a[v][i]){
			return i;
		}
	}
	return -1;
}

int get_next_adj(int (*a)[4],int v,int u) //v是初始节点 上一次找到的相邻点
{
	int i;

	for(i = u + 1;i < 4;i ++){
		if(1 == a[v][i]){
			return i;
		}
	}
	return -1;
}

int visit[4] = {0};

void deep(int (*a)[4],char *data,int v)
{
	int u;

	if(1 == visit[v])
		return ;

	printf(" %c ",data[v]); //如果自己没有被访问过 打印自己并标记
	visit[v] = 1;

	u = get_frist_adj(a,v); //获取第一个邻接点

	while(-1 != u){ // 如果邻接点存在 以这个邻接点 为入口遍历整个图
		deep(a,data,u);
		u = get_next_adj(a,v,u);
	}

	return ;
}


void ground(int (*a)[4],char *data,int v)
{
	int queue[20];
	int front = 0;
	int rear = 0;
	int u;

	int visit[4] = {0};


	queue[rear ++] = v;
	visit[v] = 1;

	printf("ground :");
	while(front !=  rear){
		u = get_frist_adj(a,queue[front]);

		while( -1 != u ){ //如果邻接点存在 继续循环

			if(1 != visit[u]){ //如果找到节点没有入队过 则入队
				queue[rear ++] = u;
				visit[u] = 1;
			}
			u = get_next_adj(a,queue[front],u);
		}
		printf(" %c ",data[queue[front ++]]); //邻接点都入队后 ，出队打印自己

	}

	printf("\n");

	return ;
}

int main(int argc, const char *argv[])
{

	bzero(visit,sizeof(visit));
	deep(a,data,0);
	printf("\n");

	ground(a,data,1);
	return 0;
}
