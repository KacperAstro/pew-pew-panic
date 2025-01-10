#include <iostream>
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <random>

const sf::Vector2f WINDOW_SIZE = sf::Vector2f(800, 600);

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> callback;
    bool isHovered;

public:
    Button(const sf::Vector2f& position, const sf::Vector2f& size,
        const std::string& buttonText, const sf::Font& font,
        std::function<void()> onClick)
        : callback(onClick), isHovered(false) {

        shape.setPosition(position);
        shape.setSize(size);
        shape.setFillColor(sf::Color(100, 100, 100));
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::White);

        text.setFont(font);
        text.setString(buttonText);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);

        // Center text 
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f,
            textBounds.top + textBounds.height / 2.0f);
        text.setPosition(
            position.x + size.x / 2.0f,
            position.y + size.y / 2.0f
        );
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        bool mouseOver = shape.getGlobalBounds().contains(mousePos.x, mousePos.y);

        if (mouseOver) {
            if (!isHovered) {
                isHovered = true;
                shape.setFillColor(sf::Color(150, 150, 150));
            }
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                callback();
            }
        } else if (isHovered) {
            isHovered = false;
            shape.setFillColor(sf::Color(100, 100, 100));
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
};

class MenuOverlay {
private:
    sf::RectangleShape background;
    std::vector<Button> buttons;
    sf::Text titleText;
    sf::Text scoreText;

public:
    MenuOverlay(const sf::Font& font, const std::string& title, const sf::Vector2f& windowSize) {
        background.setSize(windowSize);
        background.setFillColor(sf::Color(0, 0, 0, 180));

        titleText.setFont(font);
        titleText.setString(title);
        titleText.setCharacterSize(40);
        titleText.setFillColor(sf::Color::White);

        sf::FloatRect textRect = titleText.getLocalBounds();
        titleText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        titleText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f - 150);
    }

    void addButton(const Button& button) {
        buttons.push_back(button);
    }

    void setScoreText(const sf::Font& font, const std::string& text) {
        scoreText.setFont(font);
        scoreText.setString(text);
        scoreText.setCharacterSize(30);
        scoreText.setFillColor(sf::Color::White);

        sf::FloatRect textRect = scoreText.getLocalBounds();
        scoreText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        scoreText.setPosition(WINDOW_SIZE.x / 2.0f, WINDOW_SIZE.y / 2.0f - 80);
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        for (auto& button : buttons) {
            button.handleEvent(event, window);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(background);
        window.draw(titleText);
        window.draw(scoreText);
        for (auto& button : buttons) {
            button.draw(window);
        }
    }
};

enum GameState {
    Play,
    Menu,
};

struct Bullet {
    sf::RectangleShape shape;
};

struct Block {
    sf::RectangleShape shape;
    int lives = 1;
    int maxLives = 1;

    void updateColor() {
        float healthPercent = static_cast<float>(lives) / maxLives;

        if (healthPercent == 1.0f) {
            shape.setFillColor(sf::Color::White);
        } else if (healthPercent >= 0.8f) {
            shape.setFillColor(sf::Color(173, 216, 230)); // Light Blue
        } else if (healthPercent >= 0.6f) {
            shape.setFillColor(sf::Color(144, 238, 144)); // Green
        } else if (healthPercent >= 0.4f) {
            shape.setFillColor(sf::Color::Yellow);
        } else if (healthPercent >= 0.2f) {
            shape.setFillColor(sf::Color(255, 165, 0)); // Orange
        } else {
            shape.setFillColor(sf::Color::Red);
        }
    }
};

struct House {
    sf::RectangleShape shape;
    int lives = 8;
    int maxLives = 8;

    void updateColor() {
        float healthPercent = static_cast<float>(lives) / maxLives;

        if (healthPercent == 1.0f) {
            shape.setFillColor(sf::Color::White);
        } else if (healthPercent >= 0.8f) {
            shape.setFillColor(sf::Color(173, 216, 230)); // Light Blue
        } else if (healthPercent >= 0.6f) {
            shape.setFillColor(sf::Color(144, 238, 144)); // Green
        } else if (healthPercent >= 0.4f) {
            shape.setFillColor(sf::Color::Yellow);
        } else if (healthPercent >= 0.2f) {
            shape.setFillColor(sf::Color(255, 165, 0)); // Orange
        } else {
            shape.setFillColor(sf::Color::Red);
        }
    }
};

struct Player {
    sf::RectangleShape shape;
    float speed;

    int currentLives = 3;
    int maxLives = 3;
    int totalLives = 4;

    bool isAlive = true;
    float respawnTimer = 0.0f;
    static constexpr float respawnDelay = 5.0f; // 5 seconds for respawn
    bool isGameOver = false;

    void updateColor() {
        if (!isAlive) {
            shape.setFillColor(sf::Color::Transparent);
            return;
        }

        float healthPercent = static_cast<float>(currentLives) / maxLives;

        if (healthPercent == 1.0f) {
            shape.setFillColor(sf::Color::White);
        } else if (healthPercent >= 0.8f) {
            shape.setFillColor(sf::Color(173, 216, 230)); // Light Blue
        } else if (healthPercent >= 0.6f) {
            shape.setFillColor(sf::Color(144, 238, 144)); // Green
        } else if (healthPercent >= 0.4f) {
            shape.setFillColor(sf::Color::Yellow);
        } else if (healthPercent >= 0.2f) {
            shape.setFillColor(sf::Color(255, 165, 0)); // Orange
        } else {
            shape.setFillColor(sf::Color::Red);
        }
    }

    void respawn() {
        if (totalLives > 0) {
            isAlive = true;
            currentLives = maxLives;
            updateColor();
            shape.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2., WINDOW_SIZE.y - (WINDOW_SIZE.y * 0.1)));
        } else {
            isGameOver = true;  // Set game over when no lives left
        }
    }

    void update(float deltaTime, sf::Clock& shootClock, std::vector<Bullet>& bullets) {
        if (!isAlive) {
            respawnTimer += deltaTime;
            if (respawnTimer >= respawnDelay) {
                respawn();
                respawnTimer = 0.0f;
            }
        } else {
            int dir = sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A);
            shape.move(sf::Vector2f(dir * speed * deltaTime, 0));

            // Player Shooting
            if (shootClock.getElapsedTime() > sf::seconds(0.5) && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                if (bullets.size() < 50) {
                    Bullet bullet{ sf::RectangleShape(sf::Vector2f(5, 15)) };
                    bullet.shape.setPosition(shape.getPosition());
                    bullet.shape.setOrigin(sf::Vector2f(2.5, 7.5));
                    bullet.shape.setFillColor(sf::Color::Green);

                    bullets.push_back(bullet);
                }
                shootClock.restart();
            }

            // Bound player pos to screen borders
            sf::Vector2f playerPos = shape.getPosition();
            sf::Vector2f playerSize = shape.getSize();
            if (playerPos.x - playerSize.x / 2. < 0) {
                shape.setPosition(sf::Vector2f(playerSize.x / 2., playerPos.y));
            }
            if (playerPos.x + playerSize.x / 2. > WINDOW_SIZE.x) {
                shape.setPosition(sf::Vector2f(WINDOW_SIZE.x - playerSize.x / 2., playerPos.y));
            }
        }
    }
};

// Later make a resource type for every state
void playState(
    sf::RenderWindow& window, Player& player,
    GameState& gameState, std::vector<Bullet>& bullets,
    sf::Clock& shootClock, std::vector<Block>& blocks,
    float dt, sf::Clock& moveClock,
    sf::Clock& blockClock, std::vector<Bullet>& blockBullets,
    std::vector<House>& houses, sf::Clock& restartClock,
    int& score, int& round,
    bool& showPostRoundMenu, sf::Event& event,
    bool& isPaused
);

void menuState(
    sf::RenderWindow& window, GameState& gameState,
    const sf::Event& event, bool& isRunning,
    Player& player, std::vector<Block>& blocks,
    std::vector<House>& houses, std::vector<Bullet>& bullets,
    std::vector<Bullet>& blockBullets, int& score,
    int& round
);
void pausedState(sf::RenderWindow& window, GameState& gameState);

void centerBlockOnGrid(
    std::vector<Block>& blocks, sf::RenderWindow& window,
    int gridColumns, int gridRows,
    float marginX, float marginY
);

void centerHouseOnGrid(std::vector<House>& houses, sf::RenderWindow& window, float marginX);

void startGame(
    Player& player, std::vector<Block>& blocks,
    std::vector<House>& houses, std::vector<Bullet>& bullets,
    std::vector<Bullet>& blockBullets, sf::RenderWindow& window,
    int& score, int& round
);

void startNewRound(
    Player& player, std::vector<Block>& blocks,
    std::vector<House>& houses, std::vector<Bullet>& bullets,
    std::vector<Bullet>& blockBullets, sf::RenderWindow& window,
    int round
);

sf::Text updateLivesText(const sf::Font& font, const int& totalLives);
sf::Text updateScoreText(const sf::Font& font, const int& score);

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE.x, WINDOW_SIZE.y), "Window");
    Player player{
        sf::RectangleShape(sf::Vector2f(60.0f, 20.0f)),
        250.0f,
    };

    std::vector<Bullet> bullets;
    std::vector<Block> blocks;
    std::vector<Bullet> blockBullets;
    std::vector<House> houses;

    int score = 0;
    int round = 1;

    GameState gameState = Menu;

    bool isRunning = true;
    bool showPostRoundMenu = false;
    bool isPaused = false;

    sf::Clock shootClock;
    sf::Clock deltaClock;
    sf::Clock moveClock;
    sf::Clock blockClock;
    sf::Clock restartClock;

    while (isRunning) {
        sf::Event event;
        float dt = deltaClock.restart().asSeconds();

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                isRunning = false;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    isPaused = !isPaused;
                }
            }
        }

        switch (gameState)
        {
        case Menu:
            menuState(
                window, gameState,
                event, isRunning,
                player, blocks,
                houses, bullets,
                blockBullets, score,
                round
            );
            break;
        case Play:
            playState(
                window, player,
                gameState, bullets,
                shootClock, blocks,
                dt, moveClock,
                blockClock, blockBullets,
                houses, restartClock,
                score, round,
                showPostRoundMenu, event,
                isPaused
            );
            break;
        }

        window.display();
    }

    window.close();
    return 0;
}

void playState(
    sf::RenderWindow& window, Player& player,
    GameState& gameState, std::vector<Bullet>& bullets,
    sf::Clock& shootClock, std::vector<Block>& blocks,
    float dt, sf::Clock& moveClock,
    sf::Clock& blockClock, std::vector<Bullet>& blockBullets,
    std::vector<House>& houses, sf::Clock& restartClock,
    int& score, int& round,
    bool& showPostRoundMenu, sf::Event& event,
    bool& isPaused
) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return;

    sf::Text livesText = updateLivesText(font, player.totalLives);
    sf::Text scoreText = updateScoreText(font, score);

    sf::Text roundText("Round: " + std::to_string(round), font, 20);
    roundText.setFillColor(sf::Color::Yellow);
    roundText.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2 - 50, WINDOW_SIZE.y / 10. - 50.));

    if (blocks.empty() && !showPostRoundMenu) {
        showPostRoundMenu = true;
    }

    // Restart game
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        startGame(player, blocks, houses, bullets, blockBullets, window, score, round);
    }

    // Testing purpose
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
        blocks.clear();
    }

    if (!showPostRoundMenu && !isPaused) {

        player.update(dt, shootClock, bullets);

        // Move Player Bullets
        for (auto it = bullets.begin(); it != bullets.end(); ) {
            it->shape.move(0, -600 * dt);

            if (it->shape.getPosition().y + it->shape.getSize().y < 0) {
                it = bullets.erase(it);
            } else {
                ++it;
            }
        }

        // Bullet deals damage to blocks
        for (int bulletId = 0; bulletId < bullets.size();) {
            bool bulletHit = false;
            for (int blockId = 0; blockId < blocks.size(); blockId++) {
                sf::FloatRect bulletBounds = bullets[bulletId].shape.getGlobalBounds();
                sf::FloatRect blockBounds = blocks[blockId].shape.getGlobalBounds();

                if (bulletBounds.intersects(blockBounds)) {
                    if (blocks[blockId].lives <= 0) {
                        blocks.erase(blocks.begin() + blockId);
                        score += 50;
                    } else {
                        blocks[blockId].lives--;
                        blocks[blockId].updateColor();
                        score += 10;
                    }
                    bulletHit = true;
                    break;
                }
            }

            if (bulletHit) {
                bullets.erase(bullets.begin() + bulletId);
            } else {
                bulletId++;
            }
        }

        // Player bullets deals damage to the houses
        for (int bulletId = 0; bulletId < bullets.size();) {
            bool bulletHit = false;
            for (int houseId = 0; houseId < houses.size(); houseId++) {
                sf::FloatRect bulletBounds = bullets[bulletId].shape.getGlobalBounds();
                sf::FloatRect houseBounds = houses[houseId].shape.getGlobalBounds();

                if (bulletBounds.intersects(houseBounds)) {
                    if (houses[houseId].lives == 0) {
                        houses.erase(houses.begin() + houseId);
                    } else {
                        houses[houseId].lives--;
                        houses[houseId].updateColor();
                    }
                    bulletHit = true;
                    break;
                }
            }

            if (bulletHit) {
                bullets.erase(bullets.begin() + bulletId);
            } else {
                bulletId++;
            }
        }

        // Get blocks that can shoot
        std::vector<Block> shootableBlocks;
        for (auto& block : blocks) {
            sf::FloatRect blockBounds = block.shape.getGlobalBounds();
            bool hasBlockBelow = false;

            for (auto& otherBlock : blocks) {
                if (&block == &otherBlock) continue;

                sf::FloatRect otherBlockBounds = otherBlock.shape.getGlobalBounds();

                if (blockBounds.left == otherBlockBounds.left &&
                    blockBounds.top + blockBounds.height + 15 /*MarginX*/ == otherBlockBounds.top) {
                    hasBlockBelow = true;
                    break;
                }
            }

            if (!hasBlockBelow) {
                // block.shape.setFillColor(sf::Color::Red);
                shootableBlocks.push_back(block);
            }
        }

        // Lower the time needed for blocks to shoot and move
        float harder;
        if (blocks.size() > 2) {
            harder = (5. / blocks.size()) + (round * 0.15f);
        } else {
            harder = 1.3 + (round * 0.15f);
        }

        harder = std::min(harder, 1.5f);

        // Move blocks
        if (moveClock.getElapsedTime() > sf::seconds(2.0 - harder)) {
            for (auto& block : blocks) {
                if (harder > 0.3) {
                    block.shape.move(sf::Vector2f(0.0, 3.0));
                } else {
                    block.shape.move(sf::Vector2f(0.0, 1.0));
                }
            }
            moveClock.restart();
        }

        // Random amount of bullets are shoot by random amount of the blocks
        if (blockClock.getElapsedTime() > sf::seconds(2.5 - harder)) {
            if (!shootableBlocks.empty() && blockBullets.size() < 50) {
                int maxAmount = 5;
                if (shootableBlocks.size() < 5) {
                    maxAmount = shootableBlocks.size();
                }
                int minAmount = 1;
                int randAmount = rand() % (maxAmount - minAmount + 1) + minAmount;

                for (int i = 0; i < randAmount; i++) {
                    int randInt = rand() % shootableBlocks.size();
                    const Block& shootBlock = shootableBlocks[randInt];

                    Bullet bullet{ sf::RectangleShape(sf::Vector2f(5, 15)) };

                    sf::Vector2f blockCenter = shootBlock.shape.getPosition() +
                        sf::Vector2f(shootBlock.shape.getSize().x / 2.f, shootBlock.shape.getSize().y);

                    bullet.shape.setPosition(blockCenter);
                    bullet.shape.setOrigin(sf::Vector2f(2.5, 7.5));
                    bullet.shape.setFillColor(sf::Color::Red);

                    blockBullets.push_back(bullet);
                }

                blockClock.restart();
            }
        }

        // Move block bullets
        for (auto it = blockBullets.begin(); it != blockBullets.end(); ) {
            it->shape.move(0, 600 * dt);

            if (it->shape.getPosition().y > WINDOW_SIZE.y) {
                it = blockBullets.erase(it);
            } else {
                ++it;
            }
        }

        // Block bullets destroy houses
        for (int bulletId = 0; bulletId < blockBullets.size();) {
            bool bulletHit = false;
            for (int houseId = 0; houseId < houses.size(); houseId++) {
                sf::FloatRect bulletBounds = blockBullets[bulletId].shape.getGlobalBounds();
                sf::FloatRect houseBounds = houses[houseId].shape.getGlobalBounds();

                if (bulletBounds.intersects(houseBounds)) {
                    if (houses[houseId].lives == 0) {
                        houses.erase(houses.begin() + houseId);
                    } else {
                        houses[houseId].lives--;
                        houses[houseId].updateColor();
                    }
                    bulletHit = true;
                    break;
                }
            }

            if (bulletHit) {
                blockBullets.erase(blockBullets.begin() + bulletId);
            } else {
                bulletId++;
            }
        }

        // Block bullets damages player
        for (int bulletId = 0; bulletId < blockBullets.size();) {
            bool bulletHit = false;

            sf::FloatRect bulletBounds = blockBullets[bulletId].shape.getGlobalBounds();
            sf::FloatRect playerBounds = player.shape.getGlobalBounds();

            if (bulletBounds.intersects(playerBounds) && player.isAlive) {
                player.currentLives--;
                player.updateColor();

                if (player.currentLives <= 0) {
                    player.totalLives--;
                    player.isAlive = false;
                    player.respawnTimer = 0.0f;
                }

                bulletHit = true;
            }

            if (bulletHit) {
                blockBullets.erase(blockBullets.begin() + bulletId);
            } else {
                bulletId++;
            }
        }

        // Show Game Over menu
        if (player.isGameOver) {
            static MenuOverlay gameOverMenu(font, "Game Over!", WINDOW_SIZE);
            static bool menuInitialized = false;

            if (!menuInitialized) {
                Button restartButton(
                    sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f - 20),
                    sf::Vector2f(120, 40),
                    "Restart",
                    font,
                    [&]() {
                        player.isGameOver = false;
                        round = 1;
                        startGame(player, blocks, houses, bullets, blockBullets, window, score, round);
                    }
                );

                Button menuButton(
                    sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 40),
                    sf::Vector2f(120, 40),
                    "Main Menu",
                    font,
                    [&]() {
                        player.isGameOver = false;
                        gameState = Menu;
                    }
                );

                gameOverMenu.addButton(restartButton);
                gameOverMenu.addButton(menuButton);
                gameOverMenu.setScoreText(font, "Final Score: " + std::to_string(score));
                menuInitialized = true;
            }

            gameOverMenu.handleEvent(event, window);
            gameOverMenu.draw(window);
            return;  // Skip regular game rendering when showing game over menu
        }
    }

    window.clear(sf::Color::Black);
    window.draw(player.shape);

    window.draw(livesText);
    window.draw(scoreText);

    for (auto bullet : bullets) {
        window.draw(bullet.shape);
    }

    for (auto bullet : blockBullets) {
        window.draw(bullet.shape);
    }

    for (auto block : blocks) {
        window.draw(block.shape);
    }

    for (auto house : houses) {
        window.draw(house.shape);
    }

    // show Pause menu
    if (isPaused) {
        static MenuOverlay pauseMenu(font, "Paused", WINDOW_SIZE);
        static bool menuInitialized = false;

        if (!menuInitialized) {
            Button resumeButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f - 60),
                sf::Vector2f(120, 40),
                "Resume",
                font,
                [&isPaused]() {
                    isPaused = false;
                }
            );

            Button restartButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f),
                sf::Vector2f(120, 40),
                "Restart",
                font,
                [&]() {
                    isPaused = false;
                    round = 1;
                    showPostRoundMenu = false;
                    startGame(player, blocks, houses, bullets, blockBullets, window, score, round);
                }
            );

            Button menuButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 60),
                sf::Vector2f(120, 40),
                "Main Menu",
                font,
                [&]() {
                    gameState = Menu;
                    showPostRoundMenu = false;
                    isPaused = false;
                }
            );

            pauseMenu.addButton(resumeButton);
            pauseMenu.addButton(restartButton);
            pauseMenu.addButton(menuButton);
            menuInitialized = true;
        }

        pauseMenu.handleEvent(event, window);
        pauseMenu.draw(window);
    }

    // Show Post round menu
    if (showPostRoundMenu) {
        static MenuOverlay postRoundMenu(font, "Round " + std::to_string(round) + " Complete!", WINDOW_SIZE);
        static bool menuInitialized = false;

        if (!menuInitialized) {
            Button continueButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f - 20),
                sf::Vector2f(120, 40),
                "Continue",
                font,
                [&]() {
                    round++;
                    showPostRoundMenu = false;
                    menuInitialized = false;
                    startNewRound(player, blocks, houses, bullets, blockBullets, window, round);
                }
            );

            Button saveButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 40),
                sf::Vector2f(120, 40),
                "Save Game",
                font,
                []() { /* Save game logic will be implemented by user */ }
            );

            Button menuButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 100),
                sf::Vector2f(120, 40),
                "Main Menu",
                font,
                [&]() {
                    gameState = Menu;
                    showPostRoundMenu = false;
                    menuInitialized = false;
                }
            );

            Button restartButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 160),
                sf::Vector2f(120, 40),
                "Restart",
                font,
                [&]() {
                    round = 1;
                    showPostRoundMenu = false;
                    menuInitialized = false;
                    startGame(player, blocks, houses, bullets, blockBullets, window, score, round);
                }
            );

            postRoundMenu.addButton(continueButton);
            postRoundMenu.addButton(saveButton);
            postRoundMenu.addButton(menuButton);
            postRoundMenu.addButton(restartButton);
            postRoundMenu.setScoreText(font, "Score: " + std::to_string(score));
            menuInitialized = true;
        }

        postRoundMenu.handleEvent(event, window);
        postRoundMenu.draw(window);
    }
}

void menuState(
    sf::RenderWindow& window, GameState& gameState,
    const sf::Event& event, bool& isRunning,
    Player& player, std::vector<Block>& blocks,
    std::vector<House>& houses, std::vector<Bullet>& bullets,
    std::vector<Bullet>& blockBullets, int& score,
    int& round
) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return;

    Button playButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. - 20),
        sf::Vector2f(120, 40),
        "Play",
        font,
        [&player, &blocks,
        &houses, &bullets,
        &blockBullets, &window,
        &score, &round,
        &gameState]() {
            startGame(player, blocks, houses, bullets, blockBullets, window, score, round);
            gameState = Play;
        }
    );

    Button exitButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. + 40),
        sf::Vector2f(120, 40),
        "Exit",
        font,
        [&isRunning]() {isRunning = false;}
    );

    sf::Text text("Game Title", font, 55);
    text.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2. - 130, WINDOW_SIZE.y / 2 - 150));

    playButton.handleEvent(event, window);
    exitButton.handleEvent(event, window);

    window.clear(sf::Color::Black);
    playButton.draw(window);
    exitButton.draw(window);
    window.draw(text);
}

void centerBlockOnGrid(
    std::vector<Block>& blocks, sf::RenderWindow& window,
    int gridColumns, int gridRows,
    float marginX, float marginY
) {
    if (blocks.empty() || gridColumns <= 0 || gridRows <= 0) return;

    sf::Vector2u windowSize = window.getSize();

    sf::Vector2f rectSize = blocks[0].shape.getSize();

    float gridWidth = gridColumns * rectSize.x + (gridColumns - 1) * marginX;
    float gridHeight = gridRows * rectSize.y + (gridRows - 1) * marginY;

    float startX = ((windowSize.x - gridWidth) / 2.0f);
    float startY = ((windowSize.y - gridHeight) / 2.0f) - (windowSize.y - gridHeight) * 0.40;

    int count = 0;
    for (int row = 0; row < gridRows; ++row) {
        int livesForRow = gridRows - row;

        for (int col = 0; col < gridColumns; ++col) {
            if (count >= blocks.size()) break;

            float x = startX + col * (rectSize.x + marginX);
            float y = startY + row * (rectSize.y + marginY);

            blocks[count].shape.setPosition(x, y);
            blocks[count].lives = livesForRow;
            blocks[count].maxLives = livesForRow;
            blocks[count].updateColor();
            ++count;
        }
    }
}

void centerHouseOnGrid(std::vector<House>& houses, sf::RenderWindow& window, float marginX) {
    if (houses.empty()) return;

    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f rectSize = houses[0].shape.getSize();

    int numHouses = houses.size();
    float largeMargin = marginX * 3;
    float totalWidth = (numHouses * rectSize.x) + ((numHouses - 1) * largeMargin);
    float startX = (windowSize.x - totalWidth) / 2.0f;
    float y = windowSize.y * 0.8f;

    for (int i = 0; i < numHouses; ++i) {
        float x = startX + i * (rectSize.x + largeMargin);
        houses[i].shape.setPosition(x, y);

        houses[i].updateColor();
    }
}

void startGame(
    Player& player, std::vector<Block>& blocks,
    std::vector<House>& houses, std::vector<Bullet>& bullets,
    std::vector<Bullet>& blockBullets, sf::RenderWindow& window,
    int& score, int& round
) {
    player.currentLives = 2;
    player.maxLives = 2;
    player.totalLives = 3;
    player.isAlive = true;

    player.shape.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2., WINDOW_SIZE.y - (WINDOW_SIZE.y * 0.1)));
    player.shape.setOrigin(sf::Vector2f(30.0f, 10.0f));

    player.updateColor();

    round = 1;
    score = 0;

    houses.clear();
    blocks.clear();
    bullets.clear();
    blockBullets.clear();

    int blockAmount = 50;
    for (int i = 0; i < blockAmount; i++) {
        Block block{ sf::RectangleShape(sf::Vector2f(50, 20)) };
        block.shape.setFillColor(sf::Color::White);
        blocks.push_back(block);
    }

    int gridCol = 10;
    int gridRow = 5;
    float marginX = 10;
    float marginY = 15;

    centerBlockOnGrid(
        blocks, window,
        gridCol, gridRow,
        marginX, marginY
    );

    int houseAmount = 4;
    for (int i = 0; i < houseAmount; i++) {
        House house{ sf::RectangleShape(sf::Vector2f(50, 30)) };
        house.shape.setFillColor(sf::Color::White);
        houses.push_back(house);
    }

    centerHouseOnGrid(houses, window, 35.);
}

void startNewRound(
    Player& player, std::vector<Block>& blocks,
    std::vector<House>& houses, std::vector<Bullet>& bullets,
    std::vector<Bullet>& blockBullets, sf::RenderWindow& window,
    int round
) {
    // Clear existing bullets
    bullets.clear();
    blockBullets.clear();

    // Restore some player health as a round bonus
    player.currentLives = std::min(player.currentLives + 1, player.maxLives);
    player.updateColor();

    // Create blocks with increased health based on round
    int blockAmount = 50 + (round - 1) * 5; // More blocks each round
    for (int i = 0; i < blockAmount; i++) {
        Block block{ sf::RectangleShape(sf::Vector2f(50, 20)) };
        block.shape.setFillColor(sf::Color::White);
        blocks.push_back(block);
    }

    int gridCol = 10;
    int gridRow = (blockAmount + gridCol - 1) / gridCol; // Calculate rows needed
    float marginX = 10;
    float marginY = 15;

    centerBlockOnGrid(
        blocks, window,
        gridCol, gridRow,
        marginX, marginY
    );

    // Increase block health based on round
    for (auto& block : blocks) {
        block.maxLives += (round - 1);
        block.lives = block.maxLives;
        block.updateColor();
    }

    // Repair houses slightly between rounds
    for (auto& house : houses) {
        house.lives = std::min(house.lives + 2, house.maxLives);
        house.updateColor();
    }
}

sf::Text updateLivesText(const sf::Font& font, const int& totalLives) {
    sf::Text livesText("", font, 20);
    std::string str;

    for (int i = 0; i < totalLives; i++) {
        str += "<3 ";
    }

    livesText.setString(str);
    livesText.setFillColor(sf::Color::Red);
    livesText.setPosition(sf::Vector2f(10., WINDOW_SIZE.y / 10. - 50.));

    return livesText;
}

sf::Text updateScoreText(const sf::Font& font, const int& score) {
    sf::Text scoreText("Score: ", font, 20);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setFillColor(sf::Color::Green);
    scoreText.setPosition(sf::Vector2f(WINDOW_SIZE.x - 120., WINDOW_SIZE.y / 10. - 50.));

    return scoreText;
}
