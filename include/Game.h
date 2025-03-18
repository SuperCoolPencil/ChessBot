#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Board.h"
#include "MoveGenerator.h"
#include "ChessRenderer.h"
#include <memory>
#include <iostream>

class Game {
public:
    Game();
    
    // Initialize the game
    bool initialize();
    
    // Main game loop
    void run();
    
private:
    // Window and rendering
    sf::RenderWindow window;
    std::unique_ptr<Board> board;
    std::unique_ptr<ChessRenderer> renderer;
    
    // Game logic
    void handleEvents();
    void update();
    void render();
    
    // Handle player move
    bool tryMakeMove(const Move& move);
    
    // Game state
    bool isGameOver = false;
    bool playerIsWhite = true;
};

// Implementation

Game::Game() : window(sf::VideoMode(512, 512), "ChessBot") {
    board = std::make_unique<Board>();
    board->initializeStandardPosition();
    renderer = std::make_unique<ChessRenderer>(window, *board);
}

bool Game::initialize() {
    return renderer->initialize();
}

void Game::run() {
    if (!initialize()) {
        // Handle initialization error
        return;
    }
    
    // Main game loop
    while (window.isOpen() && !isGameOver) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                Move move = renderer->handleMouseClick(mousePos);
                
                // Try to make the move if valid
                if (move.startSquare != move.targetSquare) {
                    tryMakeMove(move);
                }
            }
        }
    }
}

void Game::update() {
    // Game logic updates here
    
    // Check for game end conditions
    MoveGenerator moveGen(*board);
    auto legalMoves = moveGen.generateLegalMoves();
    
    if (legalMoves.empty()) {
        if (board->isInCheck(board->getSideToMove())) {
            // Checkmate
            std::cout << (board->getSideToMove() == WHITE ? "Black" : "White") << " wins by checkmate!" << std::endl;
        } else {
            // Stalemate
            std::cout << "Draw by stalemate!" << std::endl;
        }
        isGameOver = true;
    }
}

void Game::render() {
    window.clear();
    renderer->drawBoard();
    window.display();
}

bool Game::tryMakeMove(const Move& move) {
    // Check if the move is legal
    MoveGenerator moveGen(*board);
    auto legalMoves = moveGen.generateLegalMoves();
    
    for (const auto& legalMove : legalMoves) {
        if (legalMove.startSquare == move.startSquare && legalMove.targetSquare == move.targetSquare) {
            // Legal move found, make it
            board->makeMove(move);
            return true;
        }
    }
    
    return false;
}

#endif // GAME_H