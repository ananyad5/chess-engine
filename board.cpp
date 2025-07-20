#include <vector>
#include <iostream>
#include "board.h"



using namespace std;

char board[8][8] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};

void printBoard(){
    for (int i = 0; i <8; i++){
        cout << 8-i << " ";
        for (int j = 0; j < 8; j++){
            cout << board[i][j]<<" ";

        }
        cout << endl;


    }
    
    cout << " a b c d e f g h";
}

int main(){
    printBoard();
    return 0;
}




