#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <vector>
#include <string>

enum PieceType {
    NONE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum Color {
    WHITE,
    BLACK
};

struct Piece {
    PieceType type;
    Color color;
    
    Piece() : type(NONE), color(WHITE) {}
    Piece(PieceType t, Color c) : type(t), color(c) {}
    
    bool isEmpty() const { return type == NONE; }
};

struct Move {
    int startSquare;
    int targetSquare;
    PieceType promotionPiece;
    
    Move() : startSquare(0), targetSquare(0), promotionPiece(NONE) {}
    Move(int start, int target, PieceType promotion = NONE) 
        : startSquare(start), targetSquare(target), promotionPiece(promotion) {}
};

class Board {
public:
    Board();
    
    // Initialize the board with starting position
    void initializeStandardPosition();
    
    // Get piece at a specific square
    Piece getPieceAt(int square) const;
    
    // Make a move
    void makeMove(const Move& move);
    
    // Undo a move
    void undoMove();
    
    // Get all legal moves for current position
    std::vector<Move> getLegalMoves() const;
    
    // Check if the current position is in check
    bool isInCheck(Color color) const;
    
    // Get current side to move
    Color getSideToMove() const { return sideToMove; }
    
    // Conversion utilities
    static int squareFromCoords(int file, int rank) { return rank * 8 + file; }
    static std::pair<int, int> coordsFromSquare(int square) { return {square % 8, square / 8}; }
    
    // FEN notation handling
    void loadFromFen(const std::string& fen);
    std::string toFen() const;
    
    // Castling rights
    bool whiteCanCastleKingside;
    bool whiteCanCastleQueenside;
    bool blackCanCastleKingside;
    bool blackCanCastleQueenside;
    
    // En passant square
    int enPassantSquare;

private:
    std::array<Piece, 64> squares;
    Color sideToMove;
    
    // Move history for undoing moves
    std::vector<Move> moveHistory;
};

#endif // BOARD_H