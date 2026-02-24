#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define CONF_PATH_BUF 128


int main(int argc, char *argv[])
{
    char conf_path[CONF_PATH_BUF];

    // the path of the configuration must be specified in the command line
    if (argc == 2) {
        snprintf(conf_path, sizeof(conf_path), "%s", argv[1]);
    }
    else {
        printf("Usage: ./name <configuration_filepath>");
        return EXIT_FAILURE;
    }

    struct configuration configuration;
    config_init(&configuration); 
    config_load(conf_path, &configuration);

    printf("%ld %ld\n", configuration.min_size, configuration.max_size);

    return EXIT_SUCCESS;
}

