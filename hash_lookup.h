#ifndef HASH_LOOKUP
#define HASH_LOOKUP

#include <cmph.h>
#include <stdlib.h>

typedef struct {
    cmph_t *hash;
    int *values, num;
    char *keys;
} hash_lookup;

hash_lookup hashlookup_build(char **keys, int *values, int num) {
    hash_lookup lookup;
    lookup.num = num;

    if (num > 1) {
        cmph_io_adapter_t *source = cmph_io_vector_adapter(keys, num);
        cmph_config_t *config = cmph_config_new(source);
        cmph_config_set_algo(config, CMPH_CHD);
        lookup.hash = cmph_new(config);
        cmph_config_destroy(config);
        lookup.keys = malloc(num * sizeof(char));
        lookup.values = malloc(num * sizeof(int));

        int i;
        unsigned int id;
        char *key;
        for (i = 0; i < num; i++) {
            key = keys[i];
            id = cmph_search(lookup.hash, key, 1);
            lookup.keys[id] = keys[i][0];
            lookup.values[id] = values[i];
        }
    } else if (num == 1) {
        lookup.keys = malloc(sizeof(char));
        lookup.keys[0] = keys[0][0];
        lookup.values = malloc(sizeof(int));
        lookup.values[0] = values[0];
    }

    return lookup;
}

int hashlookup_search(hash_lookup lookup, char key) {
    if (lookup.num == 0) return -1;
    if (lookup.num == 1) return (key == lookup.keys[0]) ? lookup.values[0] : -1;
    int id = cmph_search(lookup.hash, &key, 1);
    return ((id < lookup.num) && (key == lookup.keys[id])) ? lookup.values[id] : -1;
}

void hashlookup_free(hash_lookup *lookup) {
    if (lookup->num > 0) {
        if (lookup->num > 1) cmph_destroy(lookup->hash);
        free(lookup->values);
        free(lookup->keys);
    }
}

#endif