#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define CONF_PATH_BUF 128


int main(int argc, char *argv[])
{
    char conf_path[CONF_PATH_BUF];

    // if the user provided a string, use it as the configuration relative path 
    if (argc > 1) {
        snprintf(conf_path, sizeof(conf_path), "%s", argv[1]);
    } 
    // otherwise, ask for it
    else {
        printf("Enter the : ");
        if (fgets(conf_path, sizeof(conf_path), stdin) == NULL) {
            fprintf(stderr, "Error reading conf_filename.\n");
            return EXIT_FAILURE;
        }

        // Remove trailing newline if present
        conf_path[strcspn(conf_path, "\n")] = '\0';
    }

    // Example: Try opening the file
    FILE *file = fopen(conf_path, "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

