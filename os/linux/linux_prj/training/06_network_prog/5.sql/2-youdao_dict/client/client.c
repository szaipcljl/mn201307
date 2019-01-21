#include "head.h"

void help1()
{
	printf("***********************\n");
	printf("1.QUERRY 2.HISTORY 3.DEL_HISTORY\n4.QUIT\n");
	printf("***********************\n");
	fflush(stdout);

	return ;
}

void help0()
{
	printf("***********************\n");
	printf("1.register 2.login 3.quit\n");
	printf("***********************\n");
	fflush(stdout);
	return ;
}

//终端输入的字符变更为 *
int get_passwd(char *password, int len)
{
	int i  = 0;
	initscr(); //
	clear();
	printw("PASSWD:");

	refresh();

	refresh();

	bzero(password,NAME_LEN);
	while((i < len) && ((password[i] = getchar()) != '\r' ))
	{
		printw("*");
		refresh();
		i ++;
	}
	password[i] = 0;
	endwin();
//	puts(passwd);
	return 0;
}

int set_disp_mode(int fd,int option)
{
	int err;
	struct termios term;

	if(-1 == tcgetattr(fd,&term)){
		perror("ERROR");
		return -1;
	}

	if(option)
		term.c_lflag |= ECHOFLAGS;
	else
		term.c_lflag &= ~ECHOFLAGS;

	err = tcsetattr(fd,TCSAFLUSH,&term);
	if(-1 == err){
		perror("error");
		return -2;
	}
	return 0;
}


int input_usr_info(msg_t *msg)
{

l1:
	printf("NAME:\n");
	fflush(stdout);

	fgets(msg->name,NAME_LEN,stdin);
	msg->name[strlen(msg->name) - 1] = '\0';
	if(strlen(msg->name) == 0)
		goto l1;

#if 1

l2:
	set_disp_mode(STDIN_FILENO,0);

	printf("PASSWD:\n");

	fgets(msg->passwd,NAME_LEN,stdin);
	msg->passwd[strlen(msg->passwd) - 1] = '\0';
	if(strlen(msg->passwd) == 0)
		goto l2;

	set_disp_mode(STDIN_FILENO,1);
#else

	get_passwd(msg->passwd,NAME_LEN);

#endif

	/*
l2:
	printf("PASSWD:\n");
	fflush(stdout);

	fgets(msg->passwd,NAME_LEN,stdin);
	msg->passwd[strlen(msg->passwd) - 1] = '\0';
	if(strlen(msg->passwd) == 0)
		goto l2;

	*/
	return 0;
}

int send_message(int sockfd,msg_t *msg)
{
	if(-1 == send(sockfd,msg,sizeof(msg_t),0))
		err_sys("send_message");

	return 0 ;
}

int recv_message(int sockfd,msg_t *msg)
{
	if(recv(sockfd,msg,sizeof(msg_t),0)  <= 0 )
		err_sys("recv_message");
	return 0;
}

int do_register(int sockfd,msg_t *msg)
{
	input_usr_info(msg);

	msg->type = REGISTER ;
	send_message(sockfd,msg);

	recv_message(sockfd,msg);
	if(msg->flag == -1)
	{
		printf("the user is already exists!!!\n");
		fflush(stdout);
		return -1;
	}else if(msg->flag == -2)
	{
		printf("register fail!!!\n");
		fflush(stdout);
		return -2 ;
	}

	return 0;
}

int do_login(int sockfd,msg_t *msg)
{
	input_usr_info(msg);
	msg->type = LOGIN ;
	send_message(sockfd,msg);

	recv_message(sockfd,msg);

	if(msg->flag == -3)
	{
		printf("INPUT VALID USER NAME,PLEASE REGISTER!!!\n");
		return -1;
	}else if(msg->flag == -1)
	{
		printf("PLEASE INPUT VALID PASSWD!!!\n");
		return -2;
	}else if(msg->flag == -2)
	{
		printf("LOGIN FAILED!!!\n");
		return -3 ;
	}

	return 0;
}

int do_quit(int sockfd,msg_t *msg)
{

	free(msg);
	close(sockfd);
	exit(EXIT_SUCCESS);

	return 0;
}
int input_word_info(msg_t *msg)
{
	char buf[64];
l4:
	printf("WORD:");
	fflush(stdout);
	fgets(buf,64,stdin);
	buf[strlen(buf) - 1] = '\0';
	if( strlen(buf) == 0)
		goto l4;

	strcpy(msg->data,buf);
	msg->flag = 1 ;
	return 0;
}

int do_querry(int sockfd,msg_t *msg)
{
	while(1)
	{
		input_word_info(msg);

		if(strcmp(msg->data,"#") == 0 )
			break;
		msg->type = QUERRY ;

		send_message(sockfd,msg);
		recv_message(sockfd,msg);

		if(msg->flag == -1)
		{
			printf("NO SUCH WORD!!!\n");
			continue ;
		}

		if(msg->flag == -2)
		{
			printf("QUERRY FAILED!!!\n");
			continue ;
		}

		printf("disp:%s\n",msg->data);
	}
	return 0;
}


int do_delhistory(int sockfd,msg_t *msg)
{
	msg->type = DEL_HISTORY ;

	send_message(sockfd,msg);

	recv_message(sockfd,msg);

	if(msg->flag == -1)
	{
		printf("DELETE  HISTORY  FAILED!!!\n");
		return -1;
	}

	printf("DELETE HISTORY SUCESSFULLY!!!\n");

	return 0;
}


int cli_init()
{
	int sockfd ;
	struct  sockaddr_in  my_addr;
	struct  sockaddr_in  ser_addr , peer_addr;
	socklen_t len = sizeof(struct sockaddr_in);

	bzero(&my_addr,sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(CLI_PORT);
	my_addr.sin_addr.s_addr  = inet_addr(CLI_IP);

	bzero(&ser_addr,sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(SER_PORT);
	ser_addr.sin_addr.s_addr = inet_addr(SER_IP);



	if(-1 == (sockfd = socket(AF_INET,SOCK_STREAM,0)))
		err_sys("socket");
#if 0
	if(-1 ==  bind(sockfd,(SA *)&my_addr,sizeof(my_addr) ))
		err_sys("bind");
#endif

	if(-1 == connect(sockfd,(SA *)&ser_addr,sizeof(ser_addr)))
		err_sys("connect");

	return sockfd ;
}


int getcmd0()
{
	char buf[32];

lo:
	printf(">");
	fflush(stdout);
	fgets(buf,32,stdin);
	buf[strlen(buf) - 1] = '\0';

	if(strlen(buf) == 0)
		goto lo;

	if(strcmp(buf,"1") == 0)
		return REGISTER;
    if(strcmp(buf,"2") == 0)
		return LOGIN;
	if(strcmp(buf,"3") == 0)
		return QUIT;

	return 0;
}

int getcmd1()
{
	char buf[32];

lo:
	printf(">");
	fflush(stdout);
	fgets(buf,32,stdin);
	buf[strlen(buf) - 1] = '\0';

	if(strlen(buf) == 0)
		goto lo;

	if(strcmp(buf,"1") == 0)
		return QUERRY;
    if(strcmp(buf,"2") == 0)
		return HISTORY;
	if(strcmp(buf,"3") == 0)
		return DEL_HISTORY;
	if(strcmp(buf,"4" ) == 0)
		return EXIT;

	return 0;
}

int do_youdao(int sockfd)
{
	 msg_t * msg  = malloc(sizeof(msg_t));

	while(1)
	{
		help0();

		switch(getcmd0())
		{
		case REGISTER:
			if(do_register(sockfd,msg) >= 0)
			{
				printf("register sucessfully!!!\n");
				fflush(stdout);
			}
			break;

		case LOGIN:
			if(do_login(sockfd,msg) >= 0)
			{
				printf("login sucessfully !!!\n");
				goto next;
			}
			break;

		case QUIT:
			do_quit(sockfd,msg);
			break;

		default:
			printf("invalid cmd!!!\n");
			break ;
		}
	}

next:
	while(1)
	{
		help1();
		switch(getcmd1())
		{
		case  QUERRY:
			do_querry(sockfd,msg);
			break;

		case  HISTORY:
			do_history(sockfd,msg);
			break;

		case DEL_HISTORY:
			do_delhistory(sockfd,msg);
			break;

		case EXIT:
			do_exit(sockfd,msg);
			break;
		}
	}

	return 0;
}

int do_exit(int sockfd,msg_t *msg)
{
	msg->type = EXIT;
	send_message(sockfd,msg);

	free(msg);
	close(sockfd);
	exit(EXIT_SUCCESS);

	return 0;
}

int do_history(int sockfd,msg_t *msg)
{
	msg->type = HISTORY;

	send_message(sockfd,msg);

	recv_message(sockfd,msg);

	if(msg->flag == -1)
	{
		printf("NO HISTORY RECORD!!!\n");
		return -1;
	}
	if(msg->flag == -2)
	{
		printf("QUERRY HISTORY RECORD FAILED!!!\n");
		return -2;
	}

	printf("%s\n",msg->data);

	while(1)
	{
		recv_message(sockfd,msg);
		if(msg->flag != 1)
			break ;
		printf("%s",msg->data);
	}

	return 0;
}

int main(int argc, const char *argv[])
{

	int sockfd ;

	sockfd = cli_init();

	do_youdao(sockfd);

	return 0;
}

