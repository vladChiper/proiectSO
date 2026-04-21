#ifndef COMMANDS_H
#define COMMANDS_H

#include <time.h>

typedef struct {
    int id;
    char inspector[50];
    float lat;
    float lon;
    char category[20];
    int severity;
    time_t timestamp;   
    char description[100];
} Report;

void add_report(const char *role, const char *user, const char *district);
void list_reports(const char *role, const char *user, const char *district);
void view_report(const char *role, const char *user, const char *district, int report_id);
void remove_report(const char *role, const char *user, const char *district, int report_id);
void update_threshold(const char *role, const char *user, const char *district, int value);
void filter_reports(const char *role, const char *user, const char *district, const char *condition);

#endif