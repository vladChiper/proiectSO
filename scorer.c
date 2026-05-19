#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "commands.h"

typedef struct {
    char name[50];
    int score;
} InspectorScore;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Utilizare: ./scorer <district_id>\n");
        return 1;
    }
    
    char *district = argv[1];
    char path[256];
    sprintf(path, "%s/reports.dat", district);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("District [%s] | Eroare: Nu exista rapoarte sau districtul este invalid.\n", district);
        return 1;
    }

    InspectorScore scores[100];
    int count = 0;
    Report r;

    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        int found = 0;
        for (int i = 0; i < count; i++) {
            if (strcmp(scores[i].name, r.inspector) == 0) {
                scores[i].score += r.severity;
                found = 1; 
                break;
            }
        }
        if (!found && count < 100) {
            strcpy(scores[count].name, r.inspector);
            scores[count].score = r.severity; // 
            count++;
        }
    }
    close(fd);

    for(int i = 0; i < count; i++) {
        printf("District [%s] | Inspector: %s | Workload Score: %d\n", district, scores[i].name, scores[i].score);
    }
    
    return 0;
}