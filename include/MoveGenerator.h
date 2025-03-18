#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "Board.h"
#include <vector>

class MoveGenerator {
public:
    MoveGenerator(const Board& board);
    
    // Generate all legal moves for the current position
    std::vector<Move> generateLegalMoves();
    
private:
    const Board& board;
    
    // Helper methods for generating specific piece moves
    std::vector<Move> generatePawnMoves(int square);
    std::vector<Move> generateKnightMoves(int square);
    std::vector<Move> generateBishopMoves(int square);
    std::vector<Move> generateRookMoves(int square);
    std::vector<Move> generateQueenMoves(int square);
    std::vector<Move> generateKingMoves(int square);
    
    // Generate sliding piece moves (bishop, rook, queen)
    std::vector<Move> generateSlidingMoves(int square, const std::vector<std::pair<int, int>>& directions);
    
    // Check if a move is legal (doesn't leave king in check)
    bool isLegalMove(const Move& move);
    
    // Direction vectors for sliding pieces
    const std::vector<std::pair<int, int>> bishopDirections = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    const std::vector<std::pair<int, int>> rookDirections = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    
    // Helper for checking if a square is on the board
    bool isSquareOnBoard(int file, int rank) const;
};

// Implementation of MoveGenerator methods

MoveGenerator::MoveGenerator(const Board& b) : board(b) {}

std::vector<Move> MoveGenerator::generateLegalMoves() {
    std::vector<Move> moves;
    Color sideToMove = board.getSideToMove();
    
    for (int square = 0; square < 64; square++) {
        Piece piece = board.getPieceAt(square);
        if (piece.isEmpty() || piece.color != sideToMove) {
            continue;
        }
        
        std::vector<Move> pieceMoves;
        switch (piece.type) {
            case PAWN:
                pieceMoves = generatePawnMoves(square);
                break;
            case KNIGHT:
                pieceMoves = generateKnightMoves(square);
                break;
            case BISHOP:
                pieceMoves = generateBishopMoves(square);
                break;
            case ROOK:
                pieceMoves = generateRookMoves(square);
                break;
            case QUEEN:
                pieceMoves = generateQueenMoves(square);
                break;
            case KING:
                pieceMoves = generateKingMoves(square);
                break;
            default:
                break;
        }
        
        for (const auto& move : pieceMoves) {
            if (isLegalMove(move)) {
                moves.push_back(move);
            }
        }
    }
    
    return moves;
}

bool MoveGenerator::isSquareOnBoard(int file, int rank) const {
    return file >= 0 && file < 8 && rank >= 0 && rank < 8;
}

std::vector<Move> MoveGenerator::generateSlidingMoves(int square, const std::vector<std::pair<int, int>>& directions) {
    std::vector<Move> moves;
    auto [file, rank] = Board::coordsFromSquare(square);
    Color pieceColor = board.getPieceAt(square).color;
    
    for (const auto& [dx, dy] : directions) {
        int newFile = file + dx;
        int newRank = rank + dy;
        
        while (isSquareOnBoard(newFile, newRank)) {
            int targetSquare = Board::squareFromCoords(newFile, newRank);
            Piece targetPiece = board.getPieceAt(targetSquare);
            
            if (targetPiece.isEmpty()) {
                // Empty square, add move and continue
                moves.push_back(Move(square, targetSquare));
            } else {
                // Found a piece
                if (targetPiece.color != pieceColor) {
                    // Enemy piece, add capture and stop
                    moves.push_back(Move(square, targetSquare));
                }
                // Stop sliding in this direction
                break;
            }
            
            newFile += dx;
            newRank += dy;
        }
    }
    
    return moves;
}

// Implement specific piece move generators
std::vector<Move> MoveGenerator::generatePawnMoves(int square) {
    std::vector<Move> moves;
    auto [file, rank] = Board::coordsFromSquare(square);
    Color pieceColor = board.getPieceAt(square).color;
    
    // Direction of pawn movement
    int direction = (pieceColor == WHITE) ? 1 : -1;
    
    // Starting rank for pawns
    int startingRank = (pieceColor == WHITE) ? 1 : 6;
    
    // Single push
    int newRank = rank + direction;
    if (isSquareOnBoard(file, newRank)) {
        int targetSquare = Board::squareFromCoords(file, newRank);
        if (board.getPieceAt(targetSquare).isEmpty()) {
            // Check for promotion
            if (newRank == 7 || newRank == 0) {
                // Add promotions
                moves.push_back(Move(square, targetSquare, QUEEN));
                moves.push_back(Move(square, targetSquare, ROOK));
                moves.push_back(Move(square, targetSquare, BISHOP));
                moves.push_back(Move(square, targetSquare, KNIGHT));
            } else {
                moves.push_back(Move(square, targetSquare));
            }
            
            // Double push from starting rank
            if (rank == startingRank) {
                int doubleRank = rank + 2 * direction;
                int doubleSquare = Board::squareFromCoords(file, doubleRank);
                if (board.getPieceAt(doubleSquare).isEmpty()) {
                    moves.push_back(Move(square, doubleSquare));
                }
            }
        }
    }
    
    // Captures
    for (int df = -1; df <= 1; df += 2) {
        int newFile = file + df;
        int newRank = rank + direction;
        
        if (isSquareOnBoard(newFile, newRank)) {
            int targetSquare = Board::squareFromCoords(newFile, newRank);
            Piece targetPiece = board.getPieceAt(targetSquare);
            
            // Regular capture
            if (!targetPiece.isEmpty() && targetPiece.color != pieceColor) {
                // Check for promotion
                if (newRank == 7 || newRank == 0) {
                    // Add promotions
                    moves.push_back(Move(square, targetSquare, QUEEN));
                    moves.push_back(Move(square, targetSquare, ROOK));
                    moves.push_back(Move(square, targetSquare, BISHOP));
                    moves.push_back(Move(square, targetSquare, KNIGHT));
                } else {
                    moves.push_back(Move(square, targetSquare));
                }
            }
            
            // En passant capture
            if (targetSquare == board.enPassantSquare) {
                moves.push_back(Move(square, targetSquare));
            }
        }
    }
    
    return moves;
}

std::vector<Move> MoveGenerator::generateKnightMoves(int square) {
    std::vector<Move> moves;
    auto [file, rank] = Board::coordsFromSquare(square);
    Color pieceColor = board.getPieceAt(square).color;
    
    // Knight move offsets
    const std::vector<std::pair<int, int>> knightOffsets = {
        {1, 2}, {2, 1}, {2, -1}, {1, -2},
        {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
    };
    
    for (const auto& [dx, dy] : knightOffsets) {
        int newFile = file + dx;
        int newRank = rank + dy;
        
        if (isSquareOnBoard(newFile, newRank)) {
            int targetSquare = Board::squareFromCoords(newFile, newRank);
            Piece targetPiece = board.getPieceAt(targetSquare);
            
            // Move to empty square or capture
            if (targetPiece.isEmpty() || targetPiece.color != pieceColor) {
                moves.push_back(Move(square, targetSquare));
            }
        }
    }
    
    return moves;
}

std::vector<Move> MoveGenerator::generateBishopMoves(int square) {
    return generateSlidingMoves(square, bishopDirections);
}

std::vector<Move> MoveGenerator::generateRookMoves(int square) {
    return generateSlidingMoves(square, rookDirections);
}

std::vector<Move> MoveGenerator::generateQueenMoves(int square) {
    std::vector<Move> moves;
    
    // Queen combines bishop and rook movements
    auto bishopMoves = generateSlidingMoves(square, bishopDirections);
    auto rookMoves = generateSlidingMoves(square, rookDirections);
    
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    
    return moves;
}

std::vector<Move> MoveGenerator::generateKingMoves(int square) {
    std::vector<Move> moves;
    auto [file, rank] = Board::coordsFromSquare(square);
    Color pieceColor = board.getPieceAt(square).color;
    
    // King move offsets (8 directions)
    const std::vector<std::pair<int, int>> kingOffsets = {
        {0, 1}, {1, 1}, {1, 0}, {1, -1},
        {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
    };
    
    // Regular king moves
    for (const auto& [dx, dy] : kingOffsets) {
        int newFile = file + dx;
        int newRank = rank + dy;
        
        if (isSquareOnBoard(newFile, newRank)) {
            int targetSquare = Board::squareFromCoords(newFile, newRank);
            Piece targetPiece = board.getPieceAt(targetSquare);
            
            // Move to empty square or capture
            if (targetPiece.isEmpty() || targetPiece.color != pieceColor) {
                moves.push_back(Move(square, targetSquare));
            }
        }
    }
    
    // Castling
    if (pieceColor == WHITE) {
        // Kingside castling
        if (board.whiteCanCastleKingside) {
            if (board.getPieceAt(Board::squareFromCoords(5, 0)).isEmpty() &&
                board.getPieceAt(Board::squareFromCoords(6, 0)).isEmpty()) {
                // Check if king is not in check and doesn't pass through check
                // This would require a more sophisticated implementation
                moves.push_back(Move(square, Board::squareFromCoords(6, 0)));
            }
        }
        
        // Queenside castling
        if (board.whiteCanCastleQueenside) {
            if (board.getPieceAt(Board::squareFromCoords(1, 0)).isEmpty() &&
                board.getPieceAt(Board::squareFromCoords(2, 0)).isEmpty() &&
                board.getPieceAt(Board::squareFromCoords(3, 0)).isEmpty()) {
                // Check if king is not in check and doesn't pass through check
                moves.push_back(Move(square, Board::squareFromCoords(2, 0)));
            }
        }
    } else {
        // Kingside castling
        if (board.blackCanCastleKingside) {
            if (board.getPieceAt(Board::squareFromCoords(5, 7)).isEmpty() &&
                board.getPieceAt(Board::squareFromCoords(6, 7)).isEmpty()) {
                moves.push_back(Move(square, Board::squareFromCoords(6, 7)));
            }
        }
        
        // Queenside castling
        if (board.blackCanCastleQueenside) {
            if (board.getPieceAt(Board::squareFromCoords(1, 7)).isEmpty() &&
                board.getPieceAt(Board::squareFromCoords(2, 7)).isEmpty() &&
                board.getPieceAt(Board::squareFromCoords(3, 7)).isEmpty()) {
                moves.push_back(Move(square, Board::squareFromCoords(2, 7)));
            }
        }
    }
    
    return moves;
}

bool MoveGenerator::isLegalMove(const Move& move) {
    // Create a temporary board to test the move
    Board tempBoard = board; // This would require a copy constructor
    
    // Make the move on the temporary board
    tempBoard.makeMove(move);
    
    // Check if the king is in check after the move
    Color movedColor = board.getSideToMove();
    return !tempBoard.isInCheck(movedColor);
}

#endif
