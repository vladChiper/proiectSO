#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

typedef struct{
	int id;
	char inspector[50];
	float lat;
	float lon;
	char category[20];
	int severity;
	time_t timestamp;	
	char description[100];
}Report;


void add(const char *district,Report r){
	char path[256];
	sprintf (path,"%s/reports.dat",district);
	mkdir("district",0775);
}

int check_command(int argc, char ** argv){
    if(strcmp(argv[1], "--role") != 0 ){
        fprintf(stderr,"Role lipsa");
        return 0;
    }
    if(strcmp(argv[2], "inspector") != 0 && strcmp(argv[2], "manager") != 0){
        fprintf(stderr,"Rol Gresit");
        return 0;
    }
    if(strcmp(argv[3], "--user") != 0 ){
        fprintf(stderr,"User lipsa");
        return 0;
    }
    if(strcmp(argv[5], "--add") == 0 && argv[6]){
        return 1;
    }
    if(strcmp(argv[5], "--list") == 0 && argv[6]){
        return 1;
    }
    if(strcmp(argv[5], "--view") == 0 && argv[6] && argv[7]){
        return 1;
    }
    if(strcmp(argv[5], "--remove_report") == 0 && argv[6] && argv[7]){
        return 1;
    }
    if(strcmp(argv[5], "--update_threshold") == 0 && argv[6] && argv[7]){
        return 1;
    }
    if(strcmp(argv[5], "--filter") == 0 && argv[6] && argv[7]){
        return 1;
    }



    return 0;
}


int main(int argc,char *argv[]){
	char role[20]="";
	char user[50]="";
	char command[50]="";
	char district_id[50]="";
    char report_id[50]="";
    char value[50]="";
    char condition[50]="";

    if(check_command(argc, argv)){
        for (int i=1;i<argc;i++){
		if(strcmp(argv[i], "--role")== 0 && i+1<argc){
			strcpy(role,argv[++i]);
		}
		else if(strcmp(argv[i], "--user")== 0 && i+1<argc){
			strcpy(user,argv[++i]);
		}
		else if(strcmp(argv[i], "--add")== 0 && i+1<argc){
            strcpy(command,"add");
			strcpy(district_id,argv[++i]);
		}
        else if(strcmp(argv[i], "--list")== 0 && i+1<argc){
            strcpy(command,"list");
            strcpy(district_id,argv[++i]);
        }
        else if(strcmp(argv[i], "--view")== 0 && i+1<argc){
            strcpy(command,"view");
            strcpy(district_id,argv[++i]);
            strcpy(report_id,argv[++i]);
        }
        else if(strcmp(argv[i], "--remove_report")== 0 && i+1<argc){
            strcpy(command,"remove");
            strcpy(district_id,argv[++i]);
            strcpy(report_id,argv[++i]);
        }
        else if(strcmp(argv[i], "--update_threshold")== 0 && i+1<argc){
            strcpy(command,"update");
            strcpy(district_id,argv[++i]);
            strcpy(value,argv[++i]);
        }
        else if(strcmp(argv[i], "--filter")== 0 && i+1<argc){
            strcpy(command,"filter");
            strcpy(district_id,argv[++i]);
            strcpy(condition,argv[++i]);
        }
        
	    }
    }

	
	
    printf("Role: %s\nUser: %s\nCommand: %s\n", role, user, command);
			
	
	
	return 0;
}