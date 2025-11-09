void gameBoardDelete(GameBoard* board) {
    // TODO: Liberar toda la memoria dinámica.
    // TODO: Recorrer cada GardenRow.
    // TODO: Liberar todos los RowSegment (y los planta_data si existen).
    // TODO: Liberar todos los ZombieNode.
    // TODO: Finalmente, liberar el GameBoard.
    if (!board) return;
    
    for (int i = 0; i < GRID_ROWS; i++) {
        RowSegment* mySegment = board->rows[i].first_segment;
        while (mySegment != NULL) {
            RowSegment* next_segment = mySegment->next;
            
            if (mySegment->planta_data != NULL) {
                free(mySegment->planta_data);
            }
            
            free(mySegment);
            mySegment = next_segment;
        }
        
        ZombieNode* zombieNode = board->rows[i].first_zombie;
        while (zombieNode != NULL) {
            ZombieNode* next_zombie = zombieNode->next;
            free(zombieNode);
            zombieNode = next_zombie;
        }
    }
    
    free(board);
}

void gameBoardRemovePlant(GameBoard* board, int row, int col) {
    // TODO: Similar a AddPlant, encontrar el segmento que contiene `col`.
    // TODO: Si es un segmento de tipo PLANTA, convertirlo a VACIO y liberar el `planta_data`.
    // TODO: Implementar la lógica de FUSIÓN con los segmentos vecinos si también son VACIO.
    if (!board) return;
    
    RowSegment* mySegment = board->rows[row].first_segment;
    RowSegment* prev_segment = NULL;
    
    while (mySegment != NULL) {
        int seg_end = mySegment->start_col + mySegment->length;
        
        if (mySegment->start_col <= col && col < seg_end) {
            if (mySegment->status != STATUS_PLANTA) {
                printf("No hay planta en esta celda para remover.\n");
                return;
            }
            
            if (mySegment->planta_data != NULL) {
                free(mySegment->planta_data);
                mySegment->planta_data = NULL;
            }
            
            mySegment->status = STATUS_VACIO;
            
            if (mySegment->next != NULL && mySegment->next->status == STATUS_VACIO) {
                RowSegment* next_seg = mySegment->next;
                mySegment->length += next_seg->length;
                mySegment->next = next_seg->next;
                free(next_seg);
            }
            
            if (prev_segment != NULL && prev_segment->status == STATUS_VACIO) {
                prev_segment->length += mySegment->length;
                prev_segment->next = mySegment->next;
                free(mySegment);
                mySegment = prev_segment;
            }
            
            printf("Planta removida en [%d,%d]\n", row, col);
            break;
        }
        
        prev_segment = mySegment;
        mySegment = mySegment->next;
    }
    
    printf("No se encontró segmento que contenga la columna %d\n", col);
}

int gameBoardAddPlant(GameBoard* board, int row, int col) {
    // TODO: Encontrar la GardenRow correcta. ✓
    // TODO: Recorrer la lista de RowSegment hasta encontrar el segmento VACIO que contenga a `col`. ✓
    // TODO: Si se encuentra y tiene espacio, realizar la lógica de DIVISIÓN de segmento.
    // TODO: Crear la nueva `Planta` con memoria dinámica y asignarla al `planta_data` del nuevo segmento.

    RowSegment* mySegment = board->rows[row].first_segment;
    RowSegment* prev_segment = NULL;
    
    while (mySegment != NULL) {
        int seg_end = mySegment->start_col + mySegment->length;
        
        if (mySegment->start_col <= col && col < seg_end) {
            if (mySegment->status != STATUS_VACIO) {
                gameBoardRemovePlant(board, row, col);
                return 0;
            }
            
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
            
            if (mySegment->length == 1) {
                mySegment->status = STATUS_PLANTA;
                mySegment->planta_data = new_plant;
                printf("Planta agregada en segmento de 1 celda [%d,%d]\n", row, col);
                return 1;
            }
            
            if (col == mySegment->start_col) {
                RowSegment* nuevo_seg = (RowSegment*)malloc(sizeof(RowSegment));
                if (!nuevo_seg) {
                    free(new_plant);
                    return 0;
                }
                nuevo_seg->status = STATUS_PLANTA;
                nuevo_seg->start_col = col;
                nuevo_seg->length = 1;
                nuevo_seg->planta_data = new_plant;
                nuevo_seg->next = mySegment;
                
                mySegment->start_col = col + 1;
                mySegment->length--;
                
                if (prev_segment == NULL) {
                    board->rows[row].first_segment = nuevo_seg;
                } else {
                    prev_segment->next = nuevo_seg;
                }
                
                printf("Planta agregada al inicio del segmento [%d,%d]\n", row, col);
                return 1;
            }
            
            if (col == seg_end - 1) {
                RowSegment* nuevo_seg = (RowSegment*)malloc(sizeof(RowSegment));
                if (!nuevo_seg) {
                    free(new_plant);
                    return 0;
                }
                nuevo_seg->status = STATUS_PLANTA;
                nuevo_seg->start_col = col;
                nuevo_seg->length = 1;
                nuevo_seg->planta_data = new_plant;
                nuevo_seg->next = mySegment->next;
                
                mySegment->length--;
                mySegment->next = nuevo_seg;
                
                printf("Planta agregada al final del segmento [%d,%d]\n", row, col);
                return 1;
            }
            
            RowSegment* seg_planta = (RowSegment*)malloc(sizeof(RowSegment));
            RowSegment* seg_derecha = (RowSegment*)malloc(sizeof(RowSegment));
            if (!seg_planta || !seg_derecha) {
                free(new_plant);
                if (seg_planta) free(seg_planta);
                if (seg_derecha) free(seg_derecha);
                return 0;
            }
            
            seg_planta->status = STATUS_PLANTA;
            seg_planta->start_col = col;
            seg_planta->length = 1;
            seg_planta->planta_data = new_plant;
            
            seg_derecha->status = STATUS_VACIO;
            seg_derecha->start_col = col + 1;
            seg_derecha->length = seg_end - (col + 1);
            seg_derecha->planta_data = NULL;
            seg_derecha->next = mySegment->next;
            
            mySegment->length = col - mySegment->start_col;
            mySegment->next = seg_planta;
            seg_planta->next = seg_derecha;
            
            printf("Planta agregada dividiendo segmento en 3 [%d,%d]\n", row, col);
            return 1;
        }
        
        prev_segment = mySegment;
        mySegment = mySegment->next;
    }
    
    printf("No se encontró segmento que contenga la columna %d\n", col);
    return 0;
}

void gameBoardAddZombie(GameBoard* board, int row) {
    // TODO: Crear un nuevo ZombieNode con memoria dinámica.
    // TODO: Inicializar sus datos (posición, vida, animación, etc.).
    // TODO: Agregarlo a la lista enlazada simple de la GardenRow correspondiente.
    if (!board) return;
    
    ZombieNode* newZombie = (ZombieNode*)malloc(sizeof(ZombieNode));
    if (!newZombie) {
        printf("Error: No se pudo asignar memoria para el zombie.\n");
        return;
    }

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

    if (board->rows[row].first_zombie == NULL) {
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
    // TODO: Re-implementar la lógica de `actualizarEstado` usando las nuevas estructuras.
    // TODO: Recorrer las listas de zombies de cada fila para moverlos y animarlos.
    // TODO: Recorrer las listas de segmentos de cada fila para gestionar los cooldowns y animaciones de las plantas.
    // TODO: Actualizar la lógica de disparo, colisiones y spawn de zombies.

    if (!board) return 0;

    for (int row = 0; row < GRID_ROWS; row++) {
        ZombieNode* zombieNode = board->rows[row].first_zombie;
        ZombieNode* prev_zombie = NULL;

        while (zombieNode != NULL) {
            Zombie* z = &zombieNode->zombie_data;
            
            if (z->activo) {
                float distance_per_tick = ZOMBIE_DISTANCE_PER_CYCLE / (float)(ZOMBIE_TOTAL_FRAMES * ZOMBIE_ANIMATION_SPEED);
                z->pos_x -= distance_per_tick;
                z->rect.x = (int)z->pos_x;
                
                if (z->rect.x < 130) {
                    printf("¡GAME OVER! Un zombie llegó a tu casa en la fila %d\n", row);
                    return 1;
                }
                
                z->frame_timer++;
                if (z->frame_timer >= ZOMBIE_ANIMATION_SPEED) {
                    z->frame_timer = 0;
                    z->current_frame = (z->current_frame + 1) % ZOMBIE_TOTAL_FRAMES;
                }
            }

            if (!z->activo) {
                ZombieNode* to_delete = zombieNode;
                if (prev_zombie == NULL) {
                    board->rows[row].first_zombie = zombieNode->next;
                    zombieNode = zombieNode->next;
                } else {
                    prev_zombie->next = zombieNode->next;
                    zombieNode = zombieNode->next;
                }
                free(to_delete);
            } else {
                prev_zombie = zombieNode;
                zombieNode = zombieNode->next;
            }
        }
    }

    for (int row = 0; row < GRID_ROWS; row++) {
        RowSegment* mySegment = board->rows[row].first_segment;
        
        while (mySegment != NULL) {
            if (mySegment->status == STATUS_PLANTA && mySegment->planta_data != NULL) {
                Planta* p = mySegment->planta_data;
                
                if (p->cooldown > 0) {
                    p->cooldown--;
                } else {
                    p->debe_disparar = 1;
                }
                
                p->frame_timer++;
                if (p->frame_timer >= PEASHOOTER_ANIMATION_SPEED) {
                    p->frame_timer = 0;
                    p->current_frame = (p->current_frame + 1) % PEASHOOTER_TOTAL_FRAMES;
                    
                    if (p->debe_disparar && p->current_frame == PEASHOOTER_SHOOT_FRAME) {
                        for (int i = 0; i < MAX_ARVEJAS; i++) {
                            if (!board->arvejas[i].activo) {
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

    for (int i = 0; i < MAX_ARVEJAS; i++) {
        if (board->arvejas[i].activo) {
            board->arvejas[i].rect.x += PEA_SPEED;
            
            if (board->arvejas[i].rect.x > SCREEN_WIDTH) {
                board->arvejas[i].activo = 0;
            }
        }
    }

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

    board->zombie_spawn_timer--;
    if (board->zombie_spawn_timer <= 0) {
        int random_row = rand() % GRID_ROWS;
        gameBoardAddZombie(board, random_row);
        board->zombie_spawn_timer = ZOMBIE_SPAWN_RATE;
    }
    
    return 0;
}

void gameBoardDraw(GameBoard* board) {
    // TODO: Re-implementar la lógica de `dibujar` usando las nuevas estructuras.
    // TODO: Recorrer las listas de segmentos para dibujar las plantas.
    // TODO: Recorrer las listas de zombies para dibujarlos.
    // TODO: Dibujar las arvejas y el cursor.
    if (!board) return;
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, tex_background, NULL, NULL);

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

void casosDeTest(){
    
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

        // TODO: Agregar la lógica para ver si un zombie llegó a la casa y terminó el juego
        int is_game_over = gameBoardUpdate(game_board);
        if (is_game_over) {
            game_over = 1;
        }
        gameBoardDraw(game_board);

        SDL_Delay(16);
    }

    gameBoardDelete(game_board);
    cerrar();
    return 0;
}