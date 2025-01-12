#include <iostream>
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <random>
#include <fstream>

// Save Game
// choose difficulty

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

enum MenuState {
    Play,
    Menu,
};

enum MainMenuState {
    MainMenu,
    PlayAndLoad,
};

struct Bullet {
    sf::RectangleShape shape;
};

struct Ship {
    sf::ConvexShape shape;
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

    void setShape() {
        sf::ConvexShape convex(12);
        // 50 x 20
        convex.setPoint(0, sf::Vector2f(10, 0));
        convex.setPoint(1, sf::Vector2f(0, -5));
        convex.setPoint(2, sf::Vector2f(0, -10));
        convex.setPoint(3, sf::Vector2f(10, -10));
        convex.setPoint(4, sf::Vector2f(10, -15));
        convex.setPoint(5, sf::Vector2f(15, -20));
        convex.setPoint(6, sf::Vector2f(35, -20));
        convex.setPoint(7, sf::Vector2f(40, -15));
        convex.setPoint(8, sf::Vector2f(40, -10));
        convex.setPoint(9, sf::Vector2f(50, -10));
        convex.setPoint(10, sf::Vector2f(50, -5));
        convex.setPoint(11, sf::Vector2f(40, 0));

        // convex.setOrigin(sf::Vector2f(50. / 2., 20. / 2.));

        shape = convex;
    }
};

struct House {
    sf::ConvexShape shape;
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

    void setShape() {
        sf::ConvexShape convex(8);
        convex.setPoint(0, sf::Vector2f(0, 0));
        convex.setPoint(1, sf::Vector2f(0, -30));
        convex.setPoint(2, sf::Vector2f(50, -30));
        convex.setPoint(3, sf::Vector2f(50, 0));
        convex.setPoint(4, sf::Vector2f(40, 0));
        convex.setPoint(5, sf::Vector2f(40, -10));
        convex.setPoint(6, sf::Vector2f(10, -10));
        convex.setPoint(7, sf::Vector2f(10, 0));

        // convex.setOrigin(sf::Vector2f(50. / 2., 30. / 2.));
        // 50 x 30

        shape = convex;
    }
};

struct Player {
    sf::ConvexShape shape;
    float speed;

    int currentLives = 3;
    int maxLives = 3;
    int totalLives = 4;

    bool isAlive = true;
    float respawnTimer = 0.0f;
    static constexpr float respawnDelay = 5.0f; // 5 seconds for respawn

    void setShape() {
        sf::ConvexShape convex(6);
        convex.setPoint(0, sf::Vector2f(0, 0));
        convex.setPoint(1, sf::Vector2f(0, -10));
        convex.setPoint(2, sf::Vector2f(15, -30));
        convex.setPoint(3, sf::Vector2f(40, -30));
        convex.setPoint(4, sf::Vector2f(55, -10));
        convex.setPoint(5, sf::Vector2f(55, 0));
        convex.setOrigin(sf::Vector2f(55. / 2., -30. / 2.));

        shape = convex;
    }

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

    void respawn(bool& isGameOver) {
        if (totalLives > 0) {
            isAlive = true;
            currentLives = maxLives;
            updateColor();
            shape.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2., WINDOW_SIZE.y - (WINDOW_SIZE.y * 0.1)));
        } else {
            isGameOver = true;  // Set game over when no lives left
        }
    }

    void update(float deltaTime, sf::Clock& shootClock, std::vector<Bullet>& bullets, bool& isGameOver) {
        if (!isAlive) {
            respawnTimer += deltaTime;
            if (respawnTimer >= respawnDelay) {
                respawn(isGameOver);
                respawnTimer = 0.0f;
            }
        } else {
            int dir = sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A);
            shape.move(sf::Vector2f(dir * speed * deltaTime, 0));

            // Player Shooting
            if (shootClock.getElapsedTime() > sf::seconds(0.45) && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
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
            sf::Vector2f playerSize = sf::Vector2f(shape.getPoint(5).x, shape.getPoint(2).y);
            if (playerPos.x - playerSize.x / 2. < 0) {
                shape.setPosition(sf::Vector2f(playerSize.x / 2., playerPos.y));
            }
            if (playerPos.x + playerSize.x / 2. > WINDOW_SIZE.x) {
                shape.setPosition(sf::Vector2f(WINDOW_SIZE.x - playerSize.x / 2., playerPos.y));
            }
        }
    }
};

// Helper functions
void centerBlockOnGrid(
    std::vector<Ship>& ships, sf::RenderWindow& window,
    int gridColumns, int gridRows,
    float marginX, float marginY
);

void centerHouseOnGrid(std::vector<House>& houses, sf::RenderWindow& window, float marginX);

struct GameData {
    sf::RenderWindow& window;
    Player player;
    sf::Font font;

    std::vector<Bullet> bullets;
    std::vector<Bullet> blockBullets;

    std::vector<Ship> ships;
    std::vector<House> houses;

    sf::Clock shootClock;
    sf::Clock moveClock;
    sf::Clock blockClock;
    sf::Clock restartClock;
    sf::Clock graceTimeClock;

    int score;
    int round;

    float angle;
    float orbitRadius;

    bool showPostRoundMenu;
    bool isPaused;
    bool isGameOver;

    void make() {
        player.currentLives = 2;
        player.maxLives = 2;
        player.totalLives = 3;
        player.isAlive = true;
        isGameOver = false;

        player.setShape();
        player.shape.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2., WINDOW_SIZE.y - (WINDOW_SIZE.y * 0.1)));
        player.speed = 250.f;

        player.updateColor();
        graceTimeClock.restart();

        round = 1;
        score = 0;

        angle = 0.0f;
        orbitRadius = 150.0f;

        houses.clear();
        ships.clear();
        bullets.clear();
        blockBullets.clear();

        int blockAmount = 50;
        for (int i = 0; i < blockAmount; i++) {
            Ship ship;
            ship.setShape();
            ship.shape.setFillColor(sf::Color::White);
            ships.push_back(ship);
        }

        int gridCol = 10;
        int gridRow = 5;
        float marginX = 10;
        float marginY = 15;

        centerBlockOnGrid(
            ships, window,
            gridCol, gridRow,
            marginX, marginY
        );

        int houseAmount = 4;
        for (int i = 0; i < houseAmount; i++) {
            House house;
            house.setShape();
            house.shape.setFillColor(sf::Color::White);
            houses.push_back(house);
        }

        centerHouseOnGrid(houses, window, 35.);
    }

    void saveGame() {
        // int round
        // int score
        // int player.currentLives
        // int player.totalLives;
        // std::vector<House> houses

        std::ofstream outFile("data.txt");
        if (outFile.is_open()) {
            // +1 casue we will be playing the next round
            outFile << round + 1 << std::endl;
            outFile << score << std::endl;

            outFile << player.currentLives << std::endl;
            outFile << player.totalLives << std::endl;

            outFile << houses.size() << std::endl;
            for (const auto& house : houses) {
                outFile << house.lives << std::endl;

                outFile << house.shape.getPosition().x << std::endl;
                outFile << house.shape.getPosition().y << std::endl;
            }
            outFile.close();
        }
    }

    void loadGame() {
        // int round
        // int score
        // int player.currentLives
        // int player.totalLives;
        // std::vector<House> houses

        std::ifstream inFile("data.txt");
        if (inFile.is_open()) {
            inFile >> round;
            inFile >> score;
            inFile >> player.currentLives;
            inFile >> player.totalLives;

            size_t houseSize;
            inFile >> houseSize;
            inFile.ignore();

            houses.clear();
            if (houseSize > 0) {
                for (size_t i = 0; i < houseSize; i++) {
                    House house;
                    house.setShape();

                    sf::Vector2f pos(0.0, 0.0);

                    inFile >> house.lives;
                    inFile >> pos.x;
                    inFile >> pos.y;

                    house.shape.setPosition(pos);
                    house.updateColor();

                    houses.push_back(house);
                }
            }
        }
    }
};

// Later make a resource type for every state
void playState(
    GameData& gameData, sf::Event& event,
    float dt, MenuState& menuState
);

void mainMenuState(
    GameData& gameData, MainMenuState& mainState,
    sf::Event& event, bool& isRunning,
    float& dt, MenuState& menuState
);

void playAndLoadState(
    GameData& gameData, MainMenuState& mainState,
    sf::Event& event, MenuState& menuState
);

void startNewRound(GameData& gameData);

sf::Text updateLivesText(const sf::Font& font, const int& totalLives);
sf::Text updateScoreText(const sf::Font& font, const int& score);

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE.x, WINDOW_SIZE.y), "Window");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return -1;

    // Gives error for some reason
    // It says that there is not a default constructor
    // for ConvexShape yet it compiles ?
    GameData gameData{ window };
    gameData.font = font;
    gameData.make();

    MenuState menuState = Menu;
    MainMenuState mainState = MainMenu;

    bool isRunning = true;
    sf::Clock deltaClock;

    while (isRunning) {
        sf::Event event;
        float dt = deltaClock.restart().asSeconds();

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                isRunning = false;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    gameData.isPaused = !gameData.isPaused;
                }
            }
        }

        switch (menuState)
        {
        case Menu:
            mainMenuState(
                gameData, mainState,
                event, isRunning,
                dt, menuState
            );
            break;
        case Play:
            playState(gameData, event, dt, menuState);
            break;
        }

        window.display();
    }

    window.close();
    return 0;
}

void playState(
    GameData& gameData, sf::Event& event,
    float dt, MenuState& menuState
) {
    sf::Text livesText = updateLivesText(gameData.font, gameData.player.totalLives);
    sf::Text scoreText = updateScoreText(gameData.font, gameData.score);

    sf::Text roundText("Round: " + std::to_string(gameData.round), gameData.font, 20);
    roundText.setFillColor(sf::Color::Yellow);
    roundText.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2 - 50, WINDOW_SIZE.y / 10. - 50.));

    if (gameData.ships.empty() && !gameData.showPostRoundMenu) {
        gameData.showPostRoundMenu = true;
    }

    // Restart game
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        gameData.make();
    }

    // Testing purpose
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
        gameData.ships.clear();
    }

    if (!gameData.showPostRoundMenu && !gameData.isPaused && !gameData.isGameOver) {

        gameData.player.update(dt, gameData.shootClock, gameData.bullets, gameData.isGameOver);

        // Move Player Bullets
        for (auto it = gameData.bullets.begin(); it != gameData.bullets.end(); ) {
            it->shape.move(0, -600 * dt);

            if (it->shape.getPosition().y + it->shape.getSize().y < 0) {
                it = gameData.bullets.erase(it);
            } else {
                ++it;
            }
        }

        // Bullet deals damage to ships
        for (int bulletId = 0; bulletId < gameData.bullets.size();) {
            bool bulletHit = false;
            for (int blockId = 0; blockId < gameData.ships.size(); blockId++) {
                sf::FloatRect bulletBounds = gameData.bullets[bulletId].shape.getGlobalBounds();
                sf::FloatRect blockBounds = gameData.ships[blockId].shape.getGlobalBounds();

                if (bulletBounds.intersects(blockBounds)) {
                    if (gameData.ships[blockId].lives <= 0) {
                        gameData.ships.erase(gameData.ships.begin() + blockId);
                        gameData.score += 50;
                    } else {
                        gameData.ships[blockId].lives--;
                        gameData.ships[blockId].updateColor();
                        gameData.score += 10;
                    }
                    bulletHit = true;
                    break;
                }
            }

            if (bulletHit) {
                gameData.bullets.erase(gameData.bullets.begin() + bulletId);
            } else {
                bulletId++;
            }
        }

        // Player bullets deals damage to the houses
        for (int bulletId = 0; bulletId < gameData.bullets.size();) {
            bool bulletHit = false;
            for (int houseId = 0; houseId < gameData.houses.size(); houseId++) {
                sf::FloatRect bulletBounds = gameData.bullets[bulletId].shape.getGlobalBounds();
                sf::FloatRect houseBounds = gameData.houses[houseId].shape.getGlobalBounds();

                if (bulletBounds.intersects(houseBounds)) {
                    if (gameData.houses[houseId].lives == 0) {
                        gameData.houses.erase(gameData.houses.begin() + houseId);
                    } else {
                        gameData.houses[houseId].lives--;
                        gameData.houses[houseId].updateColor();
                    }
                    bulletHit = true;
                    break;
                }
            }

            if (bulletHit) {
                gameData.bullets.erase(gameData.bullets.begin() + bulletId);
            } else {
                bulletId++;
            }
        }

        // Add a bit of a grace time at the start of the round/game
        std::vector<Ship> shootableBlocks;
        if (gameData.graceTimeClock.getElapsedTime() > sf::seconds(1.)) {
            // Get ships that can shoot
            for (auto& block : gameData.ships) {
                sf::FloatRect blockBounds = block.shape.getGlobalBounds();
                bool hasBlockBelow = false;

                for (auto& otherBlock : gameData.ships) {
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
        }

        // Lower the time needed for ships to shoot and move
        float harder;
        if (gameData.ships.size() > 2) {
            harder = (5. / gameData.ships.size()) + (gameData.round * 0.1f);
        } else {
            harder = 1.3 + (gameData.round * 0.1f);
        }

        harder = std::min(harder, 1.5f);

        // Move ships
        if (gameData.moveClock.getElapsedTime() > sf::seconds(2.0 - harder)) {
            for (auto& block : gameData.ships) {
                if (harder > 0.3) {
                    block.shape.move(sf::Vector2f(0.0, 3.0));
                } else {
                    block.shape.move(sf::Vector2f(0.0, 1.0));
                }
            }
            gameData.moveClock.restart();
        }

        // Random amount of bullets are shoot by random amount of the ships
        if (gameData.blockClock.getElapsedTime() > sf::seconds(3.0 - harder)) {
            std::vector<int> shootBlockId;

            if (!shootableBlocks.empty() && gameData.blockBullets.size() < 50) {
                int maxAmount = 4;

                if (shootableBlocks.size() < 5) {
                    maxAmount = shootableBlocks.size();
                }

                int minAmount = 0;

                int randAmount = rand() % (maxAmount - minAmount + 1) + minAmount;
                int i = 0;

                while (i < randAmount) {
                    int randInt = rand() % shootableBlocks.size();

                    if (std::count(shootBlockId.begin(), shootBlockId.end(), randInt)) {
                        continue;
                    } else {
                        shootBlockId.push_back(randInt);
                        i++;
                    }
                }

                for (const auto& id : shootBlockId) {
                    const Ship& shootBlock = shootableBlocks[id];
                    Bullet bullet{ sf::RectangleShape(sf::Vector2f(5, 15)) };

                    sf::Vector2f blockCenter = shootBlock.shape.getPosition() +
                        sf::Vector2f(50. / 2.f, 20.);

                    bullet.shape.setPosition(blockCenter);
                    bullet.shape.setOrigin(sf::Vector2f(2.5, 7.5));
                    bullet.shape.setFillColor(sf::Color::Red);

                    gameData.blockBullets.push_back(bullet);
                }

                gameData.blockClock.restart();
            }
        }

        // Check if ShootableBlocks are under certain position
        for (auto& block : shootableBlocks) {
            if (block.shape.getPosition().y >= WINDOW_SIZE.y * 0.71) {
                gameData.isGameOver = true;
            }
        }

        // Move block bullets
        for (auto it = gameData.blockBullets.begin(); it != gameData.blockBullets.end(); ) {
            it->shape.move(0, 600 * dt);

            if (it->shape.getPosition().y > WINDOW_SIZE.y) {
                it = gameData.blockBullets.erase(it);
            } else {
                ++it;
            }
        }

        // Ship bullets destroy houses
        for (int bulletId = 0; bulletId < gameData.blockBullets.size();) {
            bool bulletHit = false;
            for (int houseId = 0; houseId < gameData.houses.size(); houseId++) {
                sf::FloatRect bulletBounds = gameData.blockBullets[bulletId].shape.getGlobalBounds();
                sf::FloatRect houseBounds = gameData.houses[houseId].shape.getGlobalBounds();

                if (bulletBounds.intersects(houseBounds)) {
                    if (gameData.houses[houseId].lives == 0) {
                        gameData.houses.erase(gameData.houses.begin() + houseId);
                    } else {
                        gameData.houses[houseId].lives--;
                        gameData.houses[houseId].updateColor();
                    }
                    bulletHit = true;
                    break;
                }
            }

            if (bulletHit) {
                gameData.blockBullets.erase(gameData.blockBullets.begin() + bulletId);
            } else {
                bulletId++;
            }
        }

        // Ship bullets damages player
        for (int bulletId = 0; bulletId < gameData.blockBullets.size();) {
            bool bulletHit = false;
            sf::FloatRect bulletBounds = gameData.blockBullets[bulletId].shape.getGlobalBounds();
            sf::FloatRect playerBounds = gameData.player.shape.getGlobalBounds();

            if (bulletBounds.intersects(playerBounds) && gameData.player.isAlive) {
                gameData.player.currentLives--;
                gameData.player.updateColor();

                if (gameData.player.currentLives <= 0) {
                    gameData.player.totalLives--;
                    gameData.player.isAlive = false;
                    gameData.player.respawnTimer = 0.0f;
                }

                bulletHit = true;
            }

            if (bulletHit) {
                gameData.blockBullets.erase(gameData.blockBullets.begin() + bulletId);
            } else {
                bulletId++;
            }
        }
    }

    sf::CircleShape earth(500);
    earth.setPointCount(50);
    earth.setFillColor(sf::Color::Blue);
    earth.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2., WINDOW_SIZE.y + 400));
    earth.setOrigin(sf::Vector2f(earth.getRadius(), earth.getRadius()));

    gameData.window.clear(sf::Color::Black);

    gameData.window.draw(livesText);
    gameData.window.draw(scoreText);
    gameData.window.draw(roundText);

    gameData.window.draw(earth);

    for (auto bullet : gameData.bullets) {
        gameData.window.draw(bullet.shape);
    }

    for (auto bullet : gameData.blockBullets) {
        gameData.window.draw(bullet.shape);
    }

    for (auto block : gameData.ships) {
        gameData.window.draw(block.shape);
    }

    for (auto house : gameData.houses) {
        gameData.window.draw(house.shape);
    }

    gameData.window.draw(gameData.player.shape);

    // show Pause menu
    if (gameData.isPaused) {
        static MenuOverlay pauseMenu(gameData.font, "Paused", WINDOW_SIZE);
        static bool menuInitialized = false;

        if (!menuInitialized) {
            Button resumeButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f - 60),
                sf::Vector2f(120, 40),
                "Resume",
                gameData.font,
                [&]() {
                    gameData.isPaused = false;
                }
            );

            Button restartButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f),
                sf::Vector2f(120, 40),
                "Restart",
                gameData.font,
                [&]() {
                    gameData.isPaused = false;
                    gameData.round = 1;
                    gameData.showPostRoundMenu = false;
                    gameData.make();
                }
            );

            Button menuButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 60),
                sf::Vector2f(120, 40),
                "Main Menu",
                gameData.font,
                [&]() {
                    menuState = Menu;
                    gameData.showPostRoundMenu = false;
                    gameData.isPaused = false;
                }
            );

            pauseMenu.addButton(resumeButton);
            pauseMenu.addButton(restartButton);
            pauseMenu.addButton(menuButton);
            menuInitialized = true;
        }

        pauseMenu.handleEvent(event, gameData.window);
        pauseMenu.draw(gameData.window);
    }

    // Show Post round menu
    if (gameData.showPostRoundMenu) {
        static MenuOverlay postRoundMenu(gameData.font, "Round " + std::to_string(gameData.round) + " Complete!", WINDOW_SIZE);
        static bool menuInitialized = false;

        if (!menuInitialized) {
            Button continueButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f - 20),
                sf::Vector2f(120, 40),
                "Continue",
                gameData.font,
                [&]() {
                    gameData.round++;
                    gameData.showPostRoundMenu = false;
                    menuInitialized = false;
                    startNewRound(gameData);
                }
            );

            Button saveButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 40),
                sf::Vector2f(120, 40),
                "Save Game",
                gameData.font,
                [&]() { gameData.saveGame(); }
            );

            Button menuButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 100),
                sf::Vector2f(120, 40),
                "Main Menu",
                gameData.font,
                [&]() {
                    menuState = Menu;
                    gameData.showPostRoundMenu = false;
                    menuInitialized = false;
                }
            );

            Button restartButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 160),
                sf::Vector2f(120, 40),
                "Restart",
                gameData.font,
                [&]() {
                    gameData.round = 1;
                    gameData.showPostRoundMenu = false;
                    menuInitialized = false;
                    gameData.make();
                }
            );

            postRoundMenu.addButton(continueButton);
            postRoundMenu.addButton(saveButton);
            postRoundMenu.addButton(menuButton);
            postRoundMenu.addButton(restartButton);
            postRoundMenu.setScoreText(gameData.font, "Score: " + std::to_string(gameData.score));
            menuInitialized = true;
        }

        postRoundMenu.handleEvent(event, gameData.window);
        postRoundMenu.draw(gameData.window);
    }

    // Show Game Over menu
    if (gameData.isGameOver) {
        static MenuOverlay gameOverMenu(gameData.font, "Game Over!", WINDOW_SIZE);
        static bool menuInitialized = false;

        if (!menuInitialized) {
            Button restartButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f - 20),
                sf::Vector2f(120, 40),
                "Restart",
                gameData.font,
                [&]() {
                    gameData.isGameOver = false;
                    gameData.round = 1;
                    gameData.make();
                }
            );

            Button menuButton(
                sf::Vector2f(WINDOW_SIZE.x / 2.0f - 60, WINDOW_SIZE.y / 2.0f + 40),
                sf::Vector2f(120, 40),
                "Main Menu",
                gameData.font,
                [&]() {
                    gameData.isGameOver = false;
                    menuState = Menu;
                }
            );

            gameOverMenu.addButton(restartButton);
            gameOverMenu.addButton(menuButton);
            gameOverMenu.setScoreText(gameData.font, "Final Score: " + std::to_string(gameData.score));
            menuInitialized = true;
        }

        gameOverMenu.handleEvent(event, gameData.window);
        gameOverMenu.draw(gameData.window);
    }
}

void mainMenuState(
    GameData& gameData, MainMenuState& mainState,
    sf::Event& event, bool& isRunning,
    float& dt, MenuState& menuState
) {
    // Main Menu buttons
    Button playButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. - 20),
        sf::Vector2f(120, 40),
        "Play",
        gameData.font,
        [&]() {
            mainState = PlayAndLoad;
        }
    );

    Button exitButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. + 40),
        sf::Vector2f(120, 40),
        "Exit",
        gameData.font,
        [&isRunning]() {isRunning = false;}
    );

    // Play and Load buttons
    Button newGameButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. - 20),
        sf::Vector2f(120, 40),
        "New Game",
        gameData.font,
        [&]() {
            menuState = Play;
            // it's done second time cause Player when going back from game
            // may want to play again, therefore data has to be new
            gameData.make();
            mainState = MainMenu;
        }
    );

    Button loadGameButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. + 40),
        sf::Vector2f(120, 40),
        "Load Game",
        gameData.font,
        [&]() {
            menuState = Play;
            gameData.loadGame();
            mainState = MainMenu;
            startNewRound(gameData);
        }
    );

    Button backButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. + 100),
        sf::Vector2f(120, 40),
        "Back",
        gameData.font,
        [&]() {
            mainState = MainMenu;
        }
    );

    sf::Text text1("Pew-Pew", gameData.font, 30);
    text1.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2. - 110, WINDOW_SIZE.y / 2 - 125));
    text1.setFillColor(sf::Color::Red);

    sf::Text text2("Panic!", gameData.font, 55);
    text2.setPosition(sf::Vector2f(WINDOW_SIZE.x / 2. - 30, WINDOW_SIZE.y / 2. - 100));

    // Start of a bunch of math
    float centerX = WINDOW_SIZE.x / 2. - 100;
    float centerY = WINDOW_SIZE.y / 2. - 60;

    sf::CircleShape earth(250.f);
    earth.setFillColor(sf::Color::Blue);
    earth.setPosition(centerX, centerY);
    earth.setOrigin(earth.getRadius(), earth.getRadius());

    sf::CircleShape moon(80.f);
    moon.setFillColor(sf::Color(200, 200, 200));
    moon.setOrigin(moon.getRadius(), moon.getRadius());

    float a = gameData.orbitRadius + 170;
    float b = gameData.orbitRadius * 0.3f;

    float moonX = centerX + cos(gameData.angle) * a;
    float moonY = centerY + sin(gameData.angle) * b;

    float scaleFactor = 0.7f + 0.3f * (sin(gameData.angle) + 1.0f) / 2.0f;
    moon.setRadius(80.f * scaleFactor);
    moon.setOrigin(moon.getRadius(), moon.getRadius());

    float zPos = sin(gameData.angle);

    moon.setPosition(moonX, moonY);

    uint8_t brightness = static_cast<uint8_t>(200 * (0.7f + 0.3f * (zPos + 1.0f) / 2.0f));
    moon.setFillColor(sf::Color(brightness, brightness, brightness));
    // End of a bunch of math


    gameData.window.clear(sf::Color::Black);

    if (zPos < 0) {
        gameData.window.draw(moon);
        gameData.window.draw(earth);
    } else {
        gameData.window.draw(earth);
        gameData.window.draw(moon);
    }

    switch (mainState)
    {
    case MainMenu:
        playButton.handleEvent(event, gameData.window);
        exitButton.handleEvent(event, gameData.window);

        playButton.draw(gameData.window);
        exitButton.draw(gameData.window);
        break;
    case PlayAndLoad:
        newGameButton.handleEvent(event, gameData.window);
        loadGameButton.handleEvent(event, gameData.window);
        backButton.handleEvent(event, gameData.window);

        newGameButton.draw(gameData.window);
        loadGameButton.draw(gameData.window);
        backButton.draw(gameData.window);
        break;
    }

    gameData.window.draw(text1);
    gameData.window.draw(text2);

    gameData.angle += 0.7f * dt;
}

void playAndLoadState(
    GameData& gameData, MainMenuState& mainState,
    sf::Event& event, MenuState& menuState
) {
    Button newGameButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. - 20 - 60),
        sf::Vector2f(120, 40),
        "New Game",
        gameData.font,
        [&]() {
            menuState = Play;
            // it's done second time cause Player when going back from game
            // may want to play again, therefore data has to be new
            gameData.make();
            mainState = MainMenu;
        }
    );

    Button loadGameButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. - 20),
        sf::Vector2f(120, 40),
        "Load Game",
        gameData.font,
        [&]() {
            menuState = Play;
            gameData.loadGame();
            mainState = MainMenu;
            startNewRound(gameData);
        }
    );

    Button backButton(
        sf::Vector2f(WINDOW_SIZE.x / 2. - 60, WINDOW_SIZE.y / 2. - 20 + 60),
        sf::Vector2f(120, 40),
        "Back",
        gameData.font,
        [&]() {
            mainState = MainMenu;
        }
    );

    newGameButton.handleEvent(event, gameData.window);
    loadGameButton.handleEvent(event, gameData.window);
    backButton.handleEvent(event, gameData.window);

    gameData.window.clear(sf::Color::Black);
    newGameButton.draw(gameData.window);
    loadGameButton.draw(gameData.window);
    backButton.draw(gameData.window);
}

void centerBlockOnGrid(
    std::vector<Ship>& ships, sf::RenderWindow& window,
    int gridColumns, int gridRows,
    float marginX, float marginY
) {
    if (ships.empty() || gridColumns <= 0 || gridRows <= 0) return;

    sf::Vector2f rectSize = sf::Vector2f(50, 20);

    float gridWidth = gridColumns * rectSize.x + (gridColumns - 1) * marginX;
    float gridHeight = gridRows * rectSize.y + (gridRows - 1) * marginY;

    float startX = ((WINDOW_SIZE.x - gridWidth) / 2.0f);
    float startY = ((WINDOW_SIZE.y - gridHeight) / 2.0f) - (WINDOW_SIZE.y - gridHeight) * 0.30;

    int count = 0;
    for (int row = 0; row < gridRows; ++row) {
        int livesForRow = gridRows - row;

        for (int col = 0; col < gridColumns; ++col) {
            if (count >= ships.size()) break;

            float x = startX + col * (rectSize.x + marginX);
            float y = startY + row * (rectSize.y + marginY);

            ships[count].shape.setPosition(x, y);
            ships[count].lives = livesForRow;
            ships[count].maxLives = livesForRow;
            ships[count].updateColor();
            ++count;
        }
    }
}

void centerHouseOnGrid(std::vector<House>& houses, sf::RenderWindow& window, float marginX) {
    if (houses.empty()) return;

    // 50 x 30
    sf::Vector2f rectSize = sf::Vector2f(50, 30);

    int numHouses = houses.size();
    float largeMargin = marginX * 3;
    float totalWidth = (numHouses * rectSize.x) + ((numHouses - 1) * largeMargin);
    float startX = (WINDOW_SIZE.x - totalWidth) / 2.0f;
    float y = WINDOW_SIZE.y * 0.82f;

    for (int i = 0; i < numHouses; ++i) {
        float x = startX + i * (rectSize.x + largeMargin);
        houses[i].shape.setPosition(x, y);

        houses[i].updateColor();
    }
}

void startNewRound(GameData& gameData) {
    gameData.bullets.clear();
    gameData.blockBullets.clear();

    gameData.player.currentLives = std::min(gameData.player.currentLives + 1, gameData.player.maxLives);
    gameData.player.updateColor();
    gameData.player.isAlive = true;
    gameData.player.respawn(gameData.isGameOver);

    gameData.graceTimeClock.restart();

    // Create ships with increased health based on round
    gameData.ships.clear();
    int shipsAmount = 50 + (gameData.round - 1) * 3;
    for (int i = 0; i < shipsAmount; i++) {
        Ship ship;
        ship.setShape();
        ship.shape.setFillColor(sf::Color::White);
        gameData.ships.push_back(ship);
    }

    int gridCol = 10;
    int gridRow = (shipsAmount + gridCol - 1) / gridCol; // Calculate rows needed
    float marginX = 10;
    float marginY = 15;

    centerBlockOnGrid(
        gameData.ships, gameData.window,
        gridCol, gridRow,
        marginX, marginY
    );

    // Increase block health based on round
    for (auto& block : gameData.ships) {
        block.maxLives += (gameData.round - 1);
        block.lives = block.maxLives;
        block.updateColor();
    }

    // Repair houses slightly between rounds or make new ones
    if (gameData.houses.size() > 0) {
        for (auto& house : gameData.houses) {
            house.lives = std::min(house.lives + 2, house.maxLives);
            house.updateColor();
        }
    } else {
        int houseAmount = std::min(gameData.round, 4);
        for (int i = 0; i < houseAmount; i++) {
            House house;
            house.setShape();
            house.shape.setFillColor(sf::Color::White);
            gameData.houses.push_back(house);
        }

        centerHouseOnGrid(gameData.houses, gameData.window, 35.);
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
