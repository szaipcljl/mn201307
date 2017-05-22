/*插入排序,GDB调试测试代码 */
#include <stdio.h>

int x[10],//存放输入数据
	y[10],//工作空间数组
	num_inputs,//输入数组长度
	num_y = 0;//y中当前元素个数

//功能:从参数中提取输入数据
void get_args(int ac,char **av) 
{
	int i;
	num_inputs = ac - 1;
	for(i = 0;i < num_inputs;++i)
		x[i] = atoi(av[i + 1]);
}

//功能:向右移动工作数组中索引jj及其后面的所有元素
void scoot_over(int jj)
{
	int k;
	for(k = num_y;k > jj;++k)
		y[k] = y[k - 1];
}

//功能:插入一个数据
void insert(int new_y)
{
	int j; //工作数组为空
	if(0 == num_y){
		y[0] = new_y;
		return;
	}

	//将新元素插入到适当位置:第一个比new_y大的元素前面
	for(j = 0;j < num_y;++j){
		if(new_y < y[j]){
			//向右移动y[j]后面的所有元素
			scoot_over(j);
			y[j] = new_y;
			return;
		}
	}
}

//功能:处理所有输入数据
void process_data()
{
	for(num_y = 0;num_y < num_inputs;++num_y)
		insert(x[num_y]);
}

//打印结果数据
void print_results()
{
	int i;
	for(i = 0;i < num_inputs;++i)
		printf("%d\n",y[i]);
}

int main(int argc,char **argv)
{
	get_args(argc,argv);
	process_data();
	print_results();
}
