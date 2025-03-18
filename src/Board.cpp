#include "Board.h"
#include <sstream>
#include <iostream>

Board::Board()
{
    initializeStandardPosition();
}

void Board::initializeStandardPosition()
{
    // Clear the board
    for (auto &square : squares)
    {
        square = Piece();
    }

    // Set up pawns
    for (int file = 0; file < 8; file++)
    {
        squares[squareFromCoords(file, 1)] = Piece(PAWN, WHITE);
        squares[squareFromCoords(file, 6)] = Piece(PAWN, BLACK);
    }

    // Set up white pieces
    squares[squareFromCoords(0, 0)] = Piece(ROOK, WHITE);
    squares[squareFromCoords(1, 0)] = Piece(KNIGHT, WHITE);
    squares[squareFromCoords(2, 0)] = Piece(BISHOP, WHITE);
    squares[squareFromCoords(3, 0)] = Piece(QUEEN, WHITE);
    squares[squareFromCoords(4, 0)] = Piece(KING, WHITE);
    squares[squareFromCoords(5, 0)] = Piece(BISHOP, WHITE);
    squares[squareFromCoords(6, 0)] = Piece(KNIGHT, WHITE);
    squares[squareFromCoords(7, 0)] = Piece(ROOK, WHITE);

    // Set up black pieces
    squares[squareFromCoords(0, 7)] = Piece(ROOK, BLACK);
    squares[squareFromCoords(1, 7)] = Piece(KNIGHT, BLACK);
    squares[squareFromCoords(2, 7)] = Piece(BISHOP, BLACK);
    squares[squareFromCoords(3, 7)] = Piece(QUEEN, BLACK);
    squares[squareFromCoords(4, 7)] = Piece(KING, BLACK);
    squares[squareFromCoords(5, 7)] = Piece(BISHOP, BLACK);
    squares[squareFromCoords(6, 7)] = Piece(KNIGHT, BLACK);
    squares[squareFromCoords(7, 7)] = Piece(ROOK, BLACK);

    // Set starting state
    sideToMove = WHITE;
    whiteCanCastleKingside = true;
    whiteCanCastleQueenside = true;
    blackCanCastleKingside = true;
    blackCanCastleQueenside = true;
    enPassantSquare = -1;

    // Clear move history
    moveHistory.clear();
}

Piece Board::getPieceAt(int square) const
{
    if (square >= 0 && square < 64)
    {
        return squares[square];
    }
    return Piece();
}

// Structure to store move state for undoing
struct MoveState
{
    Move move;
    Piece capturedPiece;
    bool whiteCanCastleKingside;
    bool whiteCanCastleQueenside;
    bool blackCanCastleKingside;
    bool blackCanCastleQueenside;
    int enPassantSquare;
};

void Board::makeMove(const Move &move)
{
    int start = move.startSquare;
    int target = move.targetSquare;

    // Save current state for undo
    MoveState state;
    state.move = move;
    state.capturedPiece = getPieceAt(target);
    state.whiteCanCastleKingside = whiteCanCastleKingside;
    state.whiteCanCastleQueenside = whiteCanCastleQueenside;
    state.blackCanCastleKingside = blackCanCastleKingside;
    state.blackCanCastleQueenside = blackCanCastleQueenside;
    state.enPassantSquare = enPassantSquare;

    // Get the moving piece
    Piece movingPiece = getPieceAt(start);

    // Make the move
    squares[target] = movingPiece;
    squares[start] = Piece();

    // Handle promotions
    if (move.promotionPiece != NONE)
    {
        squares[target].type = move.promotionPiece;
    }

    // Handle castling
    if (movingPiece.type == KING)
    {
        // Check for castling move
        int rankOffset = (movingPiece.color == WHITE) ? 0 : 7;

        // Kingside castling
        if (start == squareFromCoords(4, rankOffset) && target == squareFromCoords(6, rankOffset))
        {
            // Move the rook too
            squares[squareFromCoords(5, rankOffset)] = squares[squareFromCoords(7, rankOffset)];
            squares[squareFromCoords(7, rankOffset)] = Piece();
        }

        // Queenside castling
        if (start == squareFromCoords(4, rankOffset) && target == squareFromCoords(2, rankOffset))
        {
            // Move the rook too
            squares[squareFromCoords(3, rankOffset)] = squares[squareFromCoords(0, rankOffset)];
            squares[squareFromCoords(0, rankOffset)] = Piece();
        }

        // Update castling rights
        if (movingPiece.color == WHITE)
        {
            whiteCanCastleKingside = false;
            whiteCanCastleQueenside = false;
        }
        else
        {
            blackCanCastleKingside = false;
            blackCanCastleQueenside = false;
        }
    }

    // Update castling rights for rook moves
    if (movingPiece.type == ROOK)
    {
        if (movingPiece.color == WHITE)
        {
            if (start == squareFromCoords(0, 0))
            {
                whiteCanCastleQueenside = false;
            }
            else if (start == squareFromCoords(7, 0))
            {
                whiteCanCastleKingside = false;
            }
        }
        else
        {
            if (start == squareFromCoords(0, 7))
            {
                blackCanCastleQueenside = false;
            }
            else if (start == squareFromCoords(7, 7))
            {
                blackCanCastleKingside = false;
            }
        }
    }

    // Handle en passant captures
    if (movingPiece.type == PAWN && target == enPassantSquare)
    {
        // Determine the captured pawn square
        int capturedPawnRank = (movingPiece.color == WHITE) ? 4 : 3;
        int capturedPawnSquare = squareFromCoords(target % 8, capturedPawnRank);
        squares[capturedPawnSquare] = Piece();
    }

    // Set en passant square for next move
    enPassantSquare = -1;
    if (movingPiece.type == PAWN)
    {
        int startRank = start / 8;
        int targetRank = target / 8;

        // Check for double pawn push
        if (abs(targetRank - startRank) == 2)
        {
            // Set en passant square behind the pawn
            int epRank = (startRank + targetRank) / 2;
            enPassantSquare = squareFromCoords(start % 8, epRank);
        }
    }

    // Switch side to move
    sideToMove = (sideToMove == WHITE) ? BLACK : WHITE;

    // Add move to history
    moveHistory.push_back(move);
}

bool Board::isInCheck(Color color) const
{
    // Find the king
    int kingSquare = -1;
    for (int i = 0; i < 64; i++)
    {
        Piece piece = getPieceAt(i);
        if (piece.type == KING && piece.color == color)
        {
            kingSquare = i;
            break;
        }
    }

    if (kingSquare == -1)
    {
        return false; // No king found (shouldn't happen in a valid position)
    }

    // Check for attacks from each direction and by knights

    // ... Implementation of attack detection
    // This would check all possible attacking moves from the opponent

    return false; // Placeholder - actual implementation would be more complex
}

// Additional methods would be implemented here

// FEN notation handling
void Board::loadFromFen(const std::string &fen)
{
    std::istringstream fenStream(fen);
    std::string position, activeColor, castling, enPassant, halfmove, fullmove;

    fenStream >> position >> activeColor >> castling >> enPassant >> halfmove >> fullmove;

    // Clear the board
    for (auto &square : squares)
    {
        square = Piece();
    }

    // Parse the position
    int rank = 7;
    int file = 0;

    for (char c : position)
    {
        if (c == '/')
        {
            rank--;
            file = 0;
        }
        else if (isdigit(c))
        {
            file += c - '0';
        }
        else
        {
            Piece piece;

            // Determine piece type
            switch (toupper(c))
            {
            case 'P':
                piece.type = PAWN;
                break;
            case 'N':
                piece.type = KNIGHT;
                break;
            case 'B':
                piece.type = BISHOP;
                break;
            case 'R':
                piece.type = ROOK;
                break;
            case 'Q':
                piece.type = QUEEN;
                break;
            case 'K':
                piece.type = KING;
                break;
            default:
                piece.type = NONE;
                break;
            }

            // Determine piece color
            piece.color = isupper(c) ? WHITE : BLACK;

            // Set the piece on the board
            squares[squareFromCoords(file, rank)] = piece;
            file++;
        }
    }

    // Parse active color
    sideToMove = (activeColor == "w") ? WHITE : BLACK;

    // Parse castling rights
    whiteCanCastleKingside = castling.find('K') != std::string::npos;
    whiteCanCastleQueenside = castling.find('Q') != std::string::npos;
    blackCanCastleKingside = castling.find('k') != std::string::npos;
    blackCanCastleQueenside = castling.find('q') != std::string::npos;

    // Parse en passant square
    if (enPassant == "-")
    {
        enPassantSquare = -1;
    }
    else
    {
        file = enPassant[0] - 'a';
        rank = enPassant[1] - '1';
        enPassantSquare = squareFromCoords(file, rank);
    }

    // Clear move history
    moveHistory.clear();
}

std::string Board::toFen() const
{
    std::stringstream fen;

    // Board position
    for (int rank = 7; rank >= 0; rank--)
    {
        int emptyCount = 0;

        for (int file = 0; file < 8; file++)
        {
            Piece piece = getPieceAt(squareFromCoords(file, rank));

            if (piece.isEmpty())
            {
                emptyCount++;
            }
            else
            {
                if (emptyCount > 0)
                {
                    fen << emptyCount;
                    fen << emptyCount;
                    emptyCount = 0;
                }

                char pieceChar;
                switch (piece.type)
                {
                case PAWN:
                    pieceChar = 'p';
                    break;
                case KNIGHT:
                    pieceChar = 'n';
                    break;
                case BISHOP:
                    pieceChar = 'b';
                    break;
                case ROOK:
                    pieceChar = 'r';
                    break;
                case QUEEN:
                    pieceChar = 'q';
                    break;
                case KING:
                    pieceChar = 'k';
                    break;
                default:
                    pieceChar = '?';
                    break;
                }

                if (piece.color == WHITE)
                {
                    pieceChar = toupper(pieceChar);
                }

                fen << pieceChar;
            }
        }

        if (emptyCount > 0)
        {
            fen << emptyCount;
        }

        if (rank > 0)
        {
            fen << '/';
        }
    }

    // Active color
    fen << ' ' << (sideToMove == WHITE ? 'w' : 'b');

    // Castling availability
    fen << ' ';
    if (whiteCanCastleKingside || whiteCanCastleQueenside ||
        blackCanCastleKingside || blackCanCastleQueenside)
    {
        if (whiteCanCastleKingside)
            fen << 'K';
        if (whiteCanCastleQueenside)
            fen << 'Q';
        if (blackCanCastleKingside)
            fen << 'k';
        if (blackCanCastleQueenside)
            fen << 'q';
    }
    else
    {
        fen << '-';
    }

    // En passant target square
    fen << ' ';
    if (enPassantSquare == -1)
    {
        fen << '-';
    }
    else
    {
        auto [file, rank] = coordsFromSquare(enPassantSquare);
        fen << static_cast<char>('a' + file) << static_cast<char>('1' + rank);
    }

    // Halfmove clock and fullmove number - simplified
    fen << " 0 1";

    return fen.str();
}