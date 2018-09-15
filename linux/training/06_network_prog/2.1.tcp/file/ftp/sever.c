


int download()
{

	// test open file
	//  1.打开成功
	//  2.打开失败
	//    send 》》 ok buok
	//
	while(){
		//read
		//send
	}

}



int main(int argc, const char *argv[])
{


	//socket
	//bind
	//listen

	while(1){
		//accept();

		if(0 == fork()){
			if(0 == fork()){
				close(sockfd);//关闭子进程里用不到的监听套接字
				while(1){
					//.........

					//recv(); //接收客户指令

					switch(buf.cmd){
					case DOWNLOAD:
						download();
					case GETUP:
						getup();
					}
					//执行指令
					// 1.getup
					// 2.download
					// 3.list
					// 4.quit

				}
			}
			exit(-1);

		}
		close(acceptfd); //关闭爷爷里用不到的 链接套接字
		wait(NULL);
	}

	return 0;
}
