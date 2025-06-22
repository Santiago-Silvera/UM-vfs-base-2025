#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "../include/vfs.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s imagen\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];

    // Leer el directorio raíz
    struct inode root_inode;
    if (read_inode(image_path, ROOTDIR_INODE, &root_inode) != 0) {
        fprintf(stderr, "Error al leer directorio raíz\n");
        return EXIT_FAILURE;
    }

    // Leer los bloques de datos del directorio raíz
    uint8_t data_buf[BLOCK_SIZE];
    for (int i = 0; i < root_inode.blocks; i++) {
        int block_num = get_block_number_at(image_path, &root_inode, i);
        if (block_num < 0) {
            fprintf(stderr, "Error al obtener el número de bloque %d\n", i);
            return EXIT_FAILURE;
        }

        if (read_block(image_path, block_num, data_buf) != 0) {
            fprintf(stderr, "Error al leer bloque %d\n", block_num);
            return EXIT_FAILURE;
        }

        struct dir_entry *entries = (struct dir_entry *)data_buf;
        for (uint32_t j = 0; j < DIR_ENTRIES_PER_BLOCK; j++) {
            if (entries[j].inode != 0) {
                printf("%s\n", entries[j].name);
            }
        }
    }

    return EXIT_SUCCESS;
}