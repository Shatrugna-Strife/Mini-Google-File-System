#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include <sys/shm.h> 
#include<stdbool.h>
#include<fcntl.h>
#include<sys/wait.h>
#define META "./meta.c"
#define DATA "./data.c"


struct msg_meta{
	long mtype;
	char mcom[5];
	char mfile1[250];
	char mfile2[250];
};

struct msg_data{
	long mtype;
	int mlen;
	char mfile[250];
	char mdata[5000];
};

struct msg_cmd{
	long mtype;
	int mlen;
	//char mfile[250];
	char mcmd[20][250];
};

struct msg_cmd_data{
	long mtype;
	int mlen;
	char mdata[5000];
};

struct state{
	char**arg;
	int num;
};




