#include<stdio.h>
#include "stack.h"


int weight(int option)
{
	switch(option){
	case '+':
	case '-':
		return 1;
	case '*':
	case '/':
		return 2;
	case '(':
	case ')':
		return 0;
	default:
		return -1;
	}

	return -1;
}

int computer_once(mstack_t *val_stack,mstack_t *opt_stack)
{
	int a,b;
	int opt;  //a opt b

	if(0 !=  pop_stack(opt_stack,&opt) )
		return -1;
	pop_stack(val_stack,&b);
	pop_stack(val_stack,&a);
	printf("%d %c %d = ",a,opt,b);

	switch(opt){
	case '+':
		a += b;
		break;
	case '-':
		a -= b;
		break;
	case '*':
		a *= b;
		break;
	case '/':
		a /= b;
		break;
	}

	push_stack(val_stack,&a);

	printf("%d\n",a);

	return 0;
}

int computer(char *str)
{
	mstack_t *val_stack;//存储数值
	mstack_t *opt_stack;//存储运算符
	int sum = 0;
	data_t buf_opt;//取栈顶元素的缓冲区

	val_stack = creat_stack(20);
	opt_stack = creat_stack(20);

	while(*str){
		//1.解析字符串

		if(*str >= '0' && *str <= '9'){
			//1.数值转换
			sum = 0;
			while( *str >= '0' && *str <= '9' ){
				sum = sum * 10 + *str - '0';
				str ++;
			}
			//2.压栈
			push_stack(val_stack,&sum);
			continue;
		}

		if('+' == *str || '-' == *str || '*' == *str || '/' == *str){
			if(empty_stack(opt_stack)){
				buf_opt = *str;
				push_stack(opt_stack,&buf_opt);
				str ++;
				continue;
			}
			get_stack(opt_stack,&buf_opt);
			if(weight(*str) > weight(buf_opt)/*运算符权值大*/){
				buf_opt = *str;
				push_stack(opt_stack,&buf_opt);
				str ++;
				continue;
			}
			if(weight(*str) <= weight(buf_opt)/*运算符权值不大于栈顶*/){
				computer_once(val_stack,opt_stack);
				continue;
			}

			continue;
		}

		if('(' == *str){
			buf_opt = *str;
			push_stack(opt_stack,&buf_opt);
			str ++;
			continue;
		}
		if(')' == *str){
			while(1){
				get_stack(opt_stack,&buf_opt);
				if('(' == buf_opt)
					break;

				computer_once(val_stack,opt_stack);
			}
			pop_stack(opt_stack,&buf_opt);
			str ++;
			continue;
		}

		str ++;

	}
	//继续计算剩余部分

	while(!computer_once(val_stack,opt_stack))
		;
	pop_stack(val_stack,&buf_opt);

	dis_stack(&val_stack);
	dis_stack(&opt_stack);
	return buf_opt;
}


int main(int argc, const char *argv[])
{
	//1。输入一个运算式
	//2.计算
	//3.打印

	char buf[128] = {0};
	int ans = 0;

	fgets(buf,sizeof(buf),stdin);

	ans = computer(buf);

	printf(" ans = %d\n",ans);

	return 0;
}


