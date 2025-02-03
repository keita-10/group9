#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define SIZE 3  // Tic-Tac-Toe board size

char board[SIZE][SIZE]; // Game board
pthread_mutex_t lock;
bool game_over = false; // Flag to track game status
char winner = ' '; // Stores the winner

// Function to initialize the board
void initBoard() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';
}

// Function to print the board
void printBoard() {
    printf("\n-------------\n");
    for (int i = 0; i < SIZE; i++) {
        printf("| ");
        for (int j = 0; j < SIZE; j++) {
            printf("%c | ", board[i][j]);
        }
        printf("\n-------------\n");
    }
}

// Function to check if a player has won
bool checkWinner(char player) {
    for (int i = 0; i < SIZE; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) || // Row check
            (board[0][i] == player && board[1][i] == player && board[2][i] == player))   // Column check
            return true;
    }
    return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||  // Diagonal check
           (board[0][2] == player && board[1][1] == player && board[2][0] == player);
}

// Function to check if the board is full (Draw)
bool isBoardFull() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == ' ')
                return false;
    return true;
}

// Function to generate a random move
void makeMove(char player) {
    while (!game_over) {
        pthread_mutex_lock(&lock); // Locking the board access
        
        if (game_over) { // Recheck after acquiring lock
            pthread_mutex_unlock(&lock);
            return;
        }

        int row, col;
        do {
            row = rand() % SIZE;
            col = rand() % SIZE;
        } while (board[row][col] != ' ');

        board[row][col] = player; // Make the move
        printf("Player %c moves:\n", player);
        printBoard();

        if (checkWinner(player)) {
            printf("Player %c wins! Game Over!\n", player);
            winner = player;
            game_over = true;
            pthread_mutex_unlock(&lock);
            return;
        }

        if (isBoardFull()) {
            printf("It's a draw! Game Over!\n");
            game_over = true;
            pthread_mutex_unlock(&lock);
            return;
        }

        pthread_mutex_unlock(&lock); // Unlocking the board access
        usleep(500000); // Small delay to simulate thinking time
    }
}

// Thread function for Player X
void* playerX(void* arg) {
    while (!game_over) {
        makeMove('X');
    }
    return NULL;
}

// Thread function for Player O
void* playerO(void* arg) {
    while (!game_over) {
        makeMove('O');
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t tX, tO;
    
    pthread_mutex_init(&lock, NULL);
    initBoard();
    printBoard();

    // Create player threads
    pthread_create(&tX, NULL, playerX, NULL);
    pthread_create(&tO, NULL, playerO, NULL);

    // Wait for both players to finish
    pthread_join(tX, NULL);
    pthread_join(tO, NULL);

    pthread_mutex_destroy(&lock);

    printf("\nFinal Board:\n");
    printBoard();
    printf("Game Over. %c won the game!\n", winner);

    return 0;
}
