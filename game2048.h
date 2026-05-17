#ifndef GAME2048_H
#define GAME2048_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

// ================= BẢNG MÀU ANSI NEON =================
#define RESET       "\x1b[0m"
#define BOLD        "\x1b[1m"
#define F_NEON_BLUE "\x1b[38;5;51m"
#define F_NEON_YELLOW "\x1b[38;5;226m"
#define F_NEON_GREEN "\x1b[38;5;46m"
#define F_GREEN     "\x1b[38;5;46m"
#define F_CYAN      "\x1b[38;5;87m"
#define F_MAGENTA   "\x1b[38;5;201m"
#define F_RED       "\x1b[38;5;196m"
#define F_WHITE     "\x1b[38;5;231m"
#define F_YELLOW    "\x1b[38;5;220m"

// ================= BẢNG MÀU DỊU MẮT CHO TIÊU ĐỀ =================
#define F_SOFT_CYAN   "\x1b[38;5;117m"  // Xanh dương pastel dịu (Nửa nét trên)
#define F_SOFT_PURPLE "\x1b[38;5;141m"  // Tím pastel dịu (Nửa nét dưới)

// Màu các ô số
#define C_2    "\x1b[38;5;250m" 
#define C_4    "\x1b[38;5;223m" 
#define C_8    "\x1b[38;5;208m" 
#define C_16   "\x1b[38;5;202m" 
#define C_32   "\x1b[38;5;196m" 
#define C_64   "\x1b[38;5;160m" 
#define C_128  "\x1b[38;5;226m" 
#define C_256  "\x1b[38;5;220m" 
#define C_512  "\x1b[38;5;214m" 
#define C_1024 "\x1b[38;5;226m" 
#define C_2048 "\x1b[38;5;190m" 

// --- TIÊU ĐỀ GAME MỚI: GIÃN CHỮ SIÊU ĐẸP, BỀ THẾ HƠN, KHUNG ĐỒNG BỘ XANH NEON ---
#define GAME_TITLE \
    F_NEON_BLUE "+--------------------------------------------------------+\n" \
    F_NEON_BLUE "|" RESET "               " F_NEON_YELLOW BOLD "T R O   C H O I   2 0 4 8" RESET "                " F_NEON_BLUE "|\n" \
    F_NEON_BLUE "+--------------------------------------------------------+\n" RESET
    
// ================= CẤU TRÚC DỮ LIỆU =================
typedef struct Player {
    char name[50];
    int score;
    struct Player* next;
} Player;

// ================= BIẾN TOÀN CỤC =================
extern int** board;
extern int currentScore;

// ================= NGUYÊN MẪU HÀM =================
void initBoard();
void freeBoard();
void addRandom();
void printBoard();
const char* getColor(int value);
int checkGameOver();
int moveUp();
int moveDown();
int moveLeft();
int moveRight();

Player* loadScoresFromFile();
void saveScoresToFile(Player* head);
Player* addPlayer(Player* head, const char* name, int score);
void sortLeaderboard(Player* head);
void searchPlayer(Player* head, const char* name);
void editPlayer(Player* head, const char* name, int newScore);
Player* deletePlayer(Player* head, const char* name);
int isNameExist(Player* head, const char* name);
void freeList(Player* head);

#endif