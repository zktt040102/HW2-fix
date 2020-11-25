#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <net/if.h>
#include <sys/ioctl.h>

int sockfd;//服務器socket
struct fdss{
    int num;
    char name[80];
    int duel;
    int duelpl;
};
struct chest{
    char name[5];
};
int ground = 0;
struct fdss fds[100];//客戶端的socketfd,100個元素，fds[0]~fds[99]
int size =100 ;//用來控制進入聊天室的人數為100以內
char* IP ="127.0.0.1";
short PORT = 10222;
typedef struct sockaddr SA;

void init(){
    sockfd = socket(PF_INET,SOCK_STREAM,0);
    if (sockfd == -1){
        perror("創建socket失敗");
        exit(-1);
    }

    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);
    if (bind(sockfd,(SA*)&addr,sizeof(addr)) == -1){
        perror("綁定失敗");
        exit(-1);
    }
    if (listen(sockfd,100) == -1){
        perror("設置監聽失敗");
        exit(-1);
    }
}

void playgame(int a, int b, int fd){
    ground = 2;
    int cnt = 0;
    struct chest c[9];
    int d[9];
    int i;
    int time = 1;
    int big, small;
    if(fds[a].duel > fds[b].duel){
	big = b;
	small = a;
    }
    else{
	big = a;
	small = b;	    
    }
    for(i = 0;i < 9;i++) sprintf(c[i].name,"\t");
    for(i = 0;i < 9;i++) d[i] = 0;
    char desk[80] = {};
    char buf2[80] = {};
    sprintf(buf2,"輸入1,2,3,4,5,6,7,8,9下棋");
    while(1){
        char buf[100] = {};
	snprintf(desk,80,"|%s|%s|%s|\n",c[0].name,c[1].name,c[2].name);
	send(fds[a].num,desk,strlen(desk),0);
	send(fds[b].num,desk,strlen(desk),0);
	snprintf(desk,80,"|%s|%s|%s|\n",c[3].name,c[4].name,c[5].name);
	send(fds[a].num,desk,strlen(desk),0);
	send(fds[b].num,desk,strlen(desk),0);
	snprintf(desk,80,"|%s|%s|%s|\n",c[6].name,c[7].name,c[8].name);
	send(fds[a].num,desk,strlen(desk),0);
	send(fds[b].num,desk,strlen(desk),0);
	send(fds[a].num,buf2,strlen(buf2),0);
	send(fds[b].num,buf2,strlen(buf2),0);
	sprintf(buf2,"你的回合：");
	if(time == 0){
	    if(fd == fds[big].num){
		send(fd,buf2,strlen(buf2),0);
	    }
	}
	else{
	    if(fd == fds[small].num){
		send(fd,buf2,strlen(buf2),0);
	    }
	}
	sprintf(buf2,"你贏了");
    	if((d[0]==d[1]==d[2]==4) || (d[3]==d[4]==d[5]==4) || (d[6]==d[7]==d[8]==4) || (d[0]==d[3]==d[6]==4) || (d[1]==d[4]==d[7]==4) || (d[2]==d[5]==d[8]==4) || (d[0]==d[4]==d[8]==4) ||(d[2]==d[4]==d[6]==4)){
	    send(fds[big].num,buf2,strlen(buf2),0);
	    fds[a].duel = 0;
	    fds[b].duel = 0;
	    fds[a].duelpl = 0;
	    fds[b].duelpl = 0;
	   	 break;
	}
	else if((d[0]==d[1]==d[2]==2) || (d[3]==d[4]==d[5]==2) || (d[6]==d[7]==d[8]==2) || (d[0]==d[3]==d[6]==2) || (d[1]==d[4]==d[7]==2) || (d[2]==d[5]==d[8]==2) || (d[0]==d[4]==d[8]==2) ||(d[2]==d[4]==d[6]==2)){
	    send(fds[small].num,buf2,strlen(buf2),0);
	    fds[a].duel = 0;
	    fds[b].duel = 0;
	    fds[a].duelpl = 0;
	    fds[b].duelpl = 0;
	   	 break;
	}
        if (recv(fd,buf,sizeof(buf),0) <= 0){
            for (i = 0;i < size;i++){
                if (fd == fds[i].num){
                    fds[a].duel = 0;
	    	    fds[b].duel = 0;
	    	    fds[a].duelpl = 0;
	    	    fds[b].duelpl = 0;
                    break;
                }
            }
	    char tmp[] = "GameTerminate";
	    send(fds[a].num,tmp,strlen(tmp),0);
	    send(fds[b].num,tmp,strlen(tmp),0);
	    break;
	}
	if(time == 0){
	    if(fd == fds[big].num){
		int num = atoi(buf);
		sprintf(c[i].name,"X\t");
		d[num-1] = 4;
		time = 1;
	    }
	}
	else if(time == 1){
	    if(fd == fds[small].num){
		int num = atoi(buf) -1;
		sprintf(c[i].name,"O\t");
		d[num-1] = 2;
		time = 0;
	    }
	}
	printf("1\n");
    }
}

void* service_thread(void* p){
    int fd = *(int*)p;
    printf("pthread = %d\n",fd);
    int cnt = 0;
    char buf2[100] = {};
    while(1){
        char buf[100] = {};
	int i;
	if(ground == 1){
	    for (i = 0;i < size;i++)
                if (fd == fds[i].num)break;
	    if(fds[i].duel >= 3){
		int j;
		for(j = 0;j < size;j++){
  		     if(i != j) if(fds[j].duel >= 3) break;
		}
		playgame(i,j,fd);
		ground = 0; 	
	    }	
	}
        if (recv(fd,buf,sizeof(buf),0) <= 0){
            int i;
            for (i = 0;i < size;i++){
                if (fd == fds[i].num){
                    fds[i].num = 0;
		    char buf3[80] = {};
		    sprintf(fds[i].name,"%s",buf3);
                    break;
                }
            }
                printf("退出：fd = %dquit\n",fd);
                pthread_exit((void*)i);
        }
        //把服務器接受到的信息發給指定客戶端
	if(cnt == 0){
	    int i;
            for (i = 0;i < size;i++){
                if (fd == fds[i].num){
                    sprintf(fds[i].name,"%s",buf);
                    break;
                }
            }	
	}
	if(strcmp(buf,"list") == 0){
		int i;
		for (i = 0;i < size;i++){
        		if (fds[i].num != 0){
			    sprintf(buf2,"%d. %s\n",fds[i].num,fds[i].name);
            		    send(fd,buf2,strlen(buf2),0);
       			}
    		}
	}
	for (i = 0;i < size;i++)
	    if (fd == fds[i].num) break;
	if(fds[i].duel == 1){
	    int input = atoi(buf);
	    int j;
	    for (j = 0;j < size;j++){
		if(fds[j].num == input) break;
 	    }
	    if((0 <= input) && (input <= 99)){
		if(fds[j].duel == 0){
		    fds[j].duel = 2;
		    fds[i].duelpl = fds[j].num;
		    char tmp[80];
		    sprintf(tmp,"%s向你申請對決  Y/N",fds[i].name);
		    send(fds[j].num,tmp,strlen(tmp),0);
		}
		else{
		    char tmp[] = "對象忙碌中，請重新申請對決";
		    fds[i].duel = 0;
		    send(fd,tmp,strlen(tmp),0);
		}
	    }
	    else{
		char tmp[] = "輸入錯誤，請重新申請對決";
		fds[i].duel = 0;
		send(fd,tmp,strlen(tmp),0);
	    }
	}
	if(fds[i].duel == 2){
	    int j;
	    for (j = 0;j < size;j++){
		if(fds[j].duelpl == fds[i].num) break;
 	    }
	    if(strcmp(buf,"Y") == 0){
		if(ground == 0) {
		    fds[i].duel = 4;
		    fds[j].duel = 3;
		    ground = 1;
		}
		else{
		    char tmp[] = "場地使用中，請重新申請對決";
		    fds[i].duel = 0;
		    fds[j].duel = 0;
		    fds[j].duelpl = 0;
		    send(fd,tmp,strlen(tmp),0);
		    send(fds[j].num,tmp,strlen(tmp),0);
		}
	    }
	    else if(strcmp(buf,"N") == 0){
		char tmp[] = "對方拒絕，請重新申請對決";
		fds[i].duel = 0;
		fds[j].duel = 0;
		fds[j].duelpl = 0;
		send(fds[j].num,tmp,strlen(tmp),0);
	    }
	    else{
		char tmp[] = "輸入錯誤，請重新申請對決";
		fds[i].duel = 0;
		fds[j].duel = 0;
		fds[j].duelpl = 0;
		send(fd,tmp,strlen(tmp),0);
		send(fds[j].num,tmp,strlen(tmp),0);
	    }
	}
	if(strcmp(buf,"duel") == 0){
	    if(fds[i].duel == 0){
		fds[i].duel = 1;
		char tmp[] = "想和誰對決？ 請輸入號碼：";
		send(fd,tmp,strlen(tmp),0);
	    } 
        }
	cnt ++;
    }
}

void service(){
    printf("服務器啟動\n");
    while(1){
        struct sockaddr_in fromaddr;
        socklen_t len = sizeof(fromaddr);
        int fd = accept(sockfd,(SA*)&fromaddr,&len);
        if (fd == -1){
            printf("客戶端連接出錯...\n");
            continue;
        }
        int i = 0;
        for (i = 0;i < size;i++){
            if (fds[i].num == 0){
                //記錄客戶端的socket
                fds[i].num = fd;
                printf("fd = %d\n",fd);
                //有客戶端連接之后，啟動線程給此客戶服務
                pthread_t tid;
                pthread_create(&tid,0,service_thread,&fd);
                break;
            }
        if (size == i){
            //發送給客戶端說聊天室滿了
            char* str = "對不起，聊天室已經滿了!";
            send(fd,str,strlen(str),0); 
            close(fd);
        }
        }
    }
}

int main(){
    init();
    service();
}
