#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//  CONSTANTES DEL JUEGO 
#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 500

#define GRID_OFFSET_X 220
#define GRID_OFFSET_Y 59
#define GRID_WIDTH 650
#define GRID_HEIGHT 425

#define GRID_ROWS 5
#define GRID_COLS 9
#define CELL_WIDTH (GRID_WIDTH / GRID_COLS)
#define CELL_HEIGHT (GRID_HEIGHT / GRID_ROWS)

#define PEASHOOTER_FRAME_WIDTH 177
#define PEASHOOTER_FRAME_HEIGHT 166
#define PEASHOOTER_TOTAL_FRAMES 31
#define PEASHOOTER_ANIMATION_SPEED 4
#define PEASHOOTER_SHOOT_FRAME 18

#define ZOMBIE_FRAME_WIDTH 164
#define ZOMBIE_FRAME_HEIGHT 203
#define ZOMBIE_TOTAL_FRAMES 90
#define ZOMBIE_ANIMATION_SPEED 2
#define ZOMBIE_DISTANCE_PER_CYCLE 40.0f

#define MAX_ARVEJAS 100
#define PEA_SPEED 5
#define ZOMBIE_SPAWN_RATE 300


//  ESTRUCTURAS DE DATOS 
typedef struct {
    int row, col;
} Cursor;

typedef struct {
    SDL_Rect rect;
    int activo;
    int cooldown;
    int current_frame;
    int frame_timer;
    int debe_disparar;
} Planta;

typedef struct {
    SDL_Rect rect;
    int activo;
} Arveja;

typedef struct {
    SDL_Rect rect;
    int activo;
    int vida;
    int row;
    int current_frame;
    int frame_timer;
    float pos_x;
} Zombie;

//  NUEVAS ESTRUCTURAS 
#define STATUS_VACIO 0
#define STATUS_PLANTA 1

typedef struct RowSegment {
    int status;
    int start_col;
    int length;
    Planta* planta_data;
    struct RowSegment* next;
} RowSegment;

typedef struct ZombieNode {
    Zombie zombie_data;
    struct ZombieNode* next;
} ZombieNode;

typedef struct GardenRow {
    RowSegment* first_segment;
    ZombieNode* first_zombie;
} GardenRow;

typedef struct GameBoard {
    GardenRow rows[GRID_ROWS];
    Arveja arvejas[MAX_ARVEJAS]; //array adicional para manejar las arvejas
    int zombie_spawn_timer; // variable para saber cada cuanto crear un zombie
} GameBoard;


//  VARIABLES GLOBALES 
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* tex_background = NULL;
SDL_Texture* tex_peashooter_sheet = NULL;
SDL_Texture* tex_zombie_sheet = NULL;
SDL_Texture* tex_pea = NULL;

Cursor cursor = {0, 0};
GameBoard* game_board = NULL;

//  FUNCIONES 

GameBoard* gameBoardNew() {
    GameBoard* board = (GameBoard*)malloc(sizeof(GameBoard));
    if (!board) return NULL;

    board->zombie_spawn_timer = ZOMBIE_SPAWN_RATE;

    for (int i = 0; i < GRID_ROWS; i++) {
        RowSegment* first = (RowSegment*)malloc(sizeof(RowSegment));
        if (!first) {
            free(board);
            return NULL;
        }
        first->status = STATUS_VACIO;
        first->start_col = 0;
        first->length = GRID_COLS;
        first->planta_data = NULL;
        first->next = NULL;

        board->rows[i].first_segment = first;
        board->rows[i].first_zombie = NULL;
    }
     for(int i = 0; i < MAX_ARVEJAS; i++) {
        board->arvejas[i].activo = 0;
    }
    return board;
}

void gameBoardDelete(GameBoard* board) {
    // TODO: Liberar toda la memoria dinámica.
    for (int i = 0; i < GRID_ROWS; i++) { // TODO: Recorrer cada GardenRow.
        // TODO: Liberar todos los RowSegment (y los planta_data si existen).
        RowSegment* mySegment = board->rows[i].first_segment;
        while (mySegment != NULL) {
            if (mySegment->planta_data != NULL) {
                free(mySegment->planta_data);
            }

            RowSegment* nextSegment = mySegment->next;
            free(mySegment);
            mySegment = nextSegment;   
        }
        
        // TODO: Liberar todos los ZombieNode.
        ZombieNode* zombieNode = board->rows[i].first_zombie;
        while (zombieNode != NULL) {
            ZombieNode* nextZombie = zombieNode->next;
            free(zombieNode);
            zombieNode = nextZombie;
        }
    }
    
    free(board); // TODO: Finalmente, liberar el GameBoard.
}

void gameBoardRemovePlant(GameBoard* board, int row, int col) {
    /* 
        Disclaimer: Por la forma en la que implementamos esta función tomamos la precaucion
        de inicializarla antes que gameBoardAddPlant.
        De esta forma podríamos llamar a gameBoardRemovePlant desde el AddPlant si
        llegamos a un (row, col) ocupado.
    */

    RowSegment* mySegment = board->rows[row].first_segment;
    RowSegment* prevSegment = NULL; // Me va a servir para hacer la fusión con el anterior
    
    while (mySegment != NULL) {
        // TODO: Encontrar el segmento que contiene `col`.
        // If (la columna que busco se encuentra en el rango que necesito)...
        if (mySegment->start_col <= col && col < (mySegment->start_col + mySegment->length)) {
            // ¿Estamos seguros de que es un segmento ocupado por una planta?
            if (mySegment->status != STATUS_PLANTA) {
                printf("No hay planta en esta celda para remover.\n");
                return;
            }
            
            // TODO: Si es un segmento de tipo PLANTA, convertirlo a VACIO y liberar el `planta_data`.
            if (mySegment->planta_data != NULL) {
                free(mySegment->planta_data);
                mySegment->planta_data = NULL;
            }
            
            mySegment->status = STATUS_VACIO;
            
            // Intento hacer la fusión con el siguiente segmento si esta vacio
            if (mySegment->next != NULL && mySegment->next->status == STATUS_VACIO) {
                RowSegment* next_seg = mySegment->next;
                mySegment->length += next_seg->length;
                mySegment->next = next_seg->next;
                free(next_seg);
            }
            
            // Intento hacer la fusión con el segmento anterior si esta vacio
            if (prevSegment != NULL && prevSegment->status == STATUS_VACIO) {
                prevSegment->length += mySegment->length;
                prevSegment->next = mySegment->next;
                free(mySegment);
                mySegment = prevSegment;
            }
            break; // Si "terminé mi cometido" termino el loop, no busco más nada
        }
        
        prevSegment = mySegment; // Cuando avanzo 1, mi "segmento anterior" se va a convertir en el que estoy ahora
        mySegment = mySegment->next;
    }
    
    printf("No se encontró segmento que contenga la columna %d\n", col);
}

int gameBoardAddPlant(GameBoard* board, int row, int col) {
    // TODO: Crear la nueva `Planta` con memoria dinámica y asignarla al `planta_data` del nuevo segmento.
    RowSegment* mySegment = board->rows[row].first_segment;
    RowSegment* prevSegment = NULL;
    
    // Itero por mis segmentos hasta encontrar la que pertenece al rango que quiero
    while (mySegment != NULL) {
        // TODO: Encontrar la GardenRow correcta. 
        int ultSegment =  mySegment->start_col + mySegment->length;

        if (mySegment->start_col <= col && col < (mySegment->start_col + mySegment->length)) {
            // ¿Está realmente vacio?
            if (mySegment->status != STATUS_VACIO) {
                /*
                    Si no está vacio podemos implementar gameBoardRemovePlant
                    Si no queremos que la logica funcione de esta manera podemos comentar la función 
                    La diferencia que tiene comentar la función es que ninguna planta se va a eliminar
                    De esta forma la lista no será alterada y la fusión de los segmentos de 
                    gameBoardRemovePlant no se van a hacer
                    Por ende, no nos podemos "arrepentir" de poner una planta en ningun lado
                */
                gameBoardRemovePlant(board, row, col);
                return 0;
            }
            
            // TODO: Recorrer la lista de RowSegment hasta encontrar el segmento VACIO que contenga a `col`.
            // Solo llega aca si efectivamente esta vacio
            // Inicializo una nueva planta de la misma forma que en el juego_base.c
            Planta* newPlant = (Planta*)malloc(sizeof(Planta));
            if (!newPlant) return 0;
            
            newPlant->rect.x = GRID_OFFSET_X + (col * CELL_WIDTH);
            newPlant->rect.y = GRID_OFFSET_Y + (row * CELL_HEIGHT);
            newPlant->rect.w = CELL_WIDTH;
            newPlant->rect.h = CELL_HEIGHT;
            newPlant->activo = 1;
            newPlant->cooldown = rand() % 100;
            newPlant->current_frame = 0;
            newPlant->frame_timer = 0;
            newPlant->debe_disparar = 0;
            
            
            // TODO: Si se encuentra y tiene espacio, realizar la lógica de DIVISIÓN de segmento.
            // Lo separo por casos

            // Caso 1: Mi segmento ya tiene una sola celda
            // En ese caso inserto mi nueva planta en el segmento directamente
            if (mySegment->length == 1) {
                mySegment->status = STATUS_PLANTA;
                mySegment->planta_data = newPlant;
                return 1;
            }
            
            // Caso 2: Debo insertar la planta al inicio del segmento
            if (col == mySegment->start_col) {
                RowSegment* newSegment = (RowSegment*)malloc(sizeof(RowSegment));
                if (!newSegment) {
                    free(newPlant);
                    return 0;
                }
                newSegment->status = STATUS_PLANTA;
                newSegment->start_col = col;
                newSegment->length = 1;
                newSegment->planta_data = newPlant;
                newSegment->next = mySegment;
                
                mySegment->start_col = col + 1;
                mySegment->length--;
                
                // Conecto mi segmento anterior y lo redirecciono para que apunte al nuevo segmento 
                if (prevSegment == NULL) {
                    board->rows[row].first_segment = newSegment;
                } else {
                    prevSegment->next = newSegment;
                }
                
                printf("Planta agregada al inicio del segmento [%d,%d]\n", row, col);
                return 1;
            }
            
            // Caso 3: La planta se encuentra al final del segmento
            /*
                En este caso voy a ajustar el ultimo segmento del rango para que se refiera a mi segmento
                Voy a crear un nuevo segmento que va a tener los valores de mi segmento original
                Al segmento que solía estar apuntando al final lo redirecciono
            */
            if (col == ultSegment - 1) {
                RowSegment* newSegment = (RowSegment*)malloc(sizeof(RowSegment));
                // Evito leaks!
                if (!newSegment) {
                    free(newPlant);
                    return 0;
                }
                newSegment->status = STATUS_PLANTA;
                newSegment->start_col = col;
                newSegment->length = 1;
                newSegment->planta_data = newPlant;
                newSegment->next = mySegment->next;
                
                mySegment->length--;
                mySegment->next = newSegment;

                return 1;
            }
            
            // Caso 4: (solo llega aca si los demás ifs no retornaron) La planta se encuentra en el medio
            /*
                En este caso voy a querer ajustar el segmento a la izquierda de mi segmento
                Mientras utilizo el segmento que tengo a la derecha para crear mi nuevo rango
            */
            RowSegment* mySegmentCopy = (RowSegment*)malloc(sizeof(RowSegment));
            RowSegment* mySegmentRight = (RowSegment*)malloc(sizeof(RowSegment));

            // Evito leaks!
            if (!mySegmentCopy || !mySegmentRight) {
                free(newPlant);
                if (mySegmentCopy) free(mySegmentCopy);
                if (mySegmentRight) free(mySegmentRight);
                return 0;
            }
            
            mySegmentCopy->status = STATUS_PLANTA;
            mySegmentCopy->start_col = col;
            mySegmentCopy->length = 1;
            mySegmentCopy->planta_data = newPlant;
            
            mySegmentRight->status = STATUS_VACIO;
            mySegmentRight->start_col = col + 1;
            mySegmentRight->length = ultSegment - (col + 1);
            mySegmentRight->planta_data = NULL;
            mySegmentRight->next = mySegment->next;
            
            mySegment->length = col - mySegment->start_col;
            mySegment->next = mySegmentCopy;
            mySegmentCopy->next = mySegmentRight;

            return 1;
        }
        
        prevSegment = mySegment;
        mySegment = mySegment->next;
    }
    
    printf("No se encontró segmento que contenga la columna %d\n", col);
    return 0;
}

void gameBoardAddZombie(GameBoard* board, int row) {
    // TODO: Crear un nuevo ZombieNode con memoria dinámica.
    ZombieNode* newZombie = (ZombieNode*)malloc(sizeof(ZombieNode));
    if (!newZombie) {
        printf("No pude asignar memoria para el zombie.\n");
        return;
    }
    
    // TODO: Inicializar sus datos (posición, vida, animación, etc.).
    newZombie->zombie_data.row = row;
    newZombie->zombie_data.pos_x = SCREEN_WIDTH;
    newZombie->zombie_data.rect.x = (int)newZombie->zombie_data.pos_x;
    newZombie->zombie_data.rect.y = GRID_OFFSET_Y + (row * CELL_HEIGHT);
    newZombie->zombie_data.rect.w = CELL_WIDTH;
    newZombie->zombie_data.rect.h = CELL_HEIGHT;
    newZombie->zombie_data.vida = 100;
    newZombie->zombie_data.activo = 1;
    newZombie->zombie_data.current_frame = 0;
    newZombie->zombie_data.frame_timer = 0;
    newZombie->next = NULL;
    
    // TODO: Agregarlo a la lista enlazada simple de la GardenRow correspondiente.
    // Busco su row correspondiente
    if (board->rows[row].first_zombie == NULL) { // ¿Es el primer zombie?
        board->rows[row].first_zombie = newZombie;
    } else {
        ZombieNode* current = board->rows[row].first_zombie;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newZombie;
    }
}

int gameBoardUpdate(GameBoard* board) {
    // TODO: Recorrer las listas de zombies de cada fila para moverlos y animarlos.
    for (int row = 0; row < GRID_ROWS; row++) {
        ZombieNode* zombieNode = board->rows[row].first_zombie;
        ZombieNode* prevZombie = NULL;

        while (zombieNode != NULL) { 
            Zombie* z = &zombieNode->zombie_data;
            
            if (z->activo) { // Si esta activo lo muevo
                float distance_per_tick = ZOMBIE_DISTANCE_PER_CYCLE / (float)(ZOMBIE_TOTAL_FRAMES * ZOMBIE_ANIMATION_SPEED);
                z->pos_x -= distance_per_tick;
                z->rect.x = (int)z->pos_x;
                
                // El zombie esta muy cerca de la casa?? (O sea a 130px desde el inicio de arriba a la izquierda del frame)
                // GAME OVER
                if (z->rect.x < 130) {
                    printf("El zombie llegó a la cas");
                    return 1; // Le avisa al main que es game over
                }
                
                // Actualizo la animación
                z->frame_timer++;
                if (z->frame_timer >= ZOMBIE_ANIMATION_SPEED) {
                    z->frame_timer = 0;
                    z->current_frame = (z->current_frame + 1) % ZOMBIE_TOTAL_FRAMES;
                }
            }

            // Si el zombie esta inactivo lo saco del frame
            if (!z->activo) {
                if (prevZombie == NULL) {
                    board->rows[row].first_zombie = zombieNode->next;
                    zombieNode = zombieNode->next;
                } else {
                    prevZombie->next = zombieNode->next;
                    zombieNode = zombieNode->next;
                }
                free(zombieNode);
            } else {
                prevZombie = zombieNode;
                zombieNode = zombieNode->next;
            }
        }
    }

    // TODO: Recorrer las listas de segmentos de cada fila para gestionar los cooldowns y animaciones de las plantas.
    for (int row = 0; row < GRID_ROWS; row++) {
        RowSegment* mySegment = board->rows[row].first_segment;
        
        while (mySegment != NULL) { 
            if (mySegment->status == STATUS_PLANTA && mySegment->planta_data != NULL) {
                Planta* p = mySegment->planta_data;
                
                // Actualizo los cooldowna
                if (p->cooldown > 0) {
                    p->cooldown--;
                } else {
                    p->debe_disparar = 1;
                }
                
                // Actualizo las animaciones
                p->frame_timer++;
                if (p->frame_timer >= PEASHOOTER_ANIMATION_SPEED) {
                    p->frame_timer = 0;
                    p->current_frame = (p->current_frame + 1) % PEASHOOTER_TOTAL_FRAMES;
                    
                    // Me aseguro que este disparando en el frame correcto
                    if (p->debe_disparar && p->current_frame == PEASHOOTER_SHOOT_FRAME) {
                        // ¿Hay arvejas inactivas?
                        for (int i = 0; i < MAX_ARVEJAS; i++) {
                            if (!board->arvejas[i].activo) { // La arveja esta inactiva?
                                board->arvejas[i].rect.x = p->rect.x + (CELL_WIDTH / 2);
                                board->arvejas[i].rect.y = p->rect.y + (CELL_HEIGHT / 4);
                                board->arvejas[i].rect.w = 20;
                                board->arvejas[i].rect.h = 20;
                                board->arvejas[i].activo = 1;
                                break;
                            }
                        }
                        p->cooldown = 120;
                        p->debe_disparar = 0;
                    }
                }
            }
            mySegment = mySegment->next;
        }
    }

    // TODO: Actualizar la lógica de disparo, colisiones y spawn de zombies.
    // Colisiones:
    for (int row = 0; row < GRID_ROWS; row++) {
        ZombieNode* zombieNode = board->rows[row].first_zombie;
        
        while (zombieNode != NULL) {
            if (!zombieNode->zombie_data.activo) {
                zombieNode = zombieNode->next;
                continue;
            }
            
            for (int j = 0; j < MAX_ARVEJAS; j++) {
                if (!board->arvejas[j].activo) continue;
                
                int arveja_row = (board->arvejas[j].rect.y - GRID_OFFSET_Y) / CELL_HEIGHT;
                
                if (zombieNode->zombie_data.row == arveja_row) {
                    if (SDL_HasIntersection(&board->arvejas[j].rect, &zombieNode->zombie_data.rect)) {
                        board->arvejas[j].activo = 0;
                        zombieNode->zombie_data.vida -= 25;
                        if (zombieNode->zombie_data.vida <= 0) {
                            zombieNode->zombie_data.activo = 0;
                        }
                    }
                }
            }
            
            zombieNode = zombieNode->next;
        }
    }

    
    // Actualizo las arvejas
    for (int i = 0; i < MAX_ARVEJAS; i++) {
        if (board->arvejas[i].activo) {
            board->arvejas[i].rect.x += PEA_SPEED;
            
            // Si esta afuera del frame la quiero eliminar
            if (board->arvejas[i].rect.x > SCREEN_WIDTH) {
                board->arvejas[i].activo = 0;
            }
        }
    }

    // Modifico el spawn de los zombies para que se vayan generando de manera aleatoria
    board->zombie_spawn_timer--;
    if (board->zombie_spawn_timer <= 0) {
        int random_row = rand() % GRID_ROWS; // RANDOM!
        gameBoardAddZombie(board, random_row);
        board->zombie_spawn_timer = ZOMBIE_SPAWN_RATE;
    }
    
    return 0;
}

void gameBoardDraw(GameBoard* board) {
    // TODO: Re-implementar la lógica de `dibujar` usando las nuevas estructuras.
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, tex_background, NULL, NULL);

    // TODO: Recorrer las listas de segmentos para dibujar las plantas.
    // Render si esta activo, pasando por cada nodo
    for (int row = 0; row < GRID_ROWS; row++) {
        RowSegment* mySegment = board->rows[row].first_segment;
        
        while (mySegment != NULL) {
            if (mySegment->status == STATUS_PLANTA && mySegment->planta_data != NULL) {
                Planta* p = mySegment->planta_data;
                SDL_Rect src_rect = { p->current_frame * PEASHOOTER_FRAME_WIDTH, 0, PEASHOOTER_FRAME_WIDTH, PEASHOOTER_FRAME_HEIGHT };
                SDL_RenderCopy(renderer, tex_peashooter_sheet, &src_rect, &p->rect);
            }
            mySegment = mySegment->next;
        }
    }

    // TODO: Recorrer las listas de zombies para dibujarlos.
    // Render si esta activo, pasando por cada nodo
    for (int row = 0; row < GRID_ROWS; row++) {
        ZombieNode* zombieNode = board->rows[row].first_zombie;
        
        while (zombieNode != NULL) {
            Zombie* z = &zombieNode->zombie_data;
            
            if (z->activo) {
                SDL_Rect src_rect = { z->current_frame * ZOMBIE_FRAME_WIDTH, 0, ZOMBIE_FRAME_WIDTH, ZOMBIE_FRAME_HEIGHT };
                SDL_RenderCopy(renderer, tex_zombie_sheet, &src_rect, &z->rect);
            }
            
            zombieNode = zombieNode->next;
        }
    }

    // TODO: Dibujar las arvejas y el cursor.
    // Rendeo si esta activo
    for (int i = 0; i < MAX_ARVEJAS; i++) {
        if (board->arvejas[i].activo) {
            SDL_RenderCopy(renderer, tex_pea, NULL, &board->arvejas[i].rect);
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 200);
    SDL_Rect cursor_rect = {GRID_OFFSET_X + cursor.col * CELL_WIDTH, GRID_OFFSET_Y + cursor.row * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT};
    SDL_RenderDrawRect(renderer, &cursor_rect);
    SDL_RenderPresent(renderer);
}

SDL_Texture* cargarTextura(const char* path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (newTexture == NULL) printf("No se pudo cargar la textura %s! SDL_image Error: %s\n", path, IMG_GetError());
    return newTexture;
}

int inicializar() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 0;
    window = SDL_CreateWindow("Plantas vs Zombies - Solucion", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) return 0;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) return 0;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 0;
    tex_background = cargarTextura("res/Frontyard.png");
    tex_peashooter_sheet = cargarTextura("res/peashooter_sprite_sheet.png");
    tex_zombie_sheet = cargarTextura("res/zombie_sprite_sheet.png");
    tex_pea = cargarTextura("res/pea.png");
    return 1;
}

void cerrar() {
    SDL_DestroyTexture(tex_background);
    SDL_DestroyTexture(tex_peashooter_sheet);
    SDL_DestroyTexture(tex_zombie_sheet);
    SDL_DestroyTexture(tex_pea);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

// LA PARTE DE LOS STRINGS

char* strDuplicate(char* src) {
	// Contamos la cantidad de caracteres del string fuente para saber cuánta memoria dinámica hay que solicitar
	int len_total = 0;
	for (int i = 0; src[i] != '\0'; i++) {
		len_total++;
	}
	
	// Pedimos memoria para el nuevo string
	char* res = (char*) malloc(sizeof(char) * (len_total + 1));
	if (!res) return NULL;

	// Copiamos los caracteres del string fuente a la nueva región
	int i = 0;
	for (i = 0; src[i] != '\0'; i++) {
		res[i] = src[i];
	}

	// Colocamos un terminador
	res[len_total] = '\0';

	// Devolvemos el puntero al nuevos string dinámico
	return res;
}

int strCompare(char* s1, char* s2){
	// Recorremos ambos strings en simultáneo y nos detenemos apenas encontremos una diferencia o terminen
	int i = 0;
	while (s1[i] != '\0' && s2[i] != '\0') {
	// Si el carácter de s1 es menor que el de s2, devolvemos 1. Si es mayor, -1.
        if (s1[i] < s2[i]) return 1;
        if (s1[i] > s2[i]) return -1;
        i++;
    }

	// Si terminan a la vez son iguales
    if (s1[i] == '\0' && s2[i] == '\0') return 0;

	// Si s1 termina antes, es menor. Si s2 termina antes, s1 es mayor.
    if (s1[i] == '\0') return 1;  
    else return -1;
}

char* strConcatenate(char* src1, char* src2) {
    // Calculamos la longitud de los strings
    int len1 = 0;
    while (src1[len1] != '\0') {
        len1++;
    }

    int len2 = 0;
    while (src2[len2] != '\0') {
        len2++;
    }

    // Reservamos la cantidad de memoria dinámica suficiente para concatenar los strings más el carácter terminador
    char* res = (char*) malloc(len1 + len2 + 1);
    if (res == NULL) {
        return NULL;
    }

    // Copiamos los string en la nueva region - En orden (primero el 1 y despues el 2)
    for (int i = 0; i < len1; i++) {
        res[i] = src1[i];
    }
    for (int i = 0; i < len2; i++) {
        res[len1 + i] = src2[i];
    }

    // Cerramos el nuevo string con ‘\0’
    res[len1 + len2] = '\0';

    // Liberamos la memoria original
    free(src1);
    free(src2);

    return res;
}


void testStrDuplicate(){
	printf("CASOS DE TEST STR DUPLICATE \n");
	// String vacio
	printf("El nuevo string vacio duplicado apunta a la memoria %p \n", strDuplicate(""));

	// String de un caracter
	printf("El nuevo string de un caracter apunta a la memoria %p \n", strDuplicate("a"));

	// String que incluya todos los caracteres validos distintos de cero
	char original[256];  // 255 caracteres + terminador
    int idx = 0;
    
	// Incluir TODOS los bytes del 1 al 255
	for (int c = 1; c <= 255; c++) {
		original[idx++] = (char)c;
	}
	original[idx] = '\0';  // Terminador

	// Duplicar y verificar
	char* duplicado = strDuplicate(original);
	if (duplicado == NULL) {
		printf("Error: strDuplicate falló\n");
		return;
	}

	printf("El string duplicado apunta a la memoria %p\n", (void*)duplicado);
	free(duplicado);
}

void testStrCompare() {
	printf("CASOS DE TEST STR COMPARE \n");

	// Dos strings vacios
	printf("El resultado de comparar dos string vacios es: %d\n", strCompare("",""));

	// Dos strings de un caracter
	printf("El resultado de comparar dos string de un caracter es: %d\n", strCompare("a","d"));

	// String iguales hasta un caracter
	printf("Strings iguales hasta un caracter [str1, str2]: %d\n", strCompare("aloha","aloho"));
	printf("Strings iguales hasta un caracter [str2, str1]: %d\n", strCompare("aloho","aloha"));

	// Dos strings diferentes
	printf("Comparo dos strings diferentes: %d\n", strCompare("mamma mia","gimme gimme"));

}

void testStrConcatenate() {
	printf("CASOS DE TEST STR CONCATENATE \n");
    
	// Un string vacio y un string de 3 caracteres
    printf("TEST 1: Concatenar 0 chars + 3 chars\n");
    char* s1_test1 = strDuplicate("");
    char* s2_test1 = strDuplicate("alo");
    char* resultado1 = strConcatenate(s1_test1, s2_test1);
    
    if (resultado1 == NULL) {
        printf("ERROR: strConcatenate devolvió NULL\n");
    } else {
        printf("Resultado: \"%s\"\n", resultado1);
        printf("Esperado: \"alo\"\n");
        printf("%s\n", strcmp(resultado1, "alo") == 0 ? "PASÓ" : "FALLÓ");
        free(resultado1);
    }
    
    printf("\n");
    
    // String de 3 caracteres + string vacío
    printf("TEST 2: Concatenar 3 chars + 0 chars\n");
    char* s1_test2 = strDuplicate("alo");
    char* s2_test2 = strDuplicate("");
    char* resultado2 = strConcatenate(s1_test2, s2_test2);
    
    if (resultado2 == NULL) {
        printf("ERROR: strConcatenate devolvió NULL\n");
    } else {
        printf("Resultado: \"%s\"\n", resultado2);
        printf("Esperado: \"alo\"\n");
        printf("%s\n", strcmp(resultado2, "alo") == 0 ? "PASÓ" : "FALLÓ");
        free(resultado2);
    }
    
    printf("\n");

    // Dos strings de 1 carácter
    printf("TEST 3: Concatenar dos strings de 1 char\n");
    char* s1_test3 = strDuplicate("a");
    char* s2_test3 = strDuplicate("h");
    char* resultado3 = strConcatenate(s1_test3, s2_test3);
    
    if (resultado3 == NULL) {
        printf("ERROR: strConcatenate devolvió NULL\n");
    } else {
        printf("Resultado: \"%s\"\n", resultado3);
        printf("Esperado: \"ah\"\n");
        printf("%s\n", strcmp(resultado3, "ah") == 0 ? "PASÓ" : "FALLÓ");
        free(resultado3);
    }
    
    printf("\n");

    // TEST 4: Dos strings de 5 caracteres
    printf("TEST 4: Concatenar dos strings de 5 chars\n");
    char* s1_test4 = strDuplicate("aloha");
    char* s2_test4 = strDuplicate("holas");
    char* resultado4 = strConcatenate(s1_test4, s2_test4);
    
    if (resultado4 == NULL) {
        printf("ERROR: strConcatenate devolvió NULL\n");
    } else {
        printf("Resultado: \"%s\"\n", resultado4);
        printf("Esperado: \"alohaholas\"\n");
        printf("%s\n", strcmp(resultado4, "alohaholas") == 0 ? "PASÓ" : "FALLÓ");
        free(resultado4);
    }
}

int main(int argc, char* args[]) {
	printf("SOME TESTS!\n");
	testStrDuplicate();
	printf("\n");
	testStrCompare();
	printf("\n");
	testStrConcatenate();

    srand(time(NULL));
    if (!inicializar()) return 1;

    game_board = gameBoardNew();

    SDL_Event e;
    int game_over = 0;

    while (!game_over) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) game_over = 1;
            if (e.type == SDL_MOUSEMOTION) {
                int mouse_x = e.motion.x;
                int mouse_y = e.motion.y;
                if (mouse_x >= GRID_OFFSET_X && mouse_x < GRID_OFFSET_X + GRID_WIDTH &&
                    mouse_y >= GRID_OFFSET_Y && mouse_y < GRID_OFFSET_Y + GRID_HEIGHT) {
                    cursor.col = (mouse_x - GRID_OFFSET_X) / CELL_WIDTH;
                    cursor.row = (mouse_y - GRID_OFFSET_Y) / CELL_HEIGHT;
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                gameBoardAddPlant(game_board, cursor.row, cursor.col);
            }
        }

        // TODO: Agregar la lógica para ver si un zombie llegó a la casa y terminó el juego
        int is_game_over = gameBoardUpdate(game_board);
        if (is_game_over == 1) { // gameBoardUpdate devuelve 1 cuando el zombie llega a la casa
            game_over = 1; // Se corta el while 
        }
        gameBoardDraw(game_board);

        SDL_Delay(16);
    }

    gameBoardDelete(game_board);
    cerrar();
    return 0;
}


/*
    USO DE IA
    - Estimamos que entre el 10% y 20% de las líneas de código fueron realizadas con 
    asistencia de herramientas de IA (principalmente ChatGPT)

    - Verificamos las sugerencias validando nosotras manualmente el comportamiento esperado
    
    - La inteligencia artificial en ocasiones sugería soluciones más 
    complejas de las que habíamos visto en clase, por lo que le adjuntamos a 
    la IA fragmentos de código base y archivos de clase, explicándole qué herramientas sí habíamos visto 
    
    - El uso de IA nos permitió ayudar a entender mejor cómo manipular 
    estructuras dinámicas y manejo de punteros que a veces puede ser muy confuso, 
    además de aprender a poner en palabras y explicar mejor lo que hace nuestro código

*/
