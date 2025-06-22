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
        // TODO: permisos
        const char *filename = argv[i];

        // Verificar si el nombre de archivo es válido
        if (!name_is_valid(filename)) {
            return EXIT_FAILURE;
        }
        // Verificar si el archivo ya existe
        if (dir_lookup(image_path, filename) != 0) {
            fprintf(stderr, "Error: El archivo '%s' ya existe en el sistema de archivos.\n", filename);
            return EXIT_FAILURE;
        }

        // Crear nodo-I vacío
        int new_inode = create_empty_file_in_free_inode(image_path, DEFAULT_PERM);
        if (new_inode < 0) {
            fprintf(stderr, "Error al crear archivo destino en VFS\n");
            return EXIT_FAILURE;
        }
        
        // Agregar entrada al directorio raíz
        if (add_dir_entry(image_path, filename, new_inode) != 0) {
            fprintf(stderr, "Error al agregar entrada de directorio para %s\n", filename);
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;

}