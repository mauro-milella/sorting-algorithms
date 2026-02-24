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

        // remove trailing newline
        conf_path[strcspn(conf_path, "\n")] = '\0';
    }

    struct configuration configuration;
    config_init(&configuration); 
    config_load(conf_path, &configuration);

    printf("%ld %ld\n", configuration.min_size, configuration.max_size);

    return EXIT_SUCCESS;
}

