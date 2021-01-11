#include"header.h"


void createFolder(char *);
void receive(int);

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
	if(fork()==0){
		if(fork()==0){
			d3_key = ftok(DATA,3);
			d3_id = msgget(d3_key, 0666 | IPC_CREAT);
			createFolder("DataServer3");
			receive(d3_id);
		}else{
			d2_key = ftok(DATA,2);
			d2_id = msgget(d2_key, 0666 | IPC_CREAT);
			createFolder("DataServer2");
			receive(d2_id);
		}
	}else{
		d1_key = ftok(DATA,1);
		d1_id = msgget(d1_key, 0666 | IPC_CREAT);
		createFolder("DataServer1");
		receive(d1_id);
	}
	int status;
	while(wait(&status)>0);
}

void createFolder(char *folder){
	if(fork()==0){
		char *arg[] = {"mkdir", folder,NULL};
		execvp(arg[0], arg);
	}
	int status;
	wait(&status);
	chdir(folder);
	char s[200];
	printf("%s\n",getcwd(s,200));

}

void receive(int id){
	struct msg_meta msg;
	while(1){
		if(msgrcv(id, &msg, sizeof(msg), 1, 0)==-1)printf("error567765\n");
		if(strcmp(msg.mcom, "add")==0){
			//printf("add%d\n",id);
			while(1){
				//printf("%d rcv\n", id);
				struct msg_data d;
				if(msgrcv(id, &d, sizeof(d), 1, 0)==-1)printf("error999\n");
				int n = strlen(d.mdata);
				//printf("%s %d\n", d.mdata, n);
				if(n == 0)break;
				if(fork()==0){
					execlp("touch","touch",d.mfile,NULL);
				}
				int fd = open(d.mfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if(fd == -1){
					printf("FILE error\n");
				}
				//printf("%s\n",d.mdata);
				write(fd,d.mdata,n);
				close(fd);
			}
		}
		if(strcmp(msg.mcom, "mv")==0){
			int i = 1;
			int value;
			
			while(!value){
				char charno[7];
				sprintf(charno, "%d", i);
				char file1[250];
				char file2[250];
				strcpy(file1,msg.mfile1);
				strcat(file1, charno);
				strcpy(file2,msg.mfile2);
				strcat(file2, charno);
				value = rename(file1, file2); 
				i++;
			}
		}
		if(strcmp(msg.mcom, "cp")==0){
			int i = 1;
			int value;
			
			while(1){
				char charno[7];
				sprintf(charno, "%d", i);
				char file1[250];
				char file2[250];
				strcpy(file1,msg.mfile1);
				strcat(file1, charno);
				strcpy(file2,msg.mfile2);
				strcat(file2, charno);
				int f1 = open(file1,  O_RDONLY, 0);
				if(f1== -1)break;
				int f2 = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				char c;
				while(read(f1,&c,1))write(f2,&c,1);
				//value = rename(file1, file2); 
				i++;
				close(f1);
				close(f2);
			}
		}
		if(strcmp(msg.mcom, "rm")==0){
			int i = 1;
			int value;
			
			while(!value){
				char charno[7];
				sprintf(charno, "%d", i);
				char file1[250];
				//char file2[250];
				strcpy(file1,msg.mfile1);
				strcat(file1, charno);
				//strcpy(file2,msg.mfile2);
				//strcat(file2, charno);
				value = remove(file1); 
				i++;
			}
		}
		if(strcmp(msg.mcom, "cmd")==0){
			struct msg_cmd cm;
			if(msgrcv(id, &cm, sizeof(cm), 1, 0)==-1)printf("eererw");
			int fd = open("temp",O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if(fork()==0){
				dup2(fd, STDOUT_FILENO);
				execlp("ls","ls",NULL);
			}
			close(fd);
			FILE *f = fopen("temp","r");
			char fi [250];
			fd = open("templist",O_WRONLY | O_CREAT | O_TRUNC, 0644);
			char**arg = (char**)malloc((cm.mlen+2)*sizeof(char*));
			for(int i =0;i<cm.mlen;i++){
				arg[i] = cm.mcmd[i];
			}
			arg[cm.mlen+1] = NULL;
			int s = fscanf(f,"%s\n",fi);
			while(s){
				write(fd,fi,strlen(fi));
				write(fd,"\n",1);
				arg[cm.mlen] = fi;
				if(fork()==0){
					dup2(fd, STDOUT_FILENO);
					execvp(arg[0],arg);
				}
				write(fd,"\n",1);
				s = fscanf(f,"%s\n",fi);
			}
			fclose(f);
			close(fd);
			//remove("temp");
			fd = open("templist",O_RDONLY, 0);
			struct msg_cmd_data dm;
			dm.mtype = 1;
			dm.mlen = read(fd,dm.mdata,4500);
			while(dm.mlen){
				dm.mdata[dm.mlen] = '\0';
				if(msgsnd(d1_id, &dm, sizeof(dm), 0)==-1)printf("error\n");
				dm.mlen = read(fd,dm.mdata,4500);
			}
			if(msgsnd(d1_id, &dm, sizeof(dm), 0)==-1)printf("error\n");
			close(fd);
			//remove("templist");
		}
	//printf("%s\n",msg.mfile1);
	}
	exit(0);
}
