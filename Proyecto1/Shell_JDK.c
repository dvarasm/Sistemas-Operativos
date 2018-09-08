#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

char **linea_cmd;        //almacena sólo la linea de comando a ejecutar en el instante
char **args;	//almacena los comandos con sus argumentos
char *salir = "exit";
char *hist = "historial";
int count_cmd=0;
int count_pipe=0;
int pipes[10];
int ejecutarHist = 0;

typedef struct { 
    char *linea;
    long int time;
}his;

his* historial[1000];
int his_count = 0;


void parse_cmd();
void ejec(char *str);

void terminar(char *token){
	int ret = strncmp(token,salir,5); 
	if(ret == 0){     
        exit(0);
    }	
}

void historial_cmd(){
	int j=0;
	printf(">>HISTORIAL:\n");
		printf("\ttiempo(sec) \t comando\n");
	while(j<count_cmd){
		printf(">>%d\t", j);
		printf("%ld\t\t  %s\n", historial[j]->time, historial[j]->linea);
		j++;
	}  
}


void find_pipe(char str[100]){
	char *token;
	int i=0;

	his* h = malloc(sizeof(his));
    h->linea = malloc(sizeof(char)*strlen(str));
	strcpy(h->linea, str);
    his_count++;
    historial[his_count -1] = h;

	token = strtok(str, "|\n");
	while( token != NULL ) {	
		linea_cmd[i] = malloc((strlen(token))*sizeof(char));	
 		strcpy(linea_cmd[i],token);				
		terminar(token);
		i++;		
 		token = strtok(NULL, "|\n");	
 	}
    
 	count_cmd++;
 	count_pipe=i-1;
 	linea_cmd[i]=NULL;
 	parse_cmd();
}

void parse_cmd(){
	int j=0, k=1, i=0;
	char *comando;
	args = malloc(10*sizeof(char*));
	pipes[0]=0;
	while(linea_cmd[i]!=NULL){
		comando = strtok(linea_cmd[i], " \t");	
		while(comando!=NULL){
			args[j] = malloc((strlen(comando))*sizeof(char));
			strcpy(args[j], comando);
			j++;
			comando = strtok(NULL, " \t");
		}
		pipes[k]=j;
		i++;	
		k++;
	}
	args[j]=NULL;
}


struct rusage start, end;
long int utime, stime;

void comenzarMedicion(){
	getrusage(RUSAGE_CHILDREN, &start);
}

long int resultadoMedicion(){
	getrusage(RUSAGE_CHILDREN, &end);
	utime = (end.ru_utime.tv_sec - start.ru_utime.tv_sec)*1000000 + end.ru_utime.tv_usec -
	start.ru_utime.tv_usec;
	stime = (end.ru_stime.tv_sec - start.ru_stime.tv_sec)*1000000 + end.ru_stime.tv_usec -
	start.ru_stime.tv_usec;
    //printf("CPU user time: %lu microsec\n", utime);
	//printf("CPU system time: %lu microsec\n", stime);	
return utime;
}

void ejecutar_cmd(){
 
	int pid=0, i=0;
 	int status;
   	pid=fork(); 
   	if(pid<0) printf("Error! no se pudo crear un proceso hijo");
   	if (pid==0){
  	  	status=execvp(linea_cmd[i],args); 
     	if(status){
 			printf("Error! %s no se reconoce o no se pudo ejecutar\n",linea_cmd[i]);
 	    	 exit(1);
      	}
   	}else{
  	   wait(NULL);
	}
}

 void ejecutar_cmd2(){

 	int p[2];
 	int status;
 	int i;
 	pipe(p);
 	char *a1[10], *a2[10];
 	pid_t pid1;
 	pid_t pid2;
 
 	for(i=0; i<pipes[1]; i++){
 	 	a1[i]=args[i];
 	}
 	pid1 = fork();
 	if(pid1 == -1){
 		printf("Error al ejecutar proceso");
 		exit(1);
 	}else if (pid1 == 0) {
		dup2(p[1], STDOUT_FILENO);

		close(p[0]);
    	close(p[1]);

		status = execvp(a1[0],a1);
    	if (status != 0){
			printf("Error ejecutando proceso %s, puede que no sea un comando valido\n", a1[0]);
			exit(1);
		}
    } else if(pid1>0){
		pid2 = fork();		
 		if (pid2 == -1){
			printf("Error al crear proceso");
			exit(1);
		}else if (pid2 == 0){
	  	    dup2(p[0], STDIN_FILENO);

	        close(p[0]); 
	        close(p[1]);
            status = execvp(args[pipes[1]],args + pipes[1]);

	     	if (status != 0){
				printf("Error ejecutando proceso %s, puede que no sea un comando valido\n", a2[0]);
				exit(1);
			}
		} else if(pid2>0){	
 			wait(&pid2);
	        close(p[0]); 
	        close(p[1]);
 		}
 		wait(&pid1);
    } 
}

 void ejecutar_cmd3(){
    
 	int p[2], p2[2];
 	int status;
 	int i,k;
 	pipe(p);
 	pipe(p2);
 	char *a1[10], *a2[10], *a3[10];
 	pid_t pid1;
 	pid_t pid2;
 	pid_t pid3;

 	for(i=pipes[0]; i<pipes[1]; i++){
 	 	a1[i]=args[i];
 	}
 	for(k=pipes[1]; k<pipes[2]; k++){
 		int n= k-pipes[1];
 		a2[n]=args[k];
 	}

 	pid1 = fork();
	if(pid1 == -1){
		printf("Error al ejecutar proceso");
 		exit(1);
	}else if (pid1 == 0) {
		dup2(p[1], STDOUT_FILENO);

		close(p[0]);
    	close(p[1]);

		status = execvp(a1[0],a1);
    	if (status != 0){
			printf("Error ejecutando proceso %s, puede que no sea un comando valido\n", a1[0]);
			exit(1);
		}	
    } else if(pid1>0){
		pid2 = fork();		
 		if (pid2 == -1){
			printf("Error al ejecutar proceso");
 			exit(1);
		}else if (pid2 == 0){
	  	    dup2(p[0], STDIN_FILENO); 
			dup2(p2[1], STDOUT_FILENO);
		  	
		  	close(p[0]); 
		    close(p[1]);
		    
		    status = execvp(a2[0],a2);
			if (status != 0){
				printf("Error ejecutando proceso %s, puede que no sea un comando valido\n", a2[0]);
				exit(1);
			}	
		} else if(pid2>0){	
			pid3 = fork();
			if(pid3 == -1){
				printf("Error al ejecutar proceso");
 				exit(1);
			}else if(pid3 == 0){
				dup2(p2[0], STDIN_FILENO);
			
				close(p[0]);
				close(p[1]);
				close(p2[0]);
				close(p2[1]);
				
				status = execvp(args[pipes[2]], args + pipes[2]);
				if (status != 0){
					printf("Error ejecutando proceso %s, puede que no sea un comando valido\n", a3[0]);
					exit(1);
				}
			}else if(pid3>0){
				wait(&pid3);
			
				close(p[0]);
				close(p[1]);
				close(p2[0]);
				close(p2[1]);
			}
 			wait(&pid2);
 		}
 		wait(&pid1);   
    }
}

void ejec(char *str){

    ejecutarHist = 0;
    find_pipe(str);
    comenzarMedicion();

	if(count_pipe==0){
		ejecutar_cmd();       
	}
    else if(count_pipe==1){            
	    ejecutar_cmd2();
	}
    else if(count_pipe==2){
		ejecutar_cmd3();
	}
    else{
    	printf("No soporto mas de 3 comandos :( \n");
	}
	int us = resultadoMedicion();

	historial[his_count-1]->time = us;
}

int ejecutarDeHistorial(char* str){
    int i;
    printf("%s\n", str);
    for(i = 0; i < strlen(str); i++){
        if(str[i] < '0' || str[i] > '9')
            return -1;    
    }
    return atoi(str);
}


int main(){
	while(1){
		printf("Shell-KDJ:~$ ");
		char str[100];
		fgets (str, 100, stdin);
		linea_cmd = malloc(10*sizeof(char*));
		char *h;
		h = strtok(str, "\n");
		if( strncmp(h, hist, 10)==0){
			historial_cmd();
            ejecutarHist = 1;
        }else{
            if(ejecutarHist == 1){
                int v = ejecutarDeHistorial(str);
                if(v >= 0 && v < his_count){
                    strcpy(str,historial[v]->linea);
                    ejec(str);
                }
                else{
                    ejec(str);		
                }   
            }else{
                 ejec(str);
            }
		}
	}
return 0;
}
