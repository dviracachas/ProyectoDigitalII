bool end_map = false;

// Suponemos que siempre se inicia en la coordenada 00 mirando hacia arriba
int current_x = 0;
int current_y = 0;
bool absolute_direction[4] = {0, 1, 0, 0}; // [ izq, arriba, der, abajo ]

// Ciclo hasta terminar el laberinto
while (!end_map) {
    // Iniciamos nuestros arreglos de información
    bool report[4] = {0, 0, 0, 0}; // Paredes en direcciones absolutas
    bool walls[4] = {0, 0, 0, 0};  // Paredes en dirección relativa [ izq, arriba, der, abajo ]
    char color[4] = {0, 0, 0, 0};  // Obstáculos (colores) en dirección absoluta (al final)
    
    // Adquirimos nuestros datos de la coordenada en dirección relativa
    rotate_servo(left);
    walls[0] = read_servo();
    color[0] = read_camera();

    rotate_servo(right);
    walls[2] = read_servo();
    color[2] = read_camera();
    
    rotate_servo(forward);
    walls[1] = read_servo();
    color[1] = read_camera();

    // Rotamos nuestros arreglos para obtener la información en direcciones
    // absolutas y las transmitimos

    for (int i = 0; i < 4; i++) {   // Igualamos Report a Walls
        report[i] = walls[i]
    }

    int absolute_direction_index = 0; // Identificamos la dirección 
    for (int i = 0; i < 4; i++) {
        if (absolute_direction[i] == 1) {
            absolute_direction_index = i;
            break;
        }
    }

    int next_x = current_x;
    int next_y = current_y;

    switch (absolute_direction_index) {
        case 0: // Apunta a la izquierda absoluta
            rotate_array_right(report);
            rotate_array_right(color);
            next_x--;
            break;

        case 1: // Apunta al arriba absoluto
            next_y++;
            break;

        case 2: // Apunta a la derecha absoluta
            rotate_array_left(report);
            rotate_array_left(color);
            next_x++;
            break;

        default: // Apunta al abajo absoluto
            rotate_array_right(report);
            rotate_array_right(report);
            rotate_array_right(color);
            rotate_array_right(color);
            next_y--;
            break;
        

    }

    printf("Coordenada %d%d:    ", current_x, current_y);
    printf("Paredes: %d%d%d%d   ", report[0], report[1], report[2], report[3]);
    printf("Color: %c%c%c%c\n", color[0], color[1], color[2], color[3]);

    // Con nuestra información relativa al carro, decidimos como avanzar
    int free_index = 0;
    for (int i = 0; i < 4; i++) {
        if (walls[i] == 0) {
            free_index = i;
            break;
        }
    }

    switch (free_index) {
        case 0: // Izquierda relativa está libre
            rotate_car(left);
            rotate_array_left(absolute_direction);
            forward();
            break;
        
        case 1: // Adelante relativo está libre
            forward();
            break;

        case 2: // Derecha relativa está libre
            rotate_car(right);
            rotate_array_right(absolute_direction);
            forward();
            break;

        default: // Solo atrás relativo está libre (Fin del laberinto)
            printf("Fin del recorrido\n")
            end_map = true;
            break;
    }

    //Actualiza nuestras coordeneadas para el siguiente ciclo.
    current_x = next_x;
    current_y = next_y;
    
}