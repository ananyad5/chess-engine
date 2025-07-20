#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <bitset>
#include <cstdlib>
#include <ctime>

using namespace std;

typedef uint64_t U64;

// ==== Piece indices ====
enum Piece {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

U64 bitboards[12];
U64 whitePieces, blackPieces, allPieces;
bool whiteToMove = true;

// ==== Bit Manipulation ====
inline void setBit(U64 &bb, int sq) { bb |= (1ULL << sq); }
inline void clearBit(U64 &bb, int sq) { bb &= ~(1ULL << sq); }
inline bool getBit(U64 bb, int sq) { return (bb >> sq) & 1ULL; }

// ==== Square name map ====
const string squareNames[64] = {
    "a1","b1","c1","d1","e1","f1","g1","h1",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a8","b8","c8","d8","e8","f8","g8","h8"
};

// ==== Knight lookup table ====
U64 knightAttacks[64];

U64 maskKnightAttacks(int sq) {
    U64 attacks = 0ULL;
    int r = sq / 8, f = sq % 8;
    int dr[] = {-2,-1,1,2,2,1,-1,-2};
    int df[] = {1,2,2,1,-1,-2,-2,-1};
    for (int i = 0; i < 8; ++i) {
        int nr = r + dr[i], nf = f + df[i];
        if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8)
            attacks |= (1ULL << (nr * 8 + nf));
    }
    return attacks;
}

void initKnightAttacks() {
    for (int sq = 0; sq < 64; ++sq)
        knightAttacks[sq] = maskKnightAttacks(sq);
}

// ==== Initialize Board ====
void initPosition() {
    bitboards[WP] = 0x000000000000FF00ULL;
    bitboards[WN] = 0x0000000000000042ULL;
    bitboards[WR] = 0x0000000000000081ULL;
    bitboards[WQ] = 0x0000000000000008ULL;
    bitboards[WK] = 0x0000000000000010ULL;
    bitboards[WB] = 0x0000000000000024ULL;

    bitboards[BP] = 0x00FF000000000000ULL;
    bitboards[BN] = 0x4200000000000000ULL;
    bitboards[BR] = 0x8100000000000000ULL;
    bitboards[BQ] = 0x0800000000000000ULL;
    bitboards[BK] = 0x1000000000000000ULL;
    bitboards[BB] = 0x2400000000000000ULL;

    whitePieces = bitboards[WP] | bitboards[WN] | bitboards[WB] |
                  bitboards[WR] | bitboards[WQ] | bitboards[WK];
    blackPieces = bitboards[BP] | bitboards[BN] | bitboards[BB] |
                  bitboards[BR] | bitboards[BQ] | bitboards[BK];
    allPieces = whitePieces | blackPieces;
}

// ==== Print Board ====
void printBoard() {
    char board[64];
    fill(begin(board), end(board), '.');

    for (int i = 0; i < 12; ++i) {
        U64 bb = bitboards[i];
        char symbol = "PNBRQKpnbrqk"[i];
        for (int sq = 0; sq < 64; ++sq)
            if (getBit(bb, sq))
                board[sq] = symbol;
    }

    for (int rank = 7; rank >= 0; --rank) {
        cout << rank + 1 << " ";
        for (int file = 0; file < 8; ++file)
            cout << board[rank * 8 + file] << " ";
        cout << "\n";
    }
    cout << "  a b c d e f g h\n";
    cout << (whiteToMove ? "White" : "Black") << " to move\n\n";
}

// ==== Generate Legal Moves for Pawns & Knights ====
vector<string> generateMoves() {
    vector<string> moves;
    if (whiteToMove) {
        U64 pawns = bitboards[WP];
        U64 knights = bitboards[WN];

        while (pawns) {
            int sq = __builtin_ctzll(pawns);
            clearBit(pawns, sq);
            int to = sq + 8;
            if (to < 64 && !getBit(allPieces, to))
                moves.push_back(squareNames[sq] + squareNames[to]);
        }

        while (knights) {
            int from = __builtin_ctzll(knights);
            clearBit(knights, from);
            U64 attacks = knightAttacks[from] & ~whitePieces;
            while (attacks) {
                int to = __builtin_ctzll(attacks);
                clearBit(attacks, to);
                moves.push_back(squareNames[from] + squareNames[to]);
            }
        }
    }
    // TODO: Add black move generation later
    return moves;
}

// ==== Execute Move ====
bool makeMove(const string& move) {
    if (move.length() != 4) return false;

    int from = (move[1] - '1') * 8 + (move[0] - 'a');
    int to = (move[3] - '1') * 8 + (move[2] - 'a');

    for (int i = 0; i < 12; ++i) {
        if (getBit(bitboards[i], from)) {
            clearBit(bitboards[i], from);
            setBit(bitboards[i], to);
            break;
        }
    }

    for (int i = 0; i < 12; ++i)
        if (getBit(bitboards[i], to) && i / 6 != whiteToMove)
            clearBit(bitboards[i], to);

    whiteToMove = !whiteToMove;

    whitePieces = bitboards[WP] | bitboards[WN] | bitboards[WB] | bitboards[WR] | bitboards[WQ] | bitboards[WK];
    blackPieces = bitboards[BP] | bitboards[BN] | bitboards[BB] | bitboards[BR] | bitboards[BQ] | bitboards[BK];
    allPieces = whitePieces | blackPieces;
    return true;
}

// ==== Random Move for Engine ====
void engineMove() {
    vector<string> moves = generateMoves();
    if (moves.empty()) {
        cout << "No legal moves.\n";
        return;
    }
    string mv = moves[rand() % moves.size()];
    cout << "Engine plays: " << mv << "\n";
    makeMove(mv);
}

// ==== Main ====
int main() {
    srand(time(0));
    initKnightAttacks();
    initPosition();
    printBoard();

    while (true) {
        if (whiteToMove) {
            string move;
            cout << "Your move (e.g. e2e4): ";
            cin >> move;
            if (!makeMove(move)) {
                cout << "Invalid move.\n";
                continue;
            }
        } else {
            engineMove();
        }
        printBoard();
    }

    return 0;
}
