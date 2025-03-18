#ifndef CHESS_RENDERER_H
#define CHESS_RENDERER_H

#include <SFML/Graphics.hpp>
#include "Board.h"
#include <map>
#include <string>

class ChessRenderer {
public:
    ChessRenderer(sf::RenderWindow& window, const Board& board);
    
    // Initialize textures and sprites
    bool initialize();
    
    // Draw the current board state
    void drawBoard();
    
    // Handle user input/interaction
    Move handleMouseClick(sf::Vector2i mousePos);
    
private:
    sf::RenderWindow& window;
    const Board& board;
    
    // Board dimensions
    const int boardSize = 64;
    const int squareSize = 512;
    
    // Textures and sprites
    sf::Texture boardTexture;
    sf::Sprite boardSprite;
    
    std::map<std::string, sf::Texture> pieceTextures;
    std::map<int, sf::Sprite> pieceSprites;
    
    // Currently selected square
    int selectedSquare = -1;
    
    // Convert screen coordinates to board square
    int getSquareFromPosition(sf::Vector2i pos);
    
    // Convert board square to screen coordinates
    sf::Vector2f getPositionFromSquare(int square);
    
    // Update piece sprites based on board state
    void updatePieceSprites();
    
    // Load piece textures
    bool loadPieceTextures();
};

// Implementation of ChessRenderer methods

ChessRenderer::ChessRenderer(sf::RenderWindow& w, const Board& b) : window(w), board(b) {}

bool ChessRenderer::initialize() {
    // Use class member variables for board dimensions (640x640 board, 80x80 squares)
    sf::RenderTexture renderTexture;
    if (!renderTexture.create(boardSize, boardSize)) {
        return false;
    }
    
    // Define colors for light and dark squares
    sf::Color lightSquareColor(240, 217, 181);
    sf::Color darkSquareColor(181, 136, 99);
    
    // Create a rectangle shape for each square
    sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
    
    // Draw an 8x8 chess board by alternating the colors
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            square.setPosition(col * squareSize, row * squareSize);
            square.setFillColor(((row + col) % 2 == 0) ? lightSquareColor : darkSquareColor);
            renderTexture.draw(square);
        }
    }
    
    renderTexture.display();
    boardTexture = renderTexture.getTexture();
    boardSprite.setTexture(boardTexture);
    
    // Load piece textures and update sprites
    if (!loadPieceTextures()) {
        return false;
    }
    updatePieceSprites();
    
    return true;
}


bool ChessRenderer::loadPieceTextures() {
    // Piece types
    std::vector<std::string> pieceTypes = {"pawn", "knight", "bishop", "rook", "queen", "king"};
    
    // Load textures for both colors
    for (const auto& type : pieceTypes) {
        if (!pieceTextures["white-" + type].loadFromFile("assets/white-" + type + ".png")) {
            return false;
        }
        if (!pieceTextures["black-" + type].loadFromFile("assets/black-" + type + ".png")) {
            return false;
        }
    }
    
    return true;
}

void ChessRenderer::updatePieceSprites() {
    pieceSprites.clear();
    
    for (int square = 0; square < 64; square++) {
        Piece piece = board.getPieceAt(square);
        if (piece.isEmpty()) {
            continue;
        }
        
        std::string pieceTextureName;
        if (piece.color == WHITE) {
            pieceTextureName = "white_";
        } else {
            pieceTextureName = "black_";
        }
        
        switch (piece.type) {
            case PAWN: pieceTextureName += "pawn"; break;
            case KNIGHT: pieceTextureName += "knight"; break;
            case BISHOP: pieceTextureName += "bishop"; break;
            case ROOK: pieceTextureName += "rook"; break;
            case QUEEN: pieceTextureName += "queen"; break;
            case KING: pieceTextureName += "king"; break;
            default: continue;
        }
        
        sf::Sprite sprite;
        sprite.setTexture(pieceTextures[pieceTextureName]);
        
        // Set sprite position
        auto pos = getPositionFromSquare(square);
        sprite.setPosition(pos);
        
        // Store sprite
        pieceSprites[square] = sprite;
    }
}

void ChessRenderer::drawBoard() {
    // Draw the board
    window.draw(boardSprite);
    
    // Draw pieces
    for (const auto& [square, sprite] : pieceSprites) {
        window.draw(sprite);
    }
    
    // Highlight selected square if any
    if (selectedSquare != -1) {
        sf::RectangleShape highlight;
        highlight.setSize(sf::Vector2f(squareSize, squareSize));
        highlight.setPosition(getPositionFromSquare(selectedSquare));
        highlight.setFillColor(sf::Color(255, 255, 0, 128));
        window.draw(highlight);
    }
}

int ChessRenderer::getSquareFromPosition(sf::Vector2i pos) {
    int file = pos.x / squareSize;
    int rank = 7 - (pos.y / squareSize);
    
    if (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
        return Board::squareFromCoords(file, rank);
    }
    
    return -1; // Invalid square
}

sf::Vector2f ChessRenderer::getPositionFromSquare(int square) {
    auto [file, rank] = Board::coordsFromSquare(square);
    return sf::Vector2f(file * squareSize, (7 - rank) * squareSize);
}

Move ChessRenderer::handleMouseClick(sf::Vector2i mousePos) {
    int clickedSquare = getSquareFromPosition(mousePos);
    
    if (clickedSquare == -1) {
        return Move(); // Invalid move
    }
    
    if (selectedSquare == -1) {
        // No square selected yet
        Piece clickedPiece = board.getPieceAt(clickedSquare);
        if (!clickedPiece.isEmpty() && clickedPiece.color == board.getSideToMove()) {
            selectedSquare = clickedSquare;
        }
        return Move(); // No move yet
    } else {
        // We already have a selected square
        Move move(selectedSquare, clickedSquare);
        selectedSquare = -1;
        return move;
    }
}

#endif