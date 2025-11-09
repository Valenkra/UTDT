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
    // TODO: Recorrer cada GardenRow.
    // TODO: Liberar todos los RowSegment (y los planta_data si existen).
    // TODO: Liberar todos los ZombieNode.
    // TODO: Finalmente, liberar el GameBoard.
    if (!board) return;
    
    // Liberar todos los recursos de cada fila
    for (int i = 0; i < GRID_ROWS; i++) {
        // Liberar todos los segmentos de la fila
        RowSegment* segment = board->rows[i].first_segment;
        while (segment != NULL) {
            RowSegment* next_segment = segment->next;
            
            // Si el segmento tiene una planta, liberar su memoria
            if (segment->planta_data != NULL) {
                free(segment->planta_data);
            }
            
            // Liberar el segmento
            free(segment);
            segment = next_segment;
        }
        
        // Liberar todos los zombies de la fila
        ZombieNode* zombie_node = board->rows[i].first_zombie;
        while (zombie_node != NULL) {
            ZombieNode* next_zombie = zombie_node->next;
            free(zombie_node);
            zombie_node = next_zombie;
        }
    }
    
    // Finalmente, liberar el GameBoard
    free(board);
}

int gameBoardAddPlant(GameBoard* board, int row, int col) {
    // TODO: Encontrar la GardenRow correcta. ✓
    // TODO: Recorrer la lista de RowSegment hasta encontrar el segmento VACIO que contenga a `col`. ✓
    // TODO: Si se encuentra y tiene espacio, realizar la lógica de DIVISIÓN de segmento.
    // TODO: Crear la nueva `Planta` con memoria dinámica y asignarla al `planta_data` del nuevo segmento.

    // Encontrar la GardenRow correcta - Selecciono la GardenRow correcta que me pasan por parámetro
    RowSegment* segment = board->rows[row].first_segment;
    RowSegment* prev_segment = NULL;
    
    while (segment != NULL) {
        int seg_end = segment->start_col + segment->length;
        
        // Me fijo si el segmento que estoy buscando se encuentra en este bloque
        if (segment->start_col <= col && col < seg_end) {
            // Encuentro el segmento VACIO que contenga a `col`
            if (segment->status != STATUS_VACIO) {
                printf("Ya hay una planta en esta celda\n");
                return 0;
            }
            
            // Crear la planta
            Planta* new_plant = (Planta*)malloc(sizeof(Planta));
            if (!new_plant) return 0;
            
            new_plant->rect.x = GRID_OFFSET_X + (col * CELL_WIDTH);
            new_plant->rect.y = GRID_OFFSET_Y + (row * CELL_HEIGHT);
            new_plant->rect.w = CELL_WIDTH;
            new_plant->rect.h = CELL_HEIGHT;
            new_plant->activo = 1;
            new_plant->cooldown = rand() % 100;
            new_plant->current_frame = 0;
            new_plant->frame_timer = 0;
            new_plant->debe_disparar = 0;
            
            // CASO 1: El segmento tiene una sola celda
            if (segment->length == 1) {
                segment->status = STATUS_PLANTA;
                segment->planta_data = new_plant;
                printf("Planta agregada en segmento de 1 celda [%d,%d]\n", row, col);
                return 1;
            }
            
            // CASO 2: La planta está al inicio del segmento
            if (col == segment->start_col) {
                // Crear nuevo segmento PLANTA al inicio
                RowSegment* nuevo_seg = (RowSegment*)malloc(sizeof(RowSegment));
                if (!nuevo_seg) {
                    free(new_plant);
                    return 0;
                }
                nuevo_seg->status = STATUS_PLANTA;
                nuevo_seg->start_col = col;
                nuevo_seg->length = 1;
                nuevo_seg->planta_data = new_plant;
                nuevo_seg->next = segment;
                
                // Ajustar el segmento vacío restante
                segment->start_col = col + 1;
                segment->length--;
                
                // Conectar con el segmento anterior
                if (prev_segment == NULL) {
                    board->rows[row].first_segment = nuevo_seg;
                } else {
                    prev_segment->next = nuevo_seg;
                }
                
                printf("Planta agregada al inicio del segmento [%d,%d]\n", row, col);
                return 1;
            }
            
            // CASO 3: La planta está al final del segmento
            if (col == seg_end - 1) {
                // Crear nuevo segmento PLANTA al final
                RowSegment* nuevo_seg = (RowSegment*)malloc(sizeof(RowSegment));
                if (!nuevo_seg) {
                    free(new_plant);
                    return 0;
                }
                nuevo_seg->status = STATUS_PLANTA;
                nuevo_seg->start_col = col;
                nuevo_seg->length = 1;
                nuevo_seg->planta_data = new_plant;
                nuevo_seg->next = segment->next;
                
                // Ajustar el segmento vacío
                segment->length--;
                segment->next = nuevo_seg;
                
                printf("Planta agregada al final del segmento [%d,%d]\n", row, col);
                return 1;
            }
            
            // CASO 4: La planta está en medio del segmento (división en 3)
            RowSegment* seg_planta = (RowSegment*)malloc(sizeof(RowSegment));
            RowSegment* seg_derecha = (RowSegment*)malloc(sizeof(RowSegment));
            if (!seg_planta || !seg_derecha) {
                free(new_plant);
                if (seg_planta) free(seg_planta);
                if (seg_derecha) free(seg_derecha);
                return 0;
            }
            
            // Segmento de la planta
            seg_planta->status = STATUS_PLANTA;
            seg_planta->start_col = col;
            seg_planta->length = 1;
            seg_planta->planta_data = new_plant;
            
            // Segmento vacío derecho
            seg_derecha->status = STATUS_VACIO;
            seg_derecha->start_col = col + 1;
            seg_derecha->length = seg_end - (col + 1);
            seg_derecha->planta_data = NULL;
            seg_derecha->next = segment->next;
            
            // Ajustar segmento izquierdo (el original)
            segment->length = col - segment->start_col;
            segment->next = seg_planta;
            seg_planta->next = seg_derecha;
            
            printf("Planta agregada dividiendo segmento en 3 [%d,%d]\n", row, col);
            return 1;
        }
        
        prev_segment = segment;
        segment = segment->next;
    }
    
    printf("No se encontró segmento que contenga la columna %d\n", col);
    return 0;

}

void gameBoardRemovePlant(GameBoard* board, int row, int col) {
    // TODO: Similar a AddPlant, encontrar el segmento que contiene `col`.
    // TODO: Si es un segmento de tipo PLANTA, convertirlo a VACIO y liberar el `planta_data`.
    // TODO: Implementar la lógica de FUSIÓN con los segmentos vecinos si también son VACIO.
    printf("Función gameBoardRemovePlant no implementada.\n");
}

/*
void gameBoardAddZombie(GameBoard* board, int row) {
    // TODO: Crear un nuevo ZombieNode con memoria dinámica.
    // TODO: Inicializar sus datos (posición, vida, animación, etc.).
    // TODO: Agregarlo a la lista enlazada simple de la GardenRow correspondiente.

    ZombieNode* new_zombie = (ZombieNode*)malloc(sizeof(ZombieNode));
    if (!new_zombie) {
        printf("Error al crear nuevo zombie.\n");
        return;
    }

    // Inicializar datos del zombie
    new_zombie->zombie_data.pos_x = GRID_WIDTH;
    new_zombie->zombie_data.row = row;
    new_zombie->zombie_data.vida = 100;
    new_zombie->zombie_data.activo = 1;
    new_zombie->zombie_data.current_frame = 0;
    new_zombie->zombie_data.frame_timer = 0;
    new_zombie->next = NULL;

    // Agregar a la lista de zombies de la fila correspondiente
    if (!board->rows[row].first_zombie) {
        board->rows[row].first_zombie = new_zombie;
    } else {
        ZombieNode* last = board->rows[row].first_zombie;
        while (last->next) {
            last = last->next;
        }
        last->next = new_zombie;
    }

    printf("Función gameBoardAddZombie no implementada.\n");
}
*/

void gameBoardAddZombie(GameBoard* board, int row) {
    if (!board) return;
    
    // Crear un nuevo ZombieNode con memoria dinámica
    ZombieNode* new_zombie = (ZombieNode*)malloc(sizeof(ZombieNode));
    if (!new_zombie) {
        printf("Error: No se pudo asignar memoria para el zombie.\n");
        return;
    }

    // Inicializar datos del zombie
    new_zombie->zombie_data.row = row;
    new_zombie->zombie_data.pos_x = SCREEN_WIDTH;
    new_zombie->zombie_data.rect.x = (int)new_zombie->zombie_data.pos_x;
    new_zombie->zombie_data.rect.y = GRID_OFFSET_Y + (row * CELL_HEIGHT);
    new_zombie->zombie_data.rect.w = CELL_WIDTH;
    new_zombie->zombie_data.rect.h = CELL_HEIGHT;
    new_zombie->zombie_data.vida = 100;
    new_zombie->zombie_data.activo = 1;
    new_zombie->zombie_data.current_frame = 0;
    new_zombie->zombie_data.frame_timer = 0;
    new_zombie->next = NULL;

    // Agregar a la lista enlazada de zombies de la fila correspondiente
    if (board->rows[row].first_zombie == NULL) {
        // Si no hay zombies, este es el primero
        board->rows[row].first_zombie = new_zombie;
    } else {
        // Si ya hay zombies, agregarlo al final de la lista
        ZombieNode* current = board->rows[row].first_zombie;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_zombie;
    }
}

void gameBoardUpdate(GameBoard* board) {
    if (!board) return;
    // TODO: Re-implementar la lógica de `actualizarEstado` usando las nuevas estructuras.
    // TODO: Recorrer las listas de zombies de cada fila para moverlos y animarlos.
    // TODO: Recorrer las listas de segmentos de cada fila para gestionar los cooldowns y animaciones de las plantas.
    // TODO: Actualizar la lógica de disparo, colisiones y spawn de zombies.

    //  ACTUALIZAR ZOMBIES 
    for (int row = 0; row < GRID_ROWS; row++) {
        ZombieNode* zombie_node = board->rows[row].first_zombie;
        ZombieNode* prev_zombie = NULL;

        while (zombie_node != NULL) {
            Zombie* z = &zombie_node->zombie_data;
            
            if (z->activo) {
                // Actualizar animación
                z->frame_timer++;
                if (z->frame_timer >= ZOMBIE_ANIMATION_SPEED) {
                    z->frame_timer = 0;
                    z->current_frame = (z->current_frame + 1) % ZOMBIE_TOTAL_FRAMES;
                    
                    // Mover zombie al completar el ciclo de animación
                    if (z->current_frame == 0) {
                        z->pos_x -= ZOMBIE_DISTANCE_PER_CYCLE;
                        z->rect.x = (int)z->pos_x;
                    }
                }

                // Verificar si el zombie llegó a la casa (game over)
                if (z->rect.x + z->rect.w < GRID_OFFSET_X) {
                    printf("¡Un zombie llegó a la casa! Game Over\n");
                    z->activo = 0;
                }

                // Verificar colisión con arvejas
                for (int i = 0; i < MAX_ARVEJAS; i++) {
                    if (board->arvejas[i].activo) {
                        Arveja* a = &board->arvejas[i];
                        if (SDL_HasIntersection(&a->rect, &z->rect)) {
                            a->activo = 0;
                            z->vida--;
                            if (z->vida <= 0) {
                                z->activo = 0;
                            }
                        }
                    }
                }
            }

            // Remover zombie si está inactivo
            if (!z->activo) {
                ZombieNode* to_delete = zombie_node;
                if (prev_zombie == NULL) {
                    board->rows[row].first_zombie = zombie_node->next;
                    zombie_node = zombie_node->next;
                } else {
                    prev_zombie->next = zombie_node->next;
                    zombie_node = zombie_node->next;
                }
                free(to_delete);
            } else {
                prev_zombie = zombie_node;
                zombie_node = zombie_node->next;
            }
        }
    }

    //  ACTUALIZAR PLANTAS 
    for (int row = 0; row < GRID_ROWS; row++) {
        RowSegment* segment = board->rows[row].first_segment;
        
        while (segment != NULL) {
            if (segment->status == STATUS_PLANTA && segment->planta_data != NULL) {
                Planta* p = segment->planta_data;
                
                // Actualizar animación
                p->frame_timer++;
                if (p->frame_timer >= PEASHOOTER_ANIMATION_SPEED) {
                    p->frame_timer = 0;
                    
                    // Verificar si debe disparar en este frame
                    if (p->current_frame == PEASHOOTER_SHOOT_FRAME && p->cooldown <= 0) {
                        // Verificar si hay zombies en esta fila
                        int hay_zombies = 0;
                        ZombieNode* z_node = board->rows[row].first_zombie;
                        while (z_node != NULL) {
                            if (z_node->zombie_data.activo) {
                                hay_zombies = 1;
                                break;
                            }
                            z_node = z_node->next;
                        }
                        
                        if (hay_zombies) {
                            p->debe_disparar = 1;
                            p->cooldown = 60; // Cooldown de ~1 segundo
                        }
                    }
                    
                    p->current_frame = (p->current_frame + 1) % PEASHOOTER_TOTAL_FRAMES;
                }
                
                // Actualizar cooldown
                if (p->cooldown > 0) {
                    p->cooldown--;
                }
                
                // Crear arveja si debe disparar
                if (p->debe_disparar) {
                    for (int i = 0; i < MAX_ARVEJAS; i++) {
                        if (!board->arvejas[i].activo) {
                            board->arvejas[i].rect.x = p->rect.x + p->rect.w / 2;
                            board->arvejas[i].rect.y = p->rect.y + p->rect.h / 2 - 5;
                            board->arvejas[i].rect.w = 20;
                            board->arvejas[i].rect.h = 20;
                            board->arvejas[i].activo = 1;
                            break;
                        }
                    }
                    p->debe_disparar = 0;
                }
            }
            segment = segment->next;
        }
    }

    //  ACTUALIZAR ARVEJAS 
    for (int i = 0; i < MAX_ARVEJAS; i++) {
        if (board->arvejas[i].activo) {
            board->arvejas[i].rect.x += PEA_SPEED;
            
            // Desactivar si sale de la pantalla
            if (board->arvejas[i].rect.x > SCREEN_WIDTH) {
                board->arvejas[i].activo = 0;
            }
        }
    }

    //  SPAWN DE ZOMBIES 
    board->zombie_spawn_timer--;
    if (board->zombie_spawn_timer <= 0) {
        int random_row = rand() % GRID_ROWS;
        gameBoardAddZombie(board, random_row);
        board->zombie_spawn_timer = ZOMBIE_SPAWN_RATE;
    }
}

void gameBoardDraw(GameBoard* board) {
    if (!board) return;
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, tex_background, NULL, NULL);

    // TODO: Re-implementar la lógica de `dibujar` usando las nuevas estructuras.
    // TODO: Recorrer las listas de segmentos para dibujar las plantas.
    // TODO: Recorrer las listas de zombies para dibujarlos.
    // TODO: Dibujar las arvejas y el cursor.

    // Dibujo las plantas
    for (int row = 0; row < GRID_ROWS; row++) {
        RowSegment* segment = board->rows[row].first_segment;
        
        while (segment != NULL) {
            if (segment->status == STATUS_PLANTA && segment->planta_data != NULL) {
                Planta* p = segment->planta_data;
                SDL_Rect src_rect = { p->current_frame * PEASHOOTER_FRAME_WIDTH, 0, PEASHOOTER_FRAME_WIDTH, PEASHOOTER_FRAME_HEIGHT };
                SDL_RenderCopy(renderer, tex_peashooter_sheet, &src_rect, &p->rect);
            }
            segment = segment->next;
        }
    }

    // Dibujo los zombies
    for (int row = 0; row < GRID_ROWS; row++) {
        ZombieNode* zombie_node = board->rows[row].first_zombie;
        
        while (zombie_node != NULL) {
            Zombie* z = &zombie_node->zombie_data;
            
            if (z->activo) {
                SDL_Rect src_rect = { z->current_frame * ZOMBIE_FRAME_WIDTH, 0, ZOMBIE_FRAME_WIDTH, ZOMBIE_FRAME_HEIGHT };
                SDL_RenderCopy(renderer, tex_zombie_sheet, &src_rect, &z->rect);
            }
            
            zombie_node = zombie_node->next;
        }
    }

    // Dibujo las arvejas 
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
    window = SDL_CreateWindow("Plantas vs Zombies - Base para TP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

void testCases(){
    
}

int main(int argc, char* args[]) {
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

        gameBoardUpdate(game_board);
        gameBoardDraw(game_board);

        // TODO: Agregar la lógica para ver si un zombie llegó a la casa y terminó el juego

        SDL_Delay(16);
    }

    gameBoardDelete(game_board);
    cerrar();
    return 0;
}
