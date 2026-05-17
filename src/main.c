#include "game2048.h"

// =========================================================================
// [BỘ HÀM XỬ LÝ ĐỘC LẬP CHO CHẾ ĐỘ 1VS1 (KHÔNG ẢNH HƯỞNG ĐẾN GAME GỐC)]
// =========================================================================

// Hàm trả về chuỗi chứa mã màu tương ứng cho từng số
static const char* getColorCode(int value) {
    switch (value) {
        case 2:    return "\x1b[38;5;250m";   // Xám nhạt
        case 4:    return "\x1b[38;5;186m";   // Be / Vàng nhạt
        case 8:    return "\x1b[38;5;208m";   // Cam sáng
        case 16:   return "\x1b[38;5;202m";   // Cam đậm / Đỏ cam
        case 32:   return "\x1b[38;5;196m";   // Đỏ
        case 64:   return "\x1b[38;5;9m";     // Đỏ rực
        case 128:  return "\x1b[38;5;220m";   // Vàng chanh
        case 256:  return "\x1b[38;5;220m";   // Vàng 
        case 512:  return "\x1b[38;5;220m";   
        case 1024: return "\x1b[38;5;220m";
        case 2048: return "\x1b[38;5;220m";
        case 4096: return "\x1b[38;5;160m";   // Đỏ thẫm
        case 8192: return "\x1b[38;5;160m";   
        default:   return "\x1b[38;5;255m";   // Trắng
    }
}

static void rot_1v1(int b[4][4]) {
    int t[4][4];
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            t[j][3-i] = b[i][j]; 
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            b[i][j] = t[i][j];
}

static int mvLeft_1v1(int b[4][4], int *score) {
    int moved = 0;
    for(int i=0; i<4; i++) {
        int pos = 0;
        for(int j=0; j<4; j++) {
            if(b[i][j] != 0) {
                if(j != pos) { b[i][pos] = b[i][j]; b[i][j] = 0; moved = 1; }
                pos++;
            }
        }
        for(int j=0; j<3; j++) {
            if(b[i][j] != 0 && b[i][j] == b[i][j+1]) {
                b[i][j] *= 2; *score += b[i][j]; b[i][j+1] = 0; moved = 1;
            }
        }
        pos = 0;
        for(int j=0; j<4; j++) {
            if(b[i][j] != 0) {
                if(j != pos) { b[i][pos] = b[i][j]; b[i][j] = 0; moved = 1; }
                pos++;
            }
        }
    }
    return moved;
}

static int mv_1v1(int b[4][4], int *score, int dir) {
    for(int i=0; i<dir; i++) rot_1v1(b);
    int moved = mvLeft_1v1(b, score);
    for(int i=0; i<(4-dir)%4; i++) rot_1v1(b);
    return moved;
}

static void addRnd_1v1(int b[4][4]) {
    int empty[16][2], count = 0;
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            if(b[i][j] == 0) { empty[count][0] = i; empty[count][1] = j; count++; }
    if(count > 0) {
        int r = rand() % count;
        b[empty[r][0]][empty[r][1]] = (rand() % 10 < 9) ? 2 : 4;
    }
}

static int isOver_1v1(int b[4][4]) {
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            if(b[i][j] == 0) return 0;
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++) {
            if(j<3 && b[i][j] == b[i][j+1]) return 0;
            if(i<3 && b[i][j] == b[i+1][j]) return 0;
        }
    return 1;
}

static void printDualBoards(int b1[4][4], int s1, int st1, char* name1, int b2[4][4], int s2, int st2, char* name2) {
    printf("\x1b[H"); 
    
    // In tiêu đề, tên người chơi, điểm
    printf("%s%s       [ %s - WASD ]                           [ %s - Mui Ten ]\n%s", BOLD, F_CYAN, name1, name2, RESET);
    
    // In dòng điểm, căn chỉnh chuẩn xác không bị tràn màu
    printf("%s%s       Diem: %-7d ", BOLD, F_YELLOW, s1);
    if (!st1) {
        printf("%s(THUA)    %s", F_RED, F_YELLOW); 
    } else {
        printf("          "); 
    }
    
    printf("                  Diem: %-7d ", s2);
    if (!st2) {
        printf("%s(THUA)    %s\n%s", F_RED, F_YELLOW, RESET);
    } else {
        printf("          \n%s", RESET);
    }
    
    printf("\n");
    
    const char* border_color = "\x1b[38;5;244m"; 

    for(int i=0; i<4; i++) {
        // Dòng biên trên của mỗi hàng
        printf("  %s+------+------+------+------+%s           %s+------+------+------+------+%s\n", border_color, RESET, border_color, RESET);
        
        // Cột bên trái của P1
        printf("  %s|%s", border_color, RESET);
        
        // In nội dung P1
        for(int j=0; j<4; j++) {
            if(b1[i][j] == 0) {
                printf("      %s|%s", border_color, RESET);
            } else {
                printf("%s%s%5d %s%s|%s", BOLD, getColorCode(b1[i][j]), b1[i][j], RESET, border_color, RESET);
            }
        }
        
        // Khoảng cách giữa 2 board + cột bên trái của P2
        printf("           %s|%s", border_color, RESET);
        
        // In nội dung P2
        for(int j=0; j<4; j++) {
            if(b2[i][j] == 0) {
                printf("      %s|%s", border_color, RESET);
            } else {
                printf("%s%s%5d %s%s|%s", BOLD, getColorCode(b2[i][j]), b2[i][j], RESET, border_color, RESET);
            }
        }
        printf("\n");
    }
    // Dòng biên dưới cùng
    printf("  %s+------+------+------+------+%s           %s+------+------+------+------+%s\n", border_color, RESET, border_color, RESET);
    
    printf("\n");
    printf("%s%s  [ Phim R: P1 Dau Hang ]                       [ Phim O: P2 Dau Hang ]\n%s", BOLD, F_WHITE, RESET);
}
// =========================================================================

int main() {
    system(""); 
    
    srand((unsigned int)time(0));
    Player* leaderboard = loadScoresFromFile();
    int choice = -1;
    int menu_index = 0; 

    system("cls");

    do {
        while (1) {
            printf("\x1b[H");
            printf("%s", GAME_TITLE);
            
            printf("%s+--------------------------------------------------------+%s\n", F_NEON_BLUE, RESET);
            printf("%s|                  %s%sHE THONG DIEU KHIEN%s                   %s|%s\n", F_NEON_BLUE, BOLD, F_NEON_YELLOW, RESET, F_NEON_BLUE, RESET);
            printf("%s+--------------------------------------------------------+%s\n", F_NEON_BLUE, RESET);
            
            if (menu_index == 0) printf("%s|\x1b[47;30m    " BOLD F_NEON_BLUE "[1]" RESET "\x1b[47;30m Choi Game 2048 (WASD)                           \x1b[0m%s|\n", F_NEON_BLUE, F_NEON_BLUE);
            else printf("%s|    " BOLD F_NEON_BLUE "[1]" RESET " %s%sChoi Game 2048 (WASD)%s                           %s|\n", F_NEON_BLUE, BOLD, F_WHITE, RESET, F_NEON_BLUE);

            if (menu_index == 1) printf("%s|\x1b[47;30m    " BOLD F_NEON_BLUE "[2]" RESET "\x1b[47;30m Bang xep hang (DSLK)                            \x1b[0m%s|\n", F_NEON_BLUE, F_NEON_BLUE);
            else printf("%s|    " BOLD F_NEON_BLUE "[2]" RESET " %s%sBang xep hang (DSLK)%s                            %s|\n", F_NEON_BLUE, BOLD, F_WHITE, RESET, F_NEON_BLUE);

            if (menu_index == 2) printf("%s|\x1b[47;30m    " BOLD F_NEON_BLUE "[3]" RESET "\x1b[47;30m Tim kiem nguoi choi (De quy)                    \x1b[0m%s|\n", F_NEON_BLUE, F_NEON_BLUE);
            else printf("%s|    " BOLD F_NEON_BLUE "[3]" RESET " %s%sTim kiem nguoi choi (De quy)%s                    %s|\n", F_NEON_BLUE, BOLD, F_WHITE, RESET, F_NEON_BLUE);

            if (menu_index == 3) printf("%s|\x1b[47;30m    " BOLD F_NEON_BLUE "[4]" RESET "\x1b[47;30m Sua diem Admin (Sua node)                       \x1b[0m%s|\n", F_NEON_BLUE, F_NEON_BLUE);
            else printf("%s|    " BOLD F_NEON_BLUE "[4]" RESET " %s%sSua diem Admin (Sua node)%s                       %s|\n", F_NEON_BLUE, BOLD, F_WHITE, RESET, F_NEON_BLUE);

            if (menu_index == 4) printf("%s|\x1b[47;30m    " BOLD F_NEON_BLUE "[5]" RESET "\x1b[47;30m Xoa nguoi choi (Xoa node)                       \x1b[0m%s|\n", F_NEON_BLUE, F_NEON_BLUE);
            else printf("%s|    " BOLD F_NEON_BLUE "[5]" RESET " %s%sXoa nguoi choi (Xoa node)%s                       %s|\n", F_NEON_BLUE, BOLD, F_WHITE, RESET, F_NEON_BLUE);

            if (menu_index == 5) printf("%s|\x1b[47;30m    " BOLD F_NEON_BLUE "[6]" RESET "\x1b[47;30m Tips choi Game 2048 (Pro)                       \x1b[0m%s|\n", F_NEON_BLUE, F_NEON_BLUE);
            else printf("%s|    " BOLD F_NEON_BLUE "[6]" RESET " %s%sTips choi Game 2048 (Pro)%s                       %s|\n", F_NEON_BLUE, BOLD, F_WHITE, RESET, F_NEON_BLUE);

            if (menu_index == 6) printf("%s|\x1b[47;30m    " BOLD F_NEON_BLUE "[7]" RESET "\x1b[47;30m Solo 1-1 (Ban phim chung)                       \x1b[0m%s|\n", F_NEON_BLUE, F_NEON_BLUE);
            else printf("%s|    " BOLD F_NEON_BLUE "[7]" RESET " %s%sSolo 1-1 (Ban phim chung)%s                       %s|\n", F_NEON_BLUE, BOLD, F_WHITE, RESET, F_NEON_BLUE);

            if (menu_index == 7) printf("%s|\x1b[47;30m    " BOLD F_NEON_BLUE "[0]" RESET "\x1b[47;30m Luu va Thoat (Ghi file)                         \x1b[0m%s|\n", F_NEON_BLUE, F_NEON_BLUE);
            else printf("%s|    " BOLD F_NEON_BLUE "[0]" RESET " %s%sLuu va Thoat (Ghi file)%s                         %s|\n", F_NEON_BLUE, BOLD, F_RED, RESET, F_NEON_BLUE);

            printf("%s+--------------------------------------------------------+%s\n", F_NEON_BLUE, RESET);
            printf("\n%s%sHuong dan:%s Su dung phim Mui Ten %sLen/Xuong%s va nhan %s%sEnter%s de chon.\n", BOLD, F_CYAN, RESET, BOLD, RESET, BOLD, F_NEON_GREEN, RESET);

            int ch = _getch();
            if (ch == 224) { 
                ch = _getch();
                if (ch == 72) menu_index = (menu_index - 1 + 8) % 8;
                else if (ch == 80) menu_index = (menu_index + 1) % 8;     
            } else if (ch == 13) { 
                if (menu_index == 0) choice = 1;
                else if (menu_index == 1) choice = 2;
                else if (menu_index == 2) choice = 3;
                else if (menu_index == 3) choice = 4;
                else if (menu_index == 4) choice = 5;
                else if (menu_index == 5) choice = 6;
                else if (menu_index == 6) choice = 7; 
                else if (menu_index == 7) choice = 0; 
                break; 
            }
        }

        if (choice == 1) {
            char playerName[50];
            system("cls");
            printf("%s", GAME_TITLE);
            while (1) {
                printf("%s%sNhap ten cua ban truoc khi choi (viet khong dau): %s", BOLD, F_CYAN, RESET);
                scanf("%49s", playerName);
                if (isNameExist(leaderboard, playerName)) {
                    printf("\a%s%s=> Ten nay da ton tai trong he thong! Vui long nhap ten khac.\n\n%s", BOLD, F_YELLOW, RESET);
                } else break; 
            }
            initBoard(); addRandom(); addRandom(); system("cls"); 
            while (1) {
                printBoard();
                if (checkGameOver()) {
                    printf("\a\n"); 
                    printf("%s%s  ========================================\n%s", BOLD, F_RED, RESET);
                    printf("%s%s            G A M E   O V E R !           \n%s", BOLD, F_WHITE, RESET);
                    printf("%s%s            Diem cua ban: %-5d           \n%s", BOLD, F_YELLOW, currentScore, RESET);
                    printf("%s%s  ========================================\n%s", BOLD, F_RED, RESET);
                    leaderboard = addPlayer(leaderboard, playerName, currentScore);
                    printf("\n%s%s=> Nhan phim 'q' de quay lai Menu chinh...%s", BOLD, F_CYAN, RESET);
                    while (1) { char ch = _getch(); if (ch == 'q' || ch == 'Q') break; else printf("\a"); }
                    break; 
                }
                char move = _getch(); int moved = 0;
                if (move == 'w' || move == 'W') moved = moveUp();
                else if (move == 's' || move == 'S') moved = moveDown();
                else if (move == 'a' || move == 'A') moved = moveLeft();
                else if (move == 'd' || move == 'D') moved = moveRight();
                else if (move == 'q' || move == 'Q') {
                    printf("\n%s%s  [CANH BAO] Ban co chac chan muon roi khoi tro choi khong?\n%s", BOLD, F_RED, RESET);
                    printf("%s%s  -> Tran dau se bi HUY, toan bo %d diem hien tai se BI MAT vinh vien!\n%s", BOLD, F_YELLOW, currentScore, RESET);
                    printf("%s%s  Xac nhan roi khoi tran dau? (y/n): %s", BOLD, F_WHITE, RESET);
                    int should_break = 0;
                    while (1) {
                        char confirm = _getch();
                        if (confirm == 'y' || confirm == 'Y') { should_break = 1; break; } 
                        else if (confirm == 'n' || confirm == 'N') { should_break = 0; break; }
                        else printf("\a");
                    }
                    if (should_break) break; else system("cls");
                }
                if (moved) addRandom();
            }
            freeBoard(); system("cls"); 
        } 
        else if (choice == 2) {
            system("cls"); printf("%s", GAME_TITLE); printf("%s%s--- BANG XEP HANG QUOC TE ---%s\n\n", F_CYAN, BOLD, RESET);
            if (leaderboard == NULL) printf("%sChua co du lieu nguoi choi.%s\n", F_YELLOW, RESET);
            else {
                sortLeaderboard(leaderboard); Player* temp = leaderboard; int rank = 1;
                while (temp != NULL) {
                    printf("%s%2d.%s Nguoi choi: %s%-15s%s | Diem: %s%d%s\n", F_NEON_BLUE, rank++, RESET, BOLD, temp->name, RESET, F_NEON_GREEN, temp->score, RESET);
                    temp = temp->next;
                }
            }
            printf("\n"); system("pause"); system("cls"); 
        }
        else if (choice >= 3 && choice <= 5) {
            system("cls");
            if (choice == 4 || choice == 5) {
                char inputPass[20];
                printf("%s%sVi tri nay can quyen Admin. Vui long nhap mat khau: %s", BOLD, F_YELLOW, RESET);
                scanf("%19s", inputPass);
                if (strcmp(inputPass, "TTHK") != 0) {
                    printf("\a%s%s=> Sai mat khau Admin! Ban khong co quyen truy cap.\n%s\n", BOLD, F_RED, RESET); system("pause"); system("cls"); continue; 
                }
                printf("%s%s=> Xac minh Admin thanh cong!\n\n%s", BOLD, F_GREEN, RESET);
            }
            char searchName[50]; printf("Nhap ten nguoi choi: "); scanf("%49s", searchName);
            if (choice == 3) { printf("\nKet qua tim kiem bang De quy:\n"); searchPlayer(leaderboard, searchName); }
            else if (choice == 4) {
                if (!isNameExist(leaderboard, searchName)) printf("\a%s%sLoi: Khong tim thay nguoi choi '%s' de sua diem!%s\n", BOLD, F_RED, searchName, RESET);
                else {
                    int newScore; printf("Nhap diem moi: ");
                    if (scanf("%d", &newScore) != 1) { printf("\a%s%sLoi: Diem phai la chu so!%s\n", BOLD, F_RED, RESET); while (getchar() != '\n'); }
                    else { editPlayer(leaderboard, searchName, newScore); printf("%s%s=> Da cap nhat diem thanh cong!%s\n", BOLD, F_GREEN, RESET); }
                }
            }
            else if (choice == 5) {
                if (!isNameExist(leaderboard, searchName)) printf("\a%s%sLoi: Khong tim thay nguoi choi '%s' de xoa!%s\n", BOLD, F_RED, searchName, RESET);
                else {
                    printf("%s%sCANH BAO: Ban co chac chan muon xoa '%s' vinh vien? (y/n): %s", BOLD, F_RED, searchName, RESET); char confirm; scanf(" %c", &confirm);
                    if(confirm == 'y' || confirm == 'Y') { leaderboard = deletePlayer(leaderboard, searchName); printf("%s%s=> Da xoa nguoi choi thanh cong!%s\n", BOLD, F_GREEN, RESET); }
                    else printf("%s%s=> Da huy lenh xoa!%s\n", BOLD, F_YELLOW, RESET);
                }
            }
            printf("\n"); system("pause"); system("cls"); 
        }
        else if (choice == 6) {
            system("cls"); printf("%s", GAME_TITLE);
            printf("%s%s               [ BI KIP DAT DIEM CAO 2048 ]\n\n%s", BOLD, F_NEON_YELLOW, RESET);
            printf("%s%s1. QUY TAC GOC (Goc 'Than Thanh'):\n%s", BOLD, F_CYAN, RESET);
            printf("   - Luon giu khoi co gia tri cao nhat o mot goc co dinh (Vd: Goc duoi cung ben phai).\n");
            printf("   - Han che toi da viec bam phim di chuyen khoi ve huong nguoc lai goc nay.\n\n");
            printf("%s%s2. QUY TAC CHU 'U' (Xay 'Kim Tu Thap'):\n%s", BOLD, F_CYAN, RESET);
            printf("   - Sap xep cac khoi nho hon bao quanh khoi lon nhat theo hinh chu U.\n");
            printf("   - Vi du: Goc la 1024, canh no la 512, tiep theo la 256. Dieu nay giup gop so cuc le.\n\n");
            printf("%s%s3. DON HANG (Lap day dong chua khoi max):\n%s", BOLD, F_CYAN, RESET);
            printf("   - Luon giu cho dong (hoac cot) chua khoi lon nhat kin cho (full 4 khoi).\n");
            printf("   - Neu dong do bi trong, khoi moi sinh ra co the chen vao day khoi max ra khoi goc.\n\n");
            printf("%s%s4. KIEN NHAN VA NHIN XA:\n%s", BOLD, F_CYAN, RESET);
            printf("   - Dung voi vang gop so ngay khi co the. Hay tinh truoc 2-3 buoc de tao ra\n");
            printf("     chuoi Combo gop so day chuyen lien hoan (Chain reaction).\n\n");
            printf("%s%s=> Chuc ban som pha dao duoc o 2048 va leo len TOP 1 Bang Xep Hang!\n\n%s", BOLD, F_NEON_GREEN, RESET);
            system("pause"); system("cls");
        }
        else if (choice == 7) {
            char name1[50], name2[50];
            system("cls");
            printf("%s", GAME_TITLE);
            
            printf("%s%sNhap ten Player 1 (viet khong dau): %s", BOLD, F_CYAN, RESET);
            scanf("%49s", name1);
            printf("%s%sNhap ten Player 2 (viet khong dau): %s", BOLD, F_CYAN, RESET);
            scanf("%49s", name2);
            
            system("cls");
            
            int b1[4][4] = {0}, b2[4][4] = {0};
            int score1 = 0, score2 = 0;
            int st1 = 1, st2 = 1;      
            int surrender = 0;         
            
            addRnd_1v1(b1); addRnd_1v1(b1);
            addRnd_1v1(b2); addRnd_1v1(b2);
            
            while(1) { 
                if (st1 && isOver_1v1(b1)) st1 = 0;
                if (st2 && isOver_1v1(b2)) st2 = 0;
                
                printDualBoards(b1, score1, st1, name1, b2, score2, st2, name2);
                
                if (!st1 && !st2) {
                    printf("\n%s%s  [ HET GAME ] Ca 2 nguoi choi deu da het nuoc di!\n%s", BOLD, F_RED, RESET);
                    if (score1 > score2) printf("%s%s  => CHUC MUNG %s THANG CUOC voi %d diem!\n%s", BOLD, F_GREEN, name1, score1, RESET);
                    else if (score2 > score1) printf("%s%s  => CHUC MUNG %s THANG CUOC voi %d diem!\n%s", BOLD, F_GREEN, name2, score2, RESET);
                    else printf("%s%s  => TRAN DAU HOA NHAU (Cung %d diem)!\n%s", BOLD, F_NEON_YELLOW, score1, RESET);
                    
                    printf("\n%s%s  => Nhan phim 'q' de thoat khoi che do 1v1... %s", BOLD, F_CYAN, RESET);
                    while(1) { 
                        char c = _getch(); 
                        if (c == 'q' || c == 'Q') break; 
                        else printf("\a");
                    }
                    break;
                }
                
                int ch = _getch();
                if (ch == 224) { 
                    ch = _getch();
                    if (st2) {
                        int moved = 0;
                        if (ch == 75) moved = mv_1v1(b2, &score2, 0); 
                        else if (ch == 80) moved = mv_1v1(b2, &score2, 1); 
                        else if (ch == 77) moved = mv_1v1(b2, &score2, 2); 
                        else if (ch == 72) moved = mv_1v1(b2, &score2, 3); 
                        if (moved) addRnd_1v1(b2);
                    }
                } else {
                    if (ch == 'r' || ch == 'R') { surrender = 1; break; } 
                    if (ch == 'o' || ch == 'O') { surrender = 2; break; } 
                    
                    if (st1) { 
                        int moved = 0;
                        if (ch == 'a' || ch == 'A') moved = mv_1v1(b1, &score1, 0); 
                        else if (ch == 's' || ch == 'S') moved = mv_1v1(b1, &score1, 1); 
                        else if (ch == 'd' || ch == 'D') moved = mv_1v1(b1, &score1, 2); 
                        else if (ch == 'w' || ch == 'W') moved = mv_1v1(b1, &score1, 3); 
                        if (moved) addRnd_1v1(b1);
                    }
                }
            } 
            
            if (surrender == 1) {
                printDualBoards(b1, score1, 0, name1, b2, score2, st2, name2);
                printf("\n%s%s  [ DAU HANG ] %s da chap nhan xin thua!\n%s", BOLD, F_RED, name1, RESET);
                printf("%s%s  => CHUC MUNG %s THANG CUOC!\n%s", BOLD, F_GREEN, name2, RESET);
                printf("\n%s%s  => Nhan phim 'q' de thoat khoi che do 1v1... %s", BOLD, F_CYAN, RESET);
                while(1) { 
                    char c = _getch(); 
                    if (c == 'q' || c == 'Q') break; 
                    else printf("\a");
                }
            } else if (surrender == 2) {
                printDualBoards(b1, score1, st1, name1, b2, score2, 0, name2);
                printf("\n%s%s  [ DAU HANG ] %s da chap nhan xin thua!\n%s", BOLD, F_RED, name2, RESET);
                printf("%s%s  => CHUC MUNG %s THANG CUOC!\n%s", BOLD, F_GREEN, name1, RESET);
                printf("\n%s%s  => Nhan phim 'q' de thoat khoi che do 1v1... %s", BOLD, F_CYAN, RESET);
                while(1) { 
                    char c = _getch(); 
                    if (c == 'q' || c == 'Q') break; 
                    else printf("\a");
                }
            }
            
            system("cls");
        }
        else if (choice == 0) {
            system("cls"); printf("\a%s%sBan co that su muon thoat chuong trinh? (y/n): %s", BOLD, F_YELLOW, RESET); char confirm; scanf(" %c", &confirm);
            if(confirm != 'y' && confirm != 'Y') { choice = -1; system("cls"); }
        }
    } while (choice != 0);

    saveScoresToFile(leaderboard); freeList(leaderboard); 
    system("cls"); printf("\n%s", GAME_TITLE); printf("%sHe thong da luu va gia phong RAM an toan.%s\n", F_NEON_GREEN, RESET); printf("Tam biet! Nhan phim bat ky de thoat...\n"); _getch();
    return 0;
}