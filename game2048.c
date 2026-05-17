#include "game2048.h"

#define F_FRAME "\x1b[38;5;245m" // Màu khung viền bàn cờ (Xám)

int** board = NULL;
int currentScore = 0;

void initBoard() {
    if (board != NULL) {
        freeBoard();
    }
    board = (int**)malloc(4 * sizeof(int*));
    for (int i = 0; i < 4; i++) {
        board[i] = (int*)malloc(4 * sizeof(int));
        for (int j = 0; j < 4; j++) board[i][j] = 0;
    }
    currentScore = 0;
}

void freeBoard() {
    if (board != NULL) {
        for (int i = 0; i < 4; i++) free(board[i]);
        free(board);
        board = NULL;
    }
}

const char* getColor(int value) {
    switch(value) {
        case 2: return C_2;
        case 4: return C_4;
        case 8: return C_8;
        case 16: return C_16;
        case 32: return C_32;
        case 64: return C_64;
        case 128: return C_128;
        case 256: return C_256;
        case 512: return C_512;
        case 1024: return C_1024;
        case 2048: return C_2048;
        default: return value > 2048 ? "\x1b[38;5;199m" : RESET;
    }
}

void addRandom() {
    int emptyCells[16][2];
    int emptyCount = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == 0) {
                emptyCells[emptyCount][0] = i;
                emptyCells[emptyCount][1] = j;
                emptyCount++;
            }
        }
    }
    if (emptyCount > 0) {
        int rIndex = rand() % emptyCount;
        board[emptyCells[rIndex][0]][emptyCells[rIndex][1]] = (rand() % 10 == 0) ? 4 : 2;
    }
}

void printBoard() {
    // THAY ĐỔI QUAN TRỌNG: Dùng mã ANSI đưa con trỏ về góc trái trên cùng
    // Tuyệt đối không dùng system("cls") ở đây nữa để chống nháy màn hình!
    printf("\x1b[H"); 
    
    printf("%s", GAME_TITLE); 
    // Thêm vài khoảng trắng phía sau %d để đảm bảo xóa sạch dư ảnh của số cũ nếu số mới ngắn hơn
    printf(BOLD "Diem cua ban: %d          " RESET "\n\n", currentScore); 
    
    for (int i = 0; i < 4; i++) {
        printf("%s+----+----+----+----+%s\n", F_FRAME, RESET);
        for (int j = 0; j < 4; j++) {
            printf("%s|%s", F_FRAME, RESET);
            if (board[i][j] == 0) {
                printf("    ");
            } else {
                printf("%s%4d%s", getColor(board[i][j]), board[i][j], RESET);
            }
        }
        printf("%s|%s\n", F_FRAME, RESET);
    }
    printf("%s+----+----+----+----+%s\n", F_FRAME, RESET);
}

int slideArray(int arr[4]) {
    int writePos = 0, moved = 0, lastMerged = -1;
    for (int i = 0; i < 4; i++) {
        if (arr[i] != 0) {
            if (writePos > 0 && arr[writePos - 1] == arr[i] && lastMerged != writePos - 1) {
                arr[writePos - 1] *= 2;
                currentScore += arr[writePos - 1]; 
                arr[i] = 0;
                lastMerged = writePos - 1;
                moved = 1;
            } else {
                if (writePos != i) {
                    arr[writePos] = arr[i];
                    arr[i] = 0;
                    moved = 1;
                }
                writePos++;
            }
        }
    }
    return moved;
}

int moveLeft() {
    int moved = 0;
    for (int i = 0; i < 4; i++) if (slideArray(board[i])) moved = 1;
    return moved;
}

int moveRight() {
    int moved = 0, temp[4];
    for (int i = 0; i < 4; i++) {
        for(int j=0; j<4; j++) temp[j] = board[i][3-j];
        if (slideArray(temp)) {
            for(int j=0; j<4; j++) board[i][3-j] = temp[j];
            moved = 1;
        }
    }
    return moved;
}

int moveUp() {
    int moved = 0, temp[4];
    for (int j = 0; j < 4; j++) {
        for(int i=0; i<4; i++) temp[i] = board[i][j];
        if (slideArray(temp)) {
            for(int i=0; i<4; i++) board[i][j] = temp[i];
            moved = 1;
        }
    }
    return moved;
}

int moveDown() {
    int moved = 0, temp[4];
    for (int j = 0; j < 4; j++) {
        for(int i=0; i<4; i++) temp[i] = board[3-i][j];
        if (slideArray(temp)) {
            for(int i=0; i<4; i++) board[3-i][j] = temp[i];
            moved = 1;
        }
    }
    return moved;
}

int checkGameOver() {
    int i, j; 
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (board[i][j] == 0) return 0;
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (j < 3 && board[i][j] == board[i][j+1]) return 0;
            if (i < 3 && board[i][j] == board[i+1][j]) return 0;
        }
    }
    return 1; 
}

int isNameExist(Player* head, const char* name) {
    Player* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            return 1; 
        }
        temp = temp->next;
    }
    return 0; 
}

Player* loadScoresFromFile() {
    Player* head = NULL;
    FILE* file = fopen("scores.txt", "r");
    if (file == NULL) return NULL; 
    
    char name[50];
    int score;
    while (fscanf(file, "%49s %d", name, &score) == 2) {
        head = addPlayer(head, name, score);
    }
    fclose(file);
    return head;
}

void saveScoresToFile(Player* head) {
    FILE* file = fopen("scores.txt", "w");
    if (file == NULL) {
        printf(F_RED "Loi: Khong the mo file de ghi du lieu!" RESET "\n");
        return;
    }
    Player* temp = head;
    while (temp != NULL) {
        fprintf(file, "%s %d\n", temp->name, temp->score);
        temp = temp->next;
    }
    fclose(file);
}

Player* addPlayer(Player* head, const char* name, int score) {
    Player* newNode = (Player*)malloc(sizeof(Player));
    if (newNode == NULL) return head;
    strcpy(newNode->name, name);
    newNode->score = score;
    newNode->next = head; 
    return newNode;
}

Player* deletePlayer(Player* head, const char* name) {
    Player *temp = head, *prev = NULL;
    if (temp != NULL && strcmp(temp->name, name) == 0) {
        head = temp->next;
        free(temp);
        return head;
    }
    while (temp != NULL && strcmp(temp->name, name) != 0) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) {
        return head;
    }
    prev->next = temp->next;
    free(temp);
    return head;
}

void editPlayer(Player* head, const char* name, int newScore) {
    Player* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            temp->score = newScore;
            return;
        }
        temp = temp->next;
    }
}

void searchPlayer(Player* head, const char* name) {
    if (head == NULL) {
        printf(F_YELLOW "Khong tim thay '%s' trong bang xep hang." RESET "\n", name);
        return;
    }
    if (strcmp(head->name, name) == 0) {
        printf(F_CYAN BOLD "=> Tim thay: [%s] dang co so diem la: %d" RESET "\n", head->name, head->score);
        return;
    }
    searchPlayer(head->next, name);
}

void freeList(Player* head) {
    if (head == NULL) return;
    freeList(head->next);
    free(head);
}

void sortLeaderboard(Player* head) {
    if (head == NULL) return;
    int swapped;
    Player* ptr1;
    Player* lptr = NULL;

    do {
        swapped = 0;
        ptr1 = head;
        while (ptr1->next != lptr) {
            if (ptr1->score < ptr1->next->score) { 
                int tempScore = ptr1->score;
                ptr1->score = ptr1->next->score;
                ptr1->next->score = tempScore;
                char tempName[50];
                strcpy(tempName, ptr1->name);
                strcpy(ptr1->name, ptr1->next->name);
                strcpy(ptr1->next->name, tempName);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}