#include <stdio.h>
#include <stdlib.h>
#include "../include/vfs.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s imagen archivo1 [archivo2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];
    // Verificar que la imagen sea valida
    struct superblock sb_struct, *sb = &sb_struct;
    
    if (read_superblock(image_path, sb) != 0) {
        fprintf(stderr, "Error al leer superblock\n");
        return EXIT_FAILURE;
    }

    for (int i = 2; i < argc; i++) {
        const char *filename = argv[i];

        // Verificar si el nombre de archivo es válido
        if (!name_is_valid(filename)) {
            return EXIT_FAILURE;
        }
        // Verificar si el archivo existe
        if (dir_lookup(image_path, filename) == 0) {
            fprintf(stderr, "Error: El archivo '%s' no existe en el sistema de archivos.\n", filename);
            return EXIT_FAILURE;
        }

        if (remove_dir_entry(image_path, filename) != 0) {
            fprintf(stderr, "Error al eliminar el archivo %s\n", filename);
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;

}