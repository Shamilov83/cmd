#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define TOKEN_BUFSIZE 64
#define TOKEN_DELIM " \t\n\a\0"
#define TOKEN_DELIM_PIPE " |\n\0"

#define STDOUT 1
#define STDIN 0

#define OTL 0

char *line;
char **args;
int status = 1;
int position;

char *ReadLine(void);
void SplitLine(char *);
void CreatePipe(char**,char**);
int RunCmd(char **);

char **tokens = NULL;
char **tokens2 = NULL;
char *token = NULL;
char *token2 = NULL;
char *istr = NULL;	//
char *arrey1 = NULL;
char *arrey2 = NULL;
ssize_t bufsize;


int main (int argc, char *argv[])
{
	do{
	
	printf("%s","> ");
	line = ReadLine();
	SplitLine(line);

	free(line);

	}while(status);
return 0;
}


char *ReadLine()
{
	char *line = NULL;
	bufsize = 0;
	getline(&line,&bufsize,stdin);
return line;
}


void SplitLine(char *line)
{
int position = 0;

if((istr = strstr(line,"|"))== NULL)
{
	//printf("%s %s"," | не входит в строку","\n");
	tokens = malloc(bufsize * sizeof(char*));
	token = strtok(line, TOKEN_DELIM);
	while(token != NULL)
	{
		tokens[position] = token;
		position++;
		token = strtok(NULL,TOKEN_DELIM);
	}
tokens[position] = NULL;
//printf("%s %s %s","token= ",token,"\n");
RunCmd(tokens);
}
else{
	tokens = malloc(bufsize * sizeof(char*));
	tokens2 = malloc(bufsize * sizeof(char*));

	arrey1 = malloc(bufsize * sizeof(char*));
	arrey2 = malloc(bufsize * sizeof(char*));

	strncpy(arrey1,line,((istr-1)-line));
	strncpy(arrey2,(istr+2),10);

#if OTL == 1
printf("%s %s %s %s","дин.массив 1 =","'",arrey1,"'\n");
printf("%s %s %s %s","дин.массив 2 =","'",arrey2,"'\n");
#endif
	//разбиение на лексемы каждой команды
	token = strtok(arrey1, TOKEN_DELIM_PIPE);
		while(token != NULL)
		{
			tokens[position] = token;
			position++;
			token = strtok(NULL,TOKEN_DELIM_PIPE);
		}
	tokens[position] = NULL;

	position = 0;

	token2 = strtok(arrey2, TOKEN_DELIM_PIPE);
		while(token2 != NULL)
		{
			tokens2[position] = token2;
			position++;
			token2 = strtok(NULL,TOKEN_DELIM_PIPE);
		}
	tokens2[position] = NULL;

	CreatePipe(tokens,tokens2);	//создание неименованного канала
	}

}

int RunCmd(char **token)
{
char *new_env_tr[] = {" ",token[1],token[2],NULL};
char path[30] = {"/usr/bin/"};
char *com[1] = {token[0]};

//printf("%s %s %s","token == ",*com,"\n");

/*
if(strstr(com[0],"exit") == 0){
	status = 0;
	return	status;
	}
*/	
strncat(path,*com,5);

pid_t pid;
pid = fork();

if(pid == -1){
	printf("error create fork()1");
	exit(-1);
	}
if(pid == 0){
	execve(path,new_env_tr,NULL);
	perror("execve_tr");
	exit(0);	//завершить корректно
	}	
	else{
		
		waitpid(pid,NULL,0);
		free(tokens);
	}
}

/*создание неименованного канала и перенаправление стандартных потоков ввода-вывода в него. Создание дочерних процессов для каждой стороны неименованного канала.*/
void CreatePipe(char **token,char **token2)
{
char path[30] = {"/usr/bin/"};
char path2[30] = {"/usr/bin/"};

char *com[1] = {token[0]};
char *com2[1] = {token2[0]};

strncat(path,*com,5);
strncat(path2,*com2,5);

#if OTL == 1
printf("%s %s %s","path = ",path,"\n");
printf("%s %s %s","path2 = ",path2,"\n");
#endif

char recivebuf[14];
pid_t fd[2];

pid_t pid1;
pid_t pid2;


char *new_env_tr[] = {" ",token[1],token[2],NULL};//первый элемент массива не должен быть NULL, иначе параметр команды не работает
char *new_env_rs[] = {" ",token2[1],token2[2],NULL};

//printf("%s","CreatePipe\n");

	if(pipe(fd)== -1){
		printf("error create PIPE");
		exit(-1);
	}

pid1 = fork();//child process for transmite
//printf("%s %d %s","pid1 = ",pid1,"\n");

if(pid1 == -1){
	printf("error create fork()1");
	exit(-1);
}
if(pid1 == 0){
	//close recive stream
	dup2(fd[1],STDOUT); //создать дубликат fd с именем STDOUT. Стандартный поток вывода STDOUT закрывается, данные идут в канал fd 
	close(fd[0]);	
	close(fd[1]);
	//write(STDOUT,"Hello world!\n",14);
	execve(path,new_env_tr,NULL);
	perror("execve_tr");

	exit(EXIT_FAILURE);
	}
else{
	pid2 = fork();//child process for recive
	//printf("%s %d %s","pid2 = ",pid2,"\n");
	if(pid2 == -1){
		printf("error create fork()2");
		exit(-1);
	}
	if(pid2 == 0){
		//close transmite stream 
		dup2(fd[0],STDIN);
		close(fd[1]);
		close(fd[0]);
		//read(STDIN,recivebuf,14);
		//printf("%s",recivebuf);
		execve(path2,new_env_rs,NULL);
		perror("execve_rs");
		
		exit(EXIT_FAILURE);
	}
	else
	{
	close(fd[0]);
	close(fd[1]);
	waitpid(pid1,NULL,0);
	waitpid(pid2,NULL,0);
	free(tokens);
	free(tokens2);
	free(arrey1);
	free(arrey2);
	}

    }
}
