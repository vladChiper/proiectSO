#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commands.h"

int check_command(int argc, char ** argv){
    if (argc < 6) {
        fprintf(stderr,"Nu ai trimis destule argumente.\n");
        return 0;
    }
    if(strcmp(argv[1], "--role") != 0 ){
        fprintf(stderr,"Role lipsa\n"); return 0;
    }
    if(strcmp(argv[2], "inspector") != 0 && strcmp(argv[2], "manager") != 0){
        fprintf(stderr,"Rol Gresit\n"); return 0;
    }
    if(strcmp(argv[3], "--user") != 0 ){
        fprintf(stderr,"User lipsa\n"); return 0;
    }
    
    if(strcmp(argv[5], "--add") == 0 && argc >= 7) return 1;
    if(strcmp(argv[5], "--list") == 0 && argc >= 7) return 1;
    if(strcmp(argv[5], "--view") == 0 && argc >= 8) return 1;
    if(strcmp(argv[5], "--remove_report") == 0 && argc >= 8) return 1;
    if(strcmp(argv[5], "--update_threshold") == 0 && argc >= 8) return 1;
    if(strcmp(argv[5], "--filter") == 0 && argc >= 8) return 1;

    fprintf(stderr, "Comanda specificata gresit.\n");
    return 0;
}

int main(int argc, char *argv[]) {
    char role[20]="";
    char user[50]="";
    char command[50]="";
    char district_id[50]="";
    char report_id[50]="";
    char value[50]="";
    char condition[50]="";

    if (check_command(argc, argv)) {
        for (int i=1; i<argc; i++) {
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
                if(i+1<argc) strcpy(report_id,argv[++i]);
            }
            else if(strcmp(argv[i], "--remove_report")== 0 && i+1<argc){
                strcpy(command,"remove_report");
                strcpy(district_id,argv[++i]);
                if(i+1<argc) strcpy(report_id,argv[++i]);
            }
            else if(strcmp(argv[i], "--update_threshold")== 0 && i+1<argc){
                strcpy(command,"update_threshold");
                strcpy(district_id,argv[++i]);
                if(i+1<argc) strcpy(value,argv[++i]);
            }
            else if(strcmp(argv[i], "--filter")== 0 && i+1<argc){
                strcpy(command,"filter");
                strcpy(district_id,argv[++i]);
                if(i+1<argc) strcpy(condition,argv[++i]);
            }
        }
    } else {
        return -1; 
    }

    if(strcmp(command, "add") == 0) {
        add_report(role, user, district_id);
    }
    else if(strcmp(command, "list") == 0) {
        list_reports(role, user, district_id);
    }
    else if(strcmp(command, "view") == 0) {
        view_report(role, user, district_id, atoi(report_id));
    }
    else if(strcmp(command, "remove_report") == 0) {
        remove_report(role, user, district_id, atoi(report_id));
    }
    else if(strcmp(command, "update_threshold") == 0) {
        update_threshold(role, user, district_id, atoi(value));
    }
    else if(strcmp(command, "filter") == 0) {
        filter_reports(role, user, district_id, condition);
    }
    
    return 0;
}