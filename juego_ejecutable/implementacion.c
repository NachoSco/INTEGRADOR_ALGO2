#include <stdio.h>
#include "menu.h"

int main() {
    menu_t* menu = menu_crear();
    if (menu == NULL) {
        return EXIT_FAILURE;
    }
    inicializar_menu(menu);

    char comando[LONGITUD_MAX_COMANDO];
    while (1) {
        printf("Ingrese un comando (h para ayuda): ");
        scanf("%9s", comando);
        procesar_comando(menu, comando);
    }

    liberar_menu(menu);
    return 0;
}