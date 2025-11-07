#include "../include/utility.h"
#include "../include/appointment.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

time_t getResetTime() {
    time_t current_time;
    struct tm *time_info;

    time(&current_time);
    time_info = gmtime(&current_time);

    // Set time fields to 0
    time_info->tm_hour = 0;
    time_info->tm_min = 0;
    time_info->tm_sec = 0;

    // Convert back to time_t
    return mktime(time_info);
}

struct tm *getLocalTime() {
    time_t t = time(NULL);
    return localtime(&t);
}

//Reads date input from command line
time_t readDate() {
    printf("Bitte geben Sie das Datum im Format TT.MM.JJJJ ein: ");
    char input[11];
    fgets(input, 11, stdin);
    int day, month, year;
    char* endptr;
    day = strtol(input, &endptr, 10);
    if (endptr == input || *endptr != '.') {
        printf("\nUngueltiges Eingabeformat.\n");
        return readDate();
    }

    month = strtol(endptr + 1, &endptr, 10);
    if (endptr == input || *endptr != '.') {
        // Case where the input is not in the format of 'dd.mm.yyyy'
        printf("\nUngueltiges Eingabeformat.\n");
        return readDate();
    }
    year = strtol(endptr + 1, &endptr, 10);
    if (endptr == input || *endptr != '\0') {
        // Case where the input is not in the format of 'dd.mm.yyyy'
        printf("\nUngueltiges Eingabeformat.\n");
        return readDate();
    }
    struct tm timeinfo = {0};
    timeinfo.tm_mday = day;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_hour = 0;
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;
    time_t result = mktime(&timeinfo);
    return result;
}

//Read String from command line
char *readString() {

    // read string from stdin
    char *input_str = malloc(500);
    fgets(input_str, 500, stdin);

    // remove trailing newline
    int i = 0;
    while (input_str[i] != '\n') {
        i++;
    }
    input_str[i] = '\0';

    return input_str;
}

// Print menu with options
void printOptions() {
    printf(" ------------------------------------------------------ \n");
    printf("| Bitte waehlen Sie eine der folgenden Funktionen aus: |\n");
    printf("| 1. Alle Termine (heute)                              |\n");
    printf("| 2. Alle Termine (bestimmter Tag)                     |\n");
    printf("| 3. Alle Termine (gesamt)                             |\n");
    printf("| 4. Neuen Termin erstellen                            |\n");
    printf("| 5. Termin suchen                                     |\n");
    printf("| 6. Termin loeschen                                   |\n");
    printf("| 7. Alle Termine loeschen                             |\n");
    printf("| 8. Programm beenden                                  |\n");
    printf(" ------------------------------------------------------ \n\n");
    printf("Auswahl: ");
}

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

char *dateToString(time_t date) {
    static char str[11]; // 10 characters + null terminator
    strftime(str, sizeof(str), "%d.%m.%Y", localtime(&date));
    return str;
}