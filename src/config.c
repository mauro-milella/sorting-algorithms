/* To know more about the TOML parsing library, refer to the following:
 * https://github.com/cktan/tomlc17?tab=readme-ov-file
 * https://github.com/cktan/tomlc17/blob/main/src/tomlc17.h
 */

#include "config.h"
#include "tomlc17.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void config_init(struct configuration *config) {
    memset(config->name, 0, sizeof(config->name));

    memset(config->algorithms, 0, sizeof(config->algorithms));
    config->algorithm_count = 0;

    config->repetitions = 0;

    config->min_size = 0;
    config->max_size = 0;

    config->seed = 1605;
}

/* Parse the string corresponding to `field` in `result`, whose dimension is 
 * `buf_size`, and load it in the `name` field within `config`.
 */
void _parse_name(
    toml_result_t result, 
    struct configuration *config,
    const char *field,
    size_t buf_size
) {
    toml_datum_t name = toml_seek(result.toptab, field); 
    if (name.type != TOML_STRING) {
        fprintf(stderr, "Missing or invalid name");
    }
    strncpy(config->name, name.u.s, buf_size);
    config->name[buf_size - 1] = '\0';
}

/* Similar to `_parse_name`, but fills the `.algorith_count` and `.algorithms` 
 * fields of the configuration `config`.
 */
void _parse_algorithms(
    toml_result_t result, 
    struct configuration *config,
    const char *field,
    size_t buf_size
) {
    toml_datum_t algorithms_datum = toml_seek(result.toptab, field);
    if (algorithms_datum.type != TOML_ARRAY) {
        fprintf(stderr, "Error: missing or invalid field %s in config\n", field);
    }
    config->algorithm_count = algorithms_datum.u.arr.size;

    for (int i = 0; i < algorithms_datum.u.arr.size; i++) {
        toml_datum_t elem = algorithms_datum.u.arr.elem[i];
        if (elem.type != TOML_STRING) {
            fprintf(stderr, "Error: 'algorithms' element %d is not a string\n", i);
        }
        strncpy(config->algorithms[i], elem.u.s, buf_size - 1);
        config->algorithms[i][buf_size - 1] = '\0';
    }
}

/* Parse the `field` of `result` as a signed long long integer, and return it.
 */
int64_t _get_int(toml_result_t result, const char *field) {
    toml_datum_t repetitions_datum = toml_seek(result.toptab, field);
    if (repetitions_datum.type != TOML_INT64) {
        fprintf(stderr, "Error: missing or invalid %s in config\n", field);
    }
    return repetitions_datum.u.int64;
}


/* Parse the provided TOML configuration file.
 * Return code 0 if everything went ok.
 */
int config_load(char *config_filename, struct configuration *config) {
    FILE *config_file = fopen(config_filename, "r");
    if (config_file == NULL) {
        perror("Error while opening configuration file");
    }

    toml_result_t result = toml_parse_file(config_file);
    if (!result.ok) {
        fprintf(stderr, "Error: %s\n", result.errmsg);
    }

    _parse_name(result, config, "general.name", NAME_BUF);

    _parse_algorithms(result, config, "algorithms.enabled", NAME_BUF);

    config->repetitions = (uint32_t)_get_int(result, "general.repetitions");

    config->min_size = (uint64_t)_get_int(result, "general.min_size");

    config->max_size = (uint64_t)_get_int(result, "general.max_size");

    config->seed = (uint64_t)_get_int(result, "general.seed");

    toml_free(result);

    if (fclose(config_file) != 0) {
        perror("Error while closing the configuration file");
    }

    return 0;
}

