#include "header.h"

void execute(struct state *);
void parse(char*, struct state *);

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
	printf("%d %d %d %d\n", meta_id, d1_id, d2_id, d3_id);
	while(1){
		struct state present;
		char com[1024];
		scanf("%[^\n]%*c",com);
		//printf("%s\n", com);
		parse(com, &present);
		execute(&present);
		//for(int i = 0;i<present.num;i++)free(present.arg[1]);
		free(present.arg);
		//printf("hey us see em\n");
		//for(int i = 0; i<present.num;i++)printf("%s ", present.arg[i]);
	}
}

void parse(char*buf, struct state*curr){
	char c;
	short index = 0;
	char temp[200];
	int t = 0;
	int tot = 200;
	int fd;//
	char**path;//
	int n = strlen(buf);
	curr->arg = (char**)malloc(tot*sizeof(char*));
	for(int i = 0;i<n;i++){
		switch(buf[i]){
			case '\\':
				i++;
				temp[index] = buf[i];
				index++;
				continue;
			case ' ':
				if(index>0){
					temp[index] = '\0';
					char *tmp = (char*)malloc((strlen(temp)+1)*sizeof(char));
					strcpy(tmp,temp);
					
					index = 0;
					curr->arg[t] = tmp;
					//printf("%s\n", arg[t]);
					t++;
					if(t>tot){
						tot = (3*tot)/2;
						curr->arg = realloc(curr->arg, tot*sizeof(char*));
					}
					continue;
				}
				continue;
			default:
				;
		}
		temp[index] = buf[i];
		index++;		
	}
	if(index>0){
		temp[index] = '\0';
		char *tmp = (char*)malloc((strlen(temp)+1)*sizeof(char));
		strcpy(tmp,temp);
		index = 0;
		curr->arg[t] = tmp;
		//printf("%s\n", arg[t]);
		t++;
		if(t>tot){
		tot = (3*tot)/2;
			curr->arg = realloc(curr->arg, tot*sizeof(char*));
		}
	}
	curr->arg = realloc(curr->arg, t*sizeof(char*));
	curr->num = t;
}

void execute(struct state *com){
	if(strcmp(com->arg[0], "add")==0){
		if(com->num != 3){
			printf("Arguments Error\n");
			return;
		}
		struct msg_meta msg;
		strcpy(msg.mcom, com->arg[0]);
		strcpy(msg.mfile1, com->arg[2]);
		//printf("yes\n");
		msg.mtype = 1;
		int fd = open(com->arg[1], O_RDONLY,0);
		if(fd == -1){
			printf("FILE error\n");
			return;
		}
		if(msgsnd(meta_id, &msg, sizeof(msg), 0)==-1)printf("error\n");
		msgrcv(meta_id, &msg, sizeof(msg), 1, 0);
		if(strcmp(msg.mcom,"y")==0){
			int size = 0;
			printf("Enter chunk size (limit 4998 bytes):");scanf("%d%*c",&size);
			char tm[size+1];
			int fileno = 1;
			char c;
			int ct = 0;
			while(read(fd,&c,1)!=0){
				tm[ct] = c;
				ct++;
				if(ct == size){
					struct msg_data md;
					md.mtype = 1;
					strcpy(md.mfile, com->arg[2]);
					char charno[7];
					sprintf(charno, "%d", fileno);
					strcat(md.mfile, charno);
					fileno++;
					tm[ct] = '\0';
					strcpy(md.mdata,tm);
					ct = 0;
					//printf("%s\n", md.mdata);
					if(msgsnd(d1_id, &md, sizeof(md), 0)==-1)printf("error1\n");
					if(msgsnd(d2_id, &md, sizeof(md), 0)==-1)printf("error2\n");
					if(msgsnd(d3_id, &md, sizeof(md), 0)==-1)printf("error3\n");
				}
			}
			if(ct>0){
				struct msg_data md;
				md.mtype = 1;
				strcpy(md.mfile, com->arg[2]);
				char charno[7];
				sprintf(charno, "%d", fileno);
				strcat(md.mfile, charno);
				fileno++;
				tm[ct] = '\0';
				strcpy(md.mdata,tm);
				ct = 0;
				if(msgsnd(d1_id, &md, sizeof(md), 0)==-1)printf("error1\n");
				if(msgsnd(d2_id, &md, sizeof(md), 0)==-1)printf("error2\n");
				if(msgsnd(d3_id, &md, sizeof(md), 0)==-1)printf("error3\n");
			}
			close(fd);
			struct msg_data md;
			md.mfile[0] = '\0';
			md.mdata[0] = '\0';
			if(msgsnd(d1_id, &md, sizeof(md), 0)==-1)printf("error1;\n");
			if(msgsnd(d2_id, &md, sizeof(md), 0)==-1)printf("error2;\n");
			if(msgsnd(d3_id, &md, sizeof(md), 0)==-1)printf("error3;\n");
			return;
		}
		
		
		//if(msgsnd(d1_id, &msg, sizeof(msg), 0)==-1)printf("error1\n");
		//if(msgsnd(d2_id, &msg, sizeof(msg), 0)==-1)printf("error2\n");
		//if(msgsnd(d3_id, &msg, sizeof(msg), 0)==-1)printf("error3\n");
	}
	if(strcmp(com->arg[0], "mv")==0){
		struct msg_meta msg;
		msg.mtype = 1;
		strcpy(msg.mcom, com->arg[0]);
		strcpy(msg.mfile1, com->arg[1]);
		strcpy(msg.mfile2, com->arg[2]);
		if(msgsnd(meta_id, &msg, sizeof(msg), 0)==-1)printf("error\n");
	}
	if(strcmp(com->arg[0], "cp")==0){
		struct msg_meta msg;
		msg.mtype = 1;
		strcpy(msg.mcom, com->arg[0]);
		strcpy(msg.mfile1, com->arg[1]);
		strcpy(msg.mfile2, com->arg[2]);
		if(msgsnd(meta_id, &msg, sizeof(msg), 0)==-1)printf("error\n");
	}
	if(strcmp(com->arg[0], "rm")==0){
		struct msg_meta msg;
		msg.mtype = 1;
		strcpy(msg.mcom, com->arg[0]);
		strcpy(msg.mfile1, com->arg[1]);
		//strcpy(msg.mfile2, com->arg[2]);
		if(msgsnd(meta_id, &msg, sizeof(msg), 0)==-1)printf("error\n");
	}
	if(strcmp(com->arg[0], "cmd")==0){
		struct msg_meta msg;
		msg.mtype = 1;
		strcpy(msg.mcom, com->arg[0]);
		//strcpy(msg.mfile1, com->arg[1]);
		if(msgsnd(d1_id, &msg, sizeof(msg), 0)==-1)printf("error1\n");
		//if(msgsnd(d2_id, &msg, sizeof(msg), 0)==-1)printf("error\n");
		//if(msgsnd(d3_id, &msg, sizeof(msg), 0)==-1)printf("error\n");
		struct msg_cmd cm;
		struct msg_cmd_data dm;
		cm.mtype = 1;
		cm.mlen = com->num - 1;
		for(int i = 1;i<com->num;i++)strcpy(cm.mcmd[i-1], com->arg[i]);
		if(msgsnd(d1_id, &cm, sizeof(cm), 0)==-1)printf("error12\n");
		if(msgrcv(d1_id, &dm, sizeof(dm), 1, 0)==-1)printf("error123\n");
		printf("tets\n");
		while(dm.mlen!=0){
			printf("tets\n");
			printf("%s test",dm.mdata);
			if(msgrcv(d1_id, &dm, sizeof(dm), 1, 0)==-1)printf("error123\n");
		}
	}	
}