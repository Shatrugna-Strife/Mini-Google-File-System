#include"header.h"

void createFolder();
void receive();

key_t meta_key;
int meta_id;
key_t d1_key;
int d1_id;
key_t d2_key;
int d2_id;
key_t d3_key;
int d3_id;

int main(){
	meta_key = ftok(META,2);
	meta_id = msgget(meta_key, 0666 | IPC_CREAT);
	d1_key = ftok(DATA,1);
	d1_id = msgget(d1_key, 0666 | IPC_CREAT);
	d2_key = ftok(DATA,2);
	d2_id = msgget(d2_key, 0666 | IPC_CREAT);
	d3_key = ftok(DATA,3);
	d3_id = msgget(d3_key, 0666 | IPC_CREAT);
	createFolder();
	receive();
}

void createFolder(){
	if(fork()==0){
		char *arg[] = {"mkdir", "MetaData",NULL};
		execvp(arg[0], arg);
	}
	int status;
	wait(&status);
	chdir("MetaData");
	char s[200];
	printf("%s\n",getcwd(s,200));
}

void receive(){
	struct msg_meta msg;
	while(1){
		msgrcv(meta_id, &msg, sizeof(msg), 1, 0);
		if(strcmp(msg.mcom, "add")==0){
			if(fork()==0){
					execlp("touch", "touch", msg.mfile1,NULL);
			}
			msg.mtype = 1;
			stpcpy(msg.mcom,"add");
			if(msgsnd(d1_id, &msg, sizeof(msg), 0)==-1)printf("error\n");			
			if(msgsnd(d2_id, &msg, sizeof(msg), 0)==-1)printf("error\n");					
			if(msgsnd(d3_id, &msg, sizeof(msg), 0)==-1)printf("error\n");	
			msg.mtype = 1;			
			stpcpy(msg.mcom,"y");
			if(msgsnd(meta_id, &msg, sizeof(msg), 0)==-1)printf("error\n");			
		}
		if(strcmp(msg.mcom, "mv")==0){
			int value = rename(msg.mfile1, msg.mfile2);
			if(value)continue;
			//if(fork()==0){
			//	execlp("mv","mv", msg.mfile1, msg.mfile2,NULL);
			//	printf("%s File do not exists\n", msg.mfile1);
			//}
			msg.mtype = 1;
			if(msgsnd(d1_id, &msg, sizeof(msg), 0)==-1)printf("error\n");			
			if(msgsnd(d2_id, &msg, sizeof(msg), 0)==-1)printf("error\n");					
			if(msgsnd(d3_id, &msg, sizeof(msg), 0)==-1)printf("error\n");
		}
		if(strcmp(msg.mcom, "cp")==0){
			int value = rename(msg.mfile1, msg.mfile2);
			if(value)continue;
			if(fork()==0){
					execlp("touch", "touch", msg.mfile1,NULL);
			}
			msg.mtype = 1;
			if(msgsnd(d1_id, &msg, sizeof(msg), 0)==-1)printf("error\n");			
			if(msgsnd(d2_id, &msg, sizeof(msg), 0)==-1)printf("error\n");					
			if(msgsnd(d3_id, &msg, sizeof(msg), 0)==-1)printf("error\n");
		}
		if(strcmp(msg.mcom, "rm")==0){
			int value = remove(msg.mfile1);
			if(value)continue;
			//if(fork()==0){
			//	execlp("rm","rm", msg.mfile1, NULL);
			//	printf("%s File do not exists\n", msg.mfile1);
			//}
			msg.mtype = 1;
			if(msgsnd(d1_id, &msg, sizeof(msg), 0)==-1)printf("error\n");			
			if(msgsnd(d2_id, &msg, sizeof(msg), 0)==-1)printf("error\n");					
			if(msgsnd(d3_id, &msg, sizeof(msg), 0)==-1)printf("error\n");
		}
		//if(strcmp(msg.mcom, "cmd")==0){
		//printf("%s\n",msg.mfile1);
	}
}
	
