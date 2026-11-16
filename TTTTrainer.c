#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "Structs.h"
#include "Libraries/cJSON/cJSON.h"

short run(int8_t *valboard) {
     //board setup

    int8_t **board;
    board = malloc(3 * sizeof(int8_t *));
    for (short i = 0; i < 3; i++) {
        board[i] = malloc(3 * sizeof(int8_t));
        for (short j = 0; j < 3; j++) {
            short coord = (i*3) + j;
            board[i][j] = valboard[coord];
        }
    }
    
    int8_t mode = 1;

    //temp board setup
    int8_t **tempboard;
    tempboard = malloc(3 * sizeof(int8_t *));
    for (short i = 0; i < 3; i++) {
        tempboard[i] = malloc(3 * sizeof(int8_t));
        for (short j = 0; j < 3; j++) {
            short coord = i*3+j;

            if (coord == 4) {
                tempboard[i][j] = 8;
            }
            else if (coord % 2 == 0) {
                tempboard[i][j] = 6;
            }
            else {
                tempboard[i][j] = 4;
            }
        }
    }

    int8_t *iused = calloc(3, sizeof(int8_t));
    int8_t *jused = calloc(3, sizeof(int8_t));

    //line cover scoring
    for (short i = 0; i < 3; i++) {
        if (iused[i] == -1) {
            continue;
        }
        for (short j = 0; j < 3; j++) {
            if (jused[j] == -1) {
                continue;
            }
            if (board[i][j] == (mode * -1)) {
                for (short k = 0; k < 3; k++) {
                    tempboard[i][k] -= 2;
                    tempboard[k][j] -= 2;
                    iused[i] = -1;
                    jused[j] = -1;
                }
                if (i == j) {
                    for (short k = 0; k < 3; k++) {
                        if (iused[k] != -1 && jused[k] != -1) {
                            tempboard[k][k] -= 2;
                        }
                    }
                }
                if (j == (2-i)) {
                    for (short k = 0; k < 3; k++) {
                        if (iused[k] != -1 && jused[2-k] != -1) {
                            tempboard[k][2-k] -= 2;
                        }
                    }
                }
            }
        }
    }

    //win and loss checks horizontal
    for (short i = 0; i < 3; i++) {
        short value = 0;
        for (short j = 0; j < 3; j++) {
            if (board[i][j] == mode) {
                value++;
            }
            if(board[i][j] == (mode * -1)) {
                value--;
            }
        }
        if (value == -2) {
            for (short j = 0; j < 3; j++) {
                tempboard[i][j] += 50;
            }
        }
        if (value == 2) {
            for (short j = 0; j < 3; j++) {
                tempboard[i][j] += 75;
            }
        }
    }

    //win and loss checks vertical
    for (short i = 0; i < 3; i++) {
        short value = 0;
        for (short j = 0; j < 3; j++) {
            if (board[j][i] == mode) {
                value++;
            }
            if(board[j][i] == (mode * -1)) {
                value--;
            }
        }
        if (value == -2) {
            for (short j = 0; j < 3; j++) {
                tempboard[j][i] += 50;
            }
        }
        if (value == 2) {
            for (short j = 0; j < 3; j++) {
                tempboard[j][i] += 75;
            }
        }
    }

    //win and loss checks diagnal
    short value = board[0][2]+board[1][1] + board[2][0];
    if (value == 2) {
        for (short j = 0; j < 3; j++) {
            tempboard[j][2-j] += 75;
        }
    }
    if (value == -2) {
        for (short j = 0; j < 3; j++) {
            tempboard[j][2-j] += 50;
        }
    }
    value = board[0][0]+board[1][1] + board[2][2];
    if (value == 2) {
        for (short j = 0; j < 3; j++) {
            tempboard[j][j] += 75;
        }
    }
    if (value == -2) {
        for (short j = 0; j < 3; j++) {
            tempboard[j][j] += 50;
        }
    }

    short ID = 0;
    short max = -1000;
    for (short i = 0; i < 3; i++) {
        for (short j = 0; j < 3; j++) {
            if (board[i][j] != 0) {
                continue;
            }

            if (tempboard[i][j] > max) {
                ID = (i*3) + j;
                max = tempboard[i][j];
            }
        }
    }

    for (short i = 0; i < 3; i++) {
        free(tempboard[i]);
    }
    free(tempboard);
    free(iused);
    free(jused);
    for (short i = 0; i < 3; i++) {
        free(board[i]);
    }
    free(board);

    return ID;
}

void writeEntries(Entry *entries, short runs) {
    cJSON *root = cJSON_CreateArray();

    for (short i = 0; i < runs; i++) {
        cJSON *entry = cJSON_CreateObject();
        cJSON *board = cJSON_CreateArray();

        for (short j = 0; j < 9; j++) {
            cJSON_AddItemToArray(board, cJSON_CreateNumber(entries[i].valboard[j]));
        }

        cJSON_AddItemToObject(entry, "valboard", board);
        cJSON_AddNumberToObject(entry, "ID", entries[i].ID);

        cJSON_AddItemToArray(root, entry);
    }

    char *json = cJSON_Print(root);

    FILE *file = fopen("Entries.json", "w");
    if (file != NULL) {
        fputs(json, file);
        fclose(file);
    }

    free(json);
    cJSON_Delete(root);
}

int genboard(int8_t *valboard) {
    short num = (rand()%4)+1;
    short start = (rand()%2);
    if (start == 0) {
        start = -1;
    }

    for (short i = 0; i < num; i++) {
        while(1) {
            short index = rand()%9;
            if (valboard[index] == 0) {
                valboard[index] = start;
                break;
            }
        }
        if (i == (num - 1) && start == -1) {
            break;
        }
        while(1) {
            short index = rand()%9;
            if (valboard[index] == 0) {
                valboard[index] = -1 * start;
                break;
            }
        }
    }

    return 0;
}

int main() {
    srand(time(NULL));
    short runs = 10;
    Entry *entries = malloc(runs * sizeof(Entry));

    for (short i = 0; i < runs; i++) {
        int8_t *valboard = calloc(9, sizeof(int8_t));
        genboard(valboard);
        short value = run(valboard);
        entries[i].valboard = malloc(9 * sizeof(int8_t));
        memcpy(entries[i].valboard, valboard, 9 * sizeof(int8_t));
        entries[i].ID = value;
        free(valboard);
    }

    writeEntries(entries, runs);
    
    return 0;
}