#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

void mode_to_str(mode_t mode, char *str) {
    strcpy(str, "---------");
    if (mode & S_IRUSR) str[0] = 'r';
    if (mode & S_IWUSR) str[1] = 'w';
    if (mode & S_IXUSR) str[2] = 'x';
    if (mode & S_IRGRP) str[3] = 'r';
    if (mode & S_IWGRP) str[4] = 'w';
    if (mode & S_IXGRP) str[5] = 'x';
    if (mode & S_IROTH) str[6] = 'r';
    if (mode & S_IWOTH) str[7] = 'w';
    if (mode & S_IXOTH) str[8] = 'x';
}

// Înregistrarea operațiunilor 
void log_action(const char *district, const char *role, const char *user, const char *action) {
    char path[256];
    sprintf(path, "%s/logged_district", district);
    
    if (strcmp(role, "inspector") == 0) {
        struct stat st;
        if (stat(path, &st) == 0 && !(st.st_mode & S_IWGRP)) {
            fprintf(stderr, "Permission denied: Inspector nu poate scrie in log.\n");
            return;
        }
    }

    FILE *f = fopen(path, "a");
    if (f) {
        time_t now = time(NULL);
        fprintf(f, "[%ld] User: %s, Role: %s, Action: %s\n", now, user, role, action);
        fclose(f);
        chmod(path, 0644); // Forțăm permisiunile la rw-r--r--
    }
}

// Inițializare structură director district (dacă nu există)
void init_district(const char *district) {
    struct stat st = {0};
    if (stat(district, &st) == -1) {
        mkdir(district, 0750); // Permisiuni pentru director
    }
    
    char path[256];
    sprintf(path, "active_reports-%s", district);
    if (lstat(path, &st) == -1) { // Lstat folosit pentru symlinks
        char target[256];
        sprintf(target, "%s/reports.dat", district);
        symlink(target, path);
    }
}

// Comanda --add
void add_report(const char *role, const char *user, const char *district) {
    init_district(district);
    
    char path[256];
    sprintf(path, "%s/reports.dat", district);
    
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0664);
    if (fd < 0) {
        perror("Eroare la deschiderea reports.dat");
        return;
    }
    chmod(path, 0664); 

    Report r;

    r.id = rand() % 10000; 
    strncpy(r.inspector, user, 49); 
    r.inspector[49] = '\0';

    const char* categorii[] = {"road", "lighting", "sewage", "vegetation", "waste"};
    const char* descrieri[] = {"Gropi in carosabil", "Bec ars", "Scurgere blocata", "Copac cazut", "Gunoi depozitat ilegal"};

    r.lat = 45.0 + (float)(rand() % 1000) / 1000.0; 
    r.lon = 21.0 + (float)(rand() % 1000) / 1000.0;

    strcpy(r.category, categorii[rand() % 5]);
    r.severity = (rand() % 5) + 1;
    r.timestamp = time(NULL);
    strcpy(r.description, descrieri[rand() % 5]);

    // Scrierea in fisier
    write(fd, &r, sizeof(Report));
    close(fd);

    log_action(district, role, user, "Added random report");
    printf("Raport %d (%s) adaugat cu succes in %s.\n", r.id, r.category, district);
}

// Comanda --list
void list_reports(const char *role, const char *user, const char *district) {
    char path[256];
    sprintf(path, "%s/reports.dat", district);
    
    struct stat st;
    if (stat(path, &st) < 0) {
        perror("Eroare (districtul poate nu exista)");
        return;
    }
    
    if (strcmp(role, "inspector") == 0 && !(st.st_mode & S_IRGRP)) {
        fprintf(stderr, "Acces interzis pentru inspector!\n"); return;
    }
    if (strcmp(role, "manager") == 0 && !(st.st_mode & S_IRUSR)) {
        fprintf(stderr, "Acces interzis pentru manager!\n"); return;
    }

    char perm_str[10];
    mode_to_str(st.st_mode, perm_str);
    printf("Informatii Fisier: %s | Dimensiune: %ld bytes | Ultima modificare: %s", perm_str, st.st_size, ctime(&st.st_mtime));

    int fd = open(path, O_RDONLY);
    if (fd < 0) return;

    Report r;
    printf("\n--- Rapoarte in districtul %s ---\n", district);
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        printf("ID: %d | Inspector: %s | Categorie: %s | Severitate: %d\n", r.id, r.inspector, r.category, r.severity);
    }
    close(fd);
    log_action(district, role, user, "Listed reports");
}

// Comanda --view
void view_report(const char *role, const char *user, const char *district, int report_id) {
    char path[256];
    sprintf(path, "%s/reports.dat", district);
    
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("Eroare deschidere fisier");
        return;
    }

    Report r;
    int found = 0;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (r.id == report_id) {
            printf("\n--- Detalii Raport %d ---\nInspector: %s\nCoordonate: %.2f, %.2f\nCategorie: %s\nSeveritate: %d\nDescriere: %s\nTimestamp: %ld\n",
                r.id, r.inspector, r.lat, r.lon, r.category, r.severity, r.description, r.timestamp);
            found = 1;
            break;
        }
    }
    close(fd);
    if(!found) printf("Raportul %d nu a fost gasit.\n", report_id);
    log_action(district, role, user, "Viewed report");
}

// Comanda --remove_report
void remove_report(const char *role, const char *user, const char *district, int report_id) {
    if (strcmp(role, "manager") != 0) {
        fprintf(stderr, "Eroare: Doar rolul de manager poate sterge rapoarte.\n");
        return;
    }

    char path[256];
    sprintf(path, "%s/reports.dat", district);
    
    int fd = open(path, O_RDWR);
    if (fd < 0) { perror("Eroare"); return; }

    off_t read_pos = 0;
    off_t write_pos = 0;
    Report r;
    int found = 0;
    
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        read_pos += sizeof(Report);
        if (r.id == report_id) {
            found = 1;
            continue; // Sarim peste record-ul ce trebuie sters
        }
        
        if (found) {
            lseek(fd, write_pos, SEEK_SET);
            write(fd, &r, sizeof(Report));
            lseek(fd, read_pos, SEEK_SET); // Restabilim pozitia citirii
        }
        write_pos += sizeof(Report);
    }
    
    if (found) {
        ftruncate(fd, write_pos); // Trunchiem fisierul la final
        printf("Raportul %d a fost sters. Dimensiune noua fisier: %ld\n", report_id, write_pos);
        log_action(district, role, user, "Removed report");
    } else {
        printf("Raportul cu ID %d nu a fost gasit.\n", report_id);
    }
    close(fd);
}

// Comanda --update_threshold
void update_threshold(const char *role, const char *user, const char *district, int value) {
    if (strcmp(role, "manager") != 0) {
        fprintf(stderr, "Eroare: Doar managerii pot actualiza pragul de severitate.\n");
        return;
    }
    char path[256];
    sprintf(path, "%s/district.cfg", district);
    
    struct stat st;
    if (stat(path, &st) == 0 && (st.st_mode & 0777) != 0640) {
        fprintf(stderr, "Eroare permisiuni cfg. Refuzare operatie!\n");
        return;
    }

    FILE *f = fopen(path, "w");
    if (!f) { perror("Eroare cfg"); return; }
    
    fprintf(f, "THRESHOLD=%d\n", value);
    fclose(f);
    chmod(path, 0640);
    printf("Prag setat la %d pentru districtul %s.\n", value, district);
    log_action(district, role, user, "Updated threshold");
}

// --- Functii AI Generate pentru implementare Filtru ---
int parse_condition(const char *input, char *field, char *op, char *value) {
    char temp[256];
    strncpy(temp, input, 255);
    temp[255] = '\0';
    
    char *colon1 = strchr(temp, ':');
    if(!colon1) return 0;
    *colon1 = '\0';
    strcpy(field, temp);
    
    char *colon2 = strchr(colon1 + 1, ':');
    if(!colon2) return 0;
    *colon2 = '\0';
    strcpy(op, colon1 + 1);
    strcpy(value, colon2 + 1);
    return 1;
}

int match_condition(Report *r, const char *field, const char *op, const char *value) {
    if (strcmp(field, "severity") == 0) {
        int v = atoi(value);
        if (strcmp(op, "==") == 0) return r->severity == v;
        if (strcmp(op, "!=") == 0) return r->severity != v;
        if (strcmp(op, "<") == 0) return r->severity < v;
        if (strcmp(op, "<=") == 0) return r->severity <= v;
        if (strcmp(op, ">") == 0) return r->severity > v;
        if (strcmp(op, ">=") == 0) return r->severity >= v;
    } else if (strcmp(field, "category") == 0) {
        if (strcmp(op, "==") == 0) return strcmp(r->category, value) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(r->category, value) != 0;
    }
    return 0;
}

// Comanda --filter
void filter_reports(const char *role, const char *user, const char *district, const char *condition) {
    char path[256];
    sprintf(path, "%s/reports.dat", district);
    
    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror("Eroare"); return; }

    char field[50], op[10], val[50];
    if (!parse_condition(condition, field, op, val)) {
        fprintf(stderr, "Format conditie invalid. Asteptat: field:operator:value\n");
        close(fd);
        return;
    }

    Report r;
    printf("\n--- Rezultate Filtru in %s ---\n", district);
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (match_condition(&r, field, op, val)) {
            printf("ID: %d | Inspector: %s | Categorie: %s | Severitate: %d\n", r.id, r.inspector, r.category, r.severity);
        }
    }
    close(fd);
    log_action(district, role, user, "Filtered reports");
}