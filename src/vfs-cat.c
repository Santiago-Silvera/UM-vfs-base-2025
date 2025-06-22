#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "../include/vfs.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s imagen archivo1 [archivo2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];

    // Leer el directorio raíz
    struct inode root_inode;
    if (read_inode(image_path, ROOTDIR_INODE, &root_inode) != 0) {
        fprintf(stderr, "Error al leer directorio raíz\n");
        return EXIT_FAILURE;
    }

    for (int i = 2; i < argc; i++) {
        const char *filename = argv[i];

        if (!name_is_valid(filename)) {
            fprintf(stderr, "Error: El nombre de archivo '%s' no es válido.\n", filename);
            return EXIT_FAILURE;
        }
        int inode = dir_lookup(image_path, filename);
        if (inode == 0) {
            fprintf(stderr, "Error: El archivo '%s' no existe en el sistema de archivos.\n", filename);
            return EXIT_FAILURE;
        }

        void *buffer = malloc(BLOCK_SIZE);
        if (inode_read_data(image_path, inode, buffer, BLOCK_SIZE, 0) == -1) {
            fprintf(stderr, "Error al leer datos del archivo %s\n", filename);
            free(buffer);
            return EXIT_FAILURE;
        }
        printf("%s\n", (char *)buffer);
        free(buffer);
    }

    return EXIT_SUCCESS;
}