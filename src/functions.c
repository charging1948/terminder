//
// Created by jk on 2/26/23.
//
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/functions.h"
#include "../include/utility.h"

/**
 * @brief Creates empty list and returns it.
 *
 * This function creates a list with only two elements head and tail
 * that function as start and end of the list.
 *
 * @return created list.
 */
List createList() {
    List result;

    Element *head = malloc(sizeof(Element));
    Element *tail = malloc(sizeof(Element));

    head->appointment = tail->appointment = NULL;

    head->next = tail->next = tail;

    result.head = head;
    result.tail = tail;

    return result;
}
/**
 * @brief Clears all elements from list.
 *
 * This function clears the whole list leaving only head and tail.
 *
 * @param list The list to be cleared.
 */
void clearList(List list) {
    Element *current = list.head->next;
    while (current != list.tail) {
        Element *old = current;
        current = current->next;
        free((char *)old->appointment->description);
        free(old->appointment);
        free(old);
    }
    list.head->next = list.tail;
}
/**
 * @brief Inserts new element inside list chronological.
 *
 * This function inserts a new element in a list chronological. If
 * there are only two elements in the list (head and tail) the new element
 * is inserted after head.
 *
 * @param list The list in which the element is inserted.
 * @param start The timestamp of the new element.
 * @param text The Description of the new element.
 */
void insertElement(List list, time_t start, const char *text) {
    Appointment *appointment = (Appointment *)malloc(sizeof(Appointment));
    appointment->description = text;
    appointment->start = start;

    struct Element *element = (struct Element *)malloc(sizeof(struct Element));
    element->appointment = appointment;

    Element *current = list.head;
    // Find the correct position to insert (chronologically)
    while (current->next != list.tail) {
        if (difftime(start, current->next->appointment->start) <= 0) {
            // Insert before current->next
            element->next = current->next;
            current->next = element;
            return;
        }
        current = current->next;
    }
    // Insert at the end (before tail)
    element->next = list.tail;
    current->next = element;
}
/**
 * @brief Searches list for an element and returns it if found.
 *
 * This function searches for an element in the list based on a
 * user-entered text.
 *
 * @param list The list to be searched through.
 * @param text The text to be searched for.
 * @return The Element if there is a match, else NULL.
 */
Element *findElement(List list, const char *text) {
    Element *current = list.head->next;
    while (current != list.tail) {
        const char *desc = current->appointment->description;
        if (strcmp(desc, text) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}
/**
 * @brief Deletes Element from list using it's description.
 *
 * This function deletes an element from the list. The element to be destroyed is determined by
 * comparing a user-entered string to the description of each element.
 *
 * @param list List of appointments.
 * @param text Description of appointment within the element to be destroyed.
 * @return true, if the element successfully got deleted, false if nothing was deleted.
 */
bool deleteElement(List list, const char *text) {
    Element *current = list.head;
    while (current->next != list.tail) {
        const char *desc = current->next->appointment->description;
        if (strcmp(desc, text) == 0) {
            Element *old = current->next;
            current->next = old->next;
            free((char *)old->appointment->description);
            free(old->appointment);
            free(old);
            return true;
        }
        current = current->next;
    }
    return false;
}
/**
 * @brief Prints appointment data to command line.
 *
 * This function prints out all data of an appointment.
 *
 * @param appointment The appointment to print.
 */
void printAppointment(Appointment *appointment) {
    printf("--------------------------------------\n");
    printf("Appointment: %s\n", appointment->description);
    printf("Faellig am: %s\n", dateToString(appointment->start));
    printf("--------------------------------------\n");
}
/**
 * @brief Prints list of all appointments on a user entered date.
 *
 * This function prints all appointments on a user-entered date. If all
 * time parameters are equal to 0 the full list of all appointments is printed.
 * If it can't find any appointment on the entered date a corresponding message will be printed.
 *
 * @param list The list to print.
 * @param day Day of wanted date.
 * @param month Month of wanted date.
 * @param year Year of wanted date.
 */
void printList(List list, int day, int month, int year) {
    bool ignoreDate = day == 0 && month == 0 && year == 0;
    struct Element *ptr = list.head->next;
    bool found = false;

    while (ptr != list.tail) {
        struct tm *start = localtime(&ptr->appointment->start);
        if ((start->tm_mday == day && start->tm_mon == month &&
            start->tm_year == year) || ignoreDate) {
            printAppointment(ptr->appointment);
            found = true;
        }
        ptr = ptr->next;
    }
    if (!found) printf("Keine Termine gefunden!\n");
}

/**
 * @brief Prints list.
 *
 * This function prints out all appointments in a list using the printList-Method.
 *
 * @param list The list to print.
 */
void printFullList(List list) {
    printList(list, 0,0,0);
}
/**
 * @brief Converts input from a file to a list and returns this list.
 *
 * Opens the file with the given filename in read mode, and converts the
 * data of the file to a list of appointments. The data has to be in following format:
 *
 *     description|start_time;\n
 *
 * where description is the description of the appointment, start_time is
 * the start time of the appointment as a long integer, and ';' and '\n' are
 * used as delimiters. The function will create an empty list if the file
 * cannot be opened. All overdue appointments will not be added to the list.
 *
 * @param filename Name of input file.
 * @return Instance of list structure with appointments from specified file.
 */
List parseFile(const char *filename) {
    // read appointments from file saved in "filename" by saveList
    FILE *file = fopen(filename, "r");
    List list = createList();
    if (file == NULL) {
        printf("Datei '%s' nicht gefunden. Neue Liste wird erstellt.\n", filename);
        return list;
    }
    printf("Datei '%s' wird zum Speichern benutzt.\n", filename);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
        char *token = strtok(line, "|");
        char *description = strdup(token);
        token = strtok(NULL, ";");
        time_t start = (time_t) strtol(token, NULL, 10);
        if (difftime(start, getResetTime()) >= 0)
            insertElement(list, start, description);
        else
            free(description);
    }
    free(line);
    fclose(file);
    return list;
}
/**
 * Saves the content of a list to a file.
 *
 * @param list The list to save.
 * @param filename The name of the file to save to.
 *
 * Opens the file with the given filename in write mode, and writes the
 * appointments in the list to the file in the following format:
 *
 *     description|start_time;\n
 *
 * where description is the description of the appointment, start_time is
 * the start time of the appointment as a long integer, and ';' and '\n' are
 * used as delimiters. The function will exit with an error message if the file
 * cannot be opened.
 */
void saveList(List list, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Fehler beim Oeffnen der Datei '%s'!\n", filename);
        exit(1);
    }
    Element *current = list.head->next;
    while (current != list.tail) {
        fprintf(file, "%s|", current->appointment->description);
        fprintf(file, "%ld;\n", current->appointment->start);
        current = current->next;
    }
    fclose(file);
}


/**
* @brief Displays entry message.
*
* This function displays an entry message to welcome the user. It also prints out the
* current version id.
*
*/
void entry() {
    printf("Terminder - Version 1.1\n");
}

/**
 * @brief Displays a menu of options and performs the selected action.
 *
 * This function displays a menu of options to the console and waits for the user to
 * select an option. If the user selects a valid option, the function performs the
 * corresponding action. If the user selects an invalid option, an error message is
 * printed and the menu is displayed again.
 *
 * @param list The list of appointments.
 */
void menu(List list) {
    int option = 0;
    while(true) {
        int input;
        printOptions();

        if (scanf("%d", &input) == 1 && input >= 1 && input <= 8) {
            option = input;
            clearBuffer();
        } else {
            clearBuffer();
            printf("Ungueltige Eingabe!\n");
            sleep(1);
            continue;
        }

        struct tm *ctm = getLocalTime();
        char *input_str = NULL;
        time_t date_time;
        switch (option) {
            case 1:
                printList(list, ctm->tm_mday, ctm->tm_mon, ctm->tm_year);
                break;
            case 2:
                date_time = readDate();
                struct tm *date_tm = localtime(&date_time);
                printList(list, date_tm->tm_mday, date_tm->tm_mon,
                          date_tm->tm_year);
                break;
            case 3:
                printFullList(list);
                break;
            case 4:
                printf("Neuer Termin:\n");
                input_str = readString();
                time_t start = readDate();
                insertElement(list, start, input_str);
                break;
            case 5:
                printf("Termin suchen:\n");
                input_str = readString();
                Element *element = findElement(list, input_str);
                if (element != NULL) {
                    printAppointment(element->appointment);
                } else {
                    printf("Termin nicht gefunden!\n");
                }
                break;
            case 6:
                printf("Termin loeschen:\n");
                input_str = readString();
                if (deleteElement(list, input_str)) {
                    printf("Termin geloescht!\n");
                } else {
                    printf("Termin nicht gefunden!\n");
                }
                break;
            case 7:
                clearList(list);
                printf("Liste geloescht!\n");
                break;
            case 8:
                // aus Schleife austreten, Programm beenden
                return;
        }
    }
}

/**
 * @brief Clears and frees the memory used by a list, and prints a farewell message.
 *
 * This function clears the contents of the given list by calling `clearList`,
 * and then frees the memory used by the list's head and tail nodes. It also
 * prints a farewell message to the console.
 *
 * @param list The list to clear and free.
 */
void goodbye(List list) {
    clearList(list);
    free(list.head);
    free(list.tail);

    printf("Bis bald!\n");
}