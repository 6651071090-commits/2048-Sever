#include "game2048.h"

int main() {
    srand(time(0));
    Player* leaderboard = loadScoresFromFile();
    int choice;

    do {
        system("cls");
        printf("------- MENU TRO CHOI 2048 -------\n");
        printf("1. Choi Game 2048\n");
        printf("2. Xem Bang xep hang (Duyet De quy)\n");
        printf("3. Tim kiem nguoi choi (Tim kiem De quy)\n");
        printf("4. Sua diem (Admin)\n");
        printf("5. Xoa nguoi choi (Admin)\n");
        printf("0. Luu va Thoat\n");
        printf("----------------------------------\n");
        printf("Nhap lua chon: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            continue;
        }

        if (choice == 1) {
            // Chuc nang Choi Game
            initBoard();
            addRandom(); addRandom();
            while (1) {
                printBoard();
                if (checkGameOver()) {
                    printf("\nGAME OVER! Diem cua ban: %d\n", currentScore);
                    char name[50];
                    printf("Nhap ten cua ban: ");
                    scanf("%s", name);
                    leaderboard = addPlayer(leaderboard, name, currentScore);
                    break;
                }
                char move = _getch();
                int moved = 0;
                if (move == 'w' || move == 'W') moved = moveUp();
                else if (move == 's' || move == 'S') moved = moveDown();
                else if (move == 'a' || move == 'A') moved = moveLeft();
                else if (move == 'd' || move == 'D') moved = moveRight();
                else if (move == 'q' || move == 'Q') break;
                
                if (moved) addRandom();
            }
            freeBoard(); // Giai phong con tro cap 2 cua ban co
            printf("\nNhan phim bat ky de quay lai Menu...");
            _getch();
        } 
        else if (choice == 2) {
            // Chuc nang Xem Bang xep hang
            system("cls");
            printf("--- BANG XEP HANG (Xap xep & Duyet De quy) ---\n");
            
            if (leaderboard == NULL) {
                printf("Chua co du lieu nguoi choi.\n");
            } else {
                sortLeaderboard(leaderboard); // Sap xep truoc khi in
                
                Player* temp = leaderboard;
                int rank = 1;
                while (temp != NULL) {
                    printf("%d. Nguoi choi: %-15s Diem: %d\n", rank++, temp->name, temp->score);
                    temp = temp->next;
                }
            }
            system("pause");
        }
        else if (choice >= 3 && choice <= 5) {
            char searchName[50];
            printf("Nhap ten nguoi choi: ");
            scanf("%s", searchName);

            if (choice == 3) {
                // GOI DE QUY: Tim kiem nguoi choi
                printf("\nKet qua tim kiem:\n");
                searchPlayer(leaderboard, searchName); 
            }
            else if (choice == 4) {
                int newScore;
                printf("Nhap diem moi: ");
                scanf("%d", &newScore);
                editPlayer(leaderboard, searchName, newScore);
            }
            else if (choice == 5) {
                leaderboard = deletePlayer(leaderboard, searchName);
            }
            system("pause");
        }
    } while (choice != 0);

    // TRUOC KHI THOAT: Luu va giai phong bo nho bang De quy
    saveScoresToFile(leaderboard);
    
    // GOI DE QUY: Giai phong toan bo Danh sach lien ket
    freeList(leaderboard); 

    printf("\nDa luu du lieu vao file scores.txt.\n");
    printf("Tam biet!\n");
    return 0;
}