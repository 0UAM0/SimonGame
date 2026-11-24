#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <optional>
using namespace std;
using namespace sf;

const int WINDOW_W = 800;
const int WINDOW_H = 750;
const string BUTTONS[4] = {"buttonGreen.png", 
                           "buttonRed.png", 
                           "buttonYellow.png", 
                           "buttonBlue.png"};
const string PAUSE_BUTTON = "buttonPause.png";
const string RESUME_BUTTON = "buttonResume.png";
const string MAIN_MENU_BUTTON = "buttonMainMenu.png";
const Color BACKGROUND_COLOUR(25, 25, 45);
const Color MAIN_COLOUR(0, 255, 34);
const Color BUTTON_GLOW_COLOURS[4] = {Color(0, 255, 0),
                                      Color(255, 0, 0),
                                      Color(255, 255, 0),
                                      Color(0, 0, 255)};
const Time CLICK_FEEDBACK_DURATION = seconds(0.25f);

bool goToMenu = false;

void createWindow(RenderWindow& window);
bool loadButtons(Sprite buttons[4], Texture textures[4], Texture& pauseTexture, Sprite& pauseSprite, Texture& resumeTexture, Sprite& resumeSprite, Texture& mainMenuTexture, Sprite& mainMenuSprite);
void centerText(Text& text, float x, float y);
void drawGameName(RenderWindow& window, Font& font);
void drawScore(RenderWindow& window, Font& font, int score);
void drawButtonGlow(RenderWindow& window, Sprite buttons[4], int buttonIndex, const Color& glowColour);
int detectMouseHover(RenderWindow& window, Sprite buttons[4]);
int detectMouseClick(RenderWindow& window, Sprite buttons[4], const optional<Event>& event);
bool pauseButton(RenderWindow& window, Sprite& pauseSprite, const optional<Event>& event, Font& font, Sprite& resumeSprite, Sprite& mainMenuSprite, bool& acceptingInput, bool& patternDisplayed, Clock& roundClock);
void clickFeedback(RenderWindow& window, Sprite buttons[4], int clickedButton, Time elapsed);
void drawPauseBox(RenderWindow& window, Font& font, Sprite& resumeSprite, Sprite& mainMenuSprite);
void drawLoseScreen(RenderWindow& window, Font& font, int score);
vector<int> patternGenerate();
void resetGame(Sprite buttons[4], vector<int>& pattern, int& score, int& inputIndex, int& feedbackButton, bool& acceptingInput, bool& patternDisplayed, bool& gameOver, Clock& roundClock);
void displayPattern(RenderWindow& window, Font& font, Sprite buttons[4], Sprite& pauseSprite, Sprite& resumeSprite, Sprite& mainMenuSprite, const vector<int>& pattern, int difficulty, int score, bool& acceptingInput, bool& patternDisplayed, Clock& roundClock);

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    RenderWindow window;
    createWindow(window);


    // 0 = Main Menu
    // 1 = Difficulty Menu
    // 2 = Game
    // 3 = Credits
    int screen = 0;

    Font font;
    if(!font.openFromFile("font.ttf") && !font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        cout << "Font not found! Place a font file named font.ttf next to the program." << endl;
        return 1;
    }

    Texture textures[4];
    Sprite buttons[4] = {Sprite(textures[0]),
                         Sprite(textures[1]),
                         Sprite(textures[2]),
                         Sprite(textures[3])};
    Texture pauseTexture;
    Sprite pauseSprite(pauseTexture);
    Texture resumeTexture;
    Sprite resumeSprite(resumeTexture);
    Texture mainMenuTexture;
    Sprite mainMenuSprite(mainMenuTexture);
    if(!loadButtons(buttons, textures, pauseTexture, pauseSprite, resumeTexture, resumeSprite, mainMenuTexture, mainMenuSprite))
    {
        return 1;
    }

    SoundBuffer buttonPressSound;
    if(!buttonPressSound.loadFromFile("buttonPress.mp3"))
    {
        cerr << "Sound file not found! Place a sound file named buttonPress.mp3 next to the program." << endl;
        return 1;
    }
    Sound buttonPressSoundEffect(buttonPressSound);

    int feedbackButton = -1;
    Clock feedbackClock;
    int score = 0;
    int inputIndex = 0;
    vector<int> pattern;
    Clock roundClock;
    bool patternDisplayed = false;
    bool acceptingInput = false;
    bool gameOver = false;
    int difficulty; // 0 = Easy, 1 = Medium, 2 = Hard.

    Text title(font, "S I M O N", 50);
    title.setFillColor(MAIN_COLOUR);
    centerText(title, WINDOW_W / 2.f, 130.f);

    Text difficultyTitle(font, "SELECT DIFFICULTY", 40);
    difficultyTitle.setFillColor(MAIN_COLOUR);
    centerText(difficultyTitle, WINDOW_W / 2.f, 145.f);

    RectangleShape menuButtons[3];
    Text menuText[3] = {
        Text(font, "START GAME", 25),
        Text(font, "CREDITS", 25),
        Text(font, "EXIT", 25)
    };

    RectangleShape levelButtons[3];
    Text levelText[3] = {
        Text(font, "EASY", 25),
        Text(font, "MEDIUM", 25),
        Text(font, "HARD", 25)
    };

    for(int i = 0; i < 3; i++)
    {
        menuButtons[i].setSize({250, 60});
        menuButtons[i].setPosition({275, 250 + i * 85.f});
        menuButtons[i].setFillColor(Color(50, 100, 180));

        levelButtons[i].setSize({250, 60});
        levelButtons[i].setPosition({275, 250 + i * 85.f});
        levelButtons[i].setFillColor(Color(50, 100, 180));
    }

    for(int i = 0; i < 3; i++)
    {
        menuText[i].setFillColor(MAIN_COLOUR);
        levelText[i].setFillColor(MAIN_COLOUR);
    }

    for(int i = 0; i < 3; i++)
    {
        const Vector2f buttonCenter = menuButtons[i].getGlobalBounds().getCenter();
        centerText(menuText[i], buttonCenter.x, buttonCenter.y);
    }

    for(int i = 0; i < 3; i++)
    {
        const Vector2f buttonCenter = levelButtons[i].getGlobalBounds().getCenter();
        centerText(levelText[i], buttonCenter.x, buttonCenter.y);
    }

    Text credits(font, "Developed by\n\nMuhammad Talha\nUmer Assad\nMuhammad Nauman", 25);
    credits.setPosition({300, 180});

    Text backText(font, "Press ESC to go back", 20);
    backText.setPosition({310, 650});

    Text creditsBackText = backText;
    backText.setFillColor(MAIN_COLOUR);

    while(window.isOpen())
    {
        Vector2f mouse(Mouse::getPosition(window));

        while(const optional event = window.pollEvent())
        {
            if(event->is<Event::Closed>())
            {
                window.close();
            }

            if(const auto* key = event->getIf<Event::KeyPressed>())
            {
                if(key->code == Keyboard::Key::Escape)
                {
                    screen = 0;
                }
            }

            if(screen == 2)
            {
                if(gameOver)
                {
                    if(event->is<Event::MouseButtonPressed>())
                    {
                        screen = 0;
                    }
                    continue;
                }

                if(pauseButton(window, pauseSprite, event, font, resumeSprite, mainMenuSprite, acceptingInput, patternDisplayed, roundClock))
                {
                    if(goToMenu)
                    {
                        goToMenu = false;
                        screen = 0;
                    }
                    continue;
                }

                const int clickedButton = detectMouseClick(window, buttons, event);
                if(clickedButton != -1)
                {
                    feedbackButton = clickedButton;
                    feedbackClock.restart();
                    buttonPressSoundEffect.play();

                    if(!acceptingInput)
                    {
                        continue;
                    }

                    if(clickedButton == pattern[inputIndex])
                    {
                        inputIndex++;
                        if(inputIndex == static_cast<int>(pattern.size()))
                        {
                            score++;
                            pattern.push_back(rand() % 4);
                            inputIndex = 0;
                            acceptingInput = false;
                            patternDisplayed = false;
                            roundClock.restart();
                        }
                    }
                    else
                    {
                        gameOver = true;
                    }
                }
            }
            else if(event->is<Event::MouseButtonPressed>())
            {
                if(screen == 0)
                {
                    if(menuButtons[0].getGlobalBounds().contains(mouse))
                        screen = 1;

                    if(menuButtons[1].getGlobalBounds().contains(mouse))
                        screen = 3;

                    if(menuButtons[2].getGlobalBounds().contains(mouse))
                        window.close();
                }
                else if(screen == 1)
                {
                    for(int i = 0; i < 3; i++)
                    {
                        if(levelButtons[i].getGlobalBounds().contains(mouse))
                        {
                            difficulty = i; //0 = Easy, 1 = Medium, 2 = Hard
                            resetGame(buttons, pattern, score, inputIndex, feedbackButton, acceptingInput, patternDisplayed, gameOver, roundClock);
                            screen = 2;
                        }
                    }
                }
                else if(screen == 3)
                {
                    screen = 0;
                }
            }
        }

        if(screen == 2 && !gameOver && !patternDisplayed && roundClock.getElapsedTime() >= seconds(0.7f))
        {
            patternDisplayed = true;
            displayPattern(window, font, buttons, pauseSprite, resumeSprite, mainMenuSprite, pattern, difficulty, score, acceptingInput, patternDisplayed, roundClock);

            if(goToMenu)
            {
                goToMenu = false;
                screen = 0;
            }
        }

        window.clear(BACKGROUND_COLOUR);

        if(screen == 0)
        {
            window.draw(title);

            for(int i = 0; i < 3; i++)
            {
                if(menuButtons[i].getGlobalBounds().contains(mouse))
                    menuButtons[i].setFillColor(Color(100, 150, 230));
                else
                    menuButtons[i].setFillColor(Color(50, 100, 180));

                window.draw(menuButtons[i]);
                window.draw(menuText[i]);
            }
        }
        else if(screen == 1)
        {
            window.draw(difficultyTitle);

            for(int i = 0; i < 3; i++)
            {
                if(levelButtons[i].getGlobalBounds().contains(mouse))
                    levelButtons[i].setFillColor(Color(100, 150, 230));
                else
                    levelButtons[i].setFillColor(Color(50, 100, 180));

                window.draw(levelButtons[i]);
                window.draw(levelText[i]);
            }

            window.draw(backText);
        }
        else if(screen == 2)
        {
            drawGameName(window, font);
            drawScore(window, font, score);

            clickFeedback(window, buttons, feedbackButton, feedbackClock.getElapsedTime());
            for(int i = 0; i < 4; i++)
                window.draw(buttons[i]);

            if(gameOver)
            {
                drawLoseScreen(window, font, score);
            }
            else
            {
                pauseButton(window, pauseSprite, nullopt, font, resumeSprite, mainMenuSprite, acceptingInput, patternDisplayed, roundClock);
            }
        }
        else if(screen == 3)
        {
            window.draw(credits);
            window.draw(creditsBackText);
        }

        window.display();
    }

    return 0;
}

void createWindow(RenderWindow& window)
{
    window.create(VideoMode(Vector2u(WINDOW_W, WINDOW_H)), "Simon Game");
    window.setFramerateLimit(60);
}

bool loadButtons(Sprite buttons[4], Texture textures[4], Texture& pauseTexture, Sprite& pauseSprite,Texture& resumeTexture, Sprite& resumeSprite, Texture& mainMenuTexture, Sprite& mainMenuSprite)
{
    for(int i = 0; i < 4; i++)
    {
        if(!textures[i].loadFromFile(BUTTONS[i]))
        {
            cerr << "Error loading texture: " << BUTTONS[i] << endl;
            return false;
        }
        buttons[i].setTexture(textures[i], true);
        buttons[i].setOrigin(Vector2f(128.f, 128.f));
    }

    buttons[0].setPosition(Vector2f(228.f, 248.f)); // Green button
    buttons[1].setPosition(Vector2f(548.f, 248.f)); // Red button
    buttons[2].setPosition(Vector2f(228.f, 548.f)); // Yellow button
    buttons[3].setPosition(Vector2f(548.f, 548.f)); // Blue button

    if(!pauseTexture.loadFromFile(PAUSE_BUTTON))
    {
        cerr << "Error loading texture: " << PAUSE_BUTTON << endl;
        return false;
    }
    pauseSprite.setTexture(pauseTexture, true);
    pauseSprite.setOrigin(Vector2f(32.f, 32.f));
    pauseSprite.setPosition(Vector2f(768.f, 32.f));

    if(!resumeTexture.loadFromFile(RESUME_BUTTON))
    {
        cerr << "Error loading texture: " << RESUME_BUTTON << endl;
        return false;
    }
    resumeSprite.setTexture(resumeTexture, true);
    resumeSprite.setTextureRect(IntRect({8,76},{240,92}));
    resumeSprite.setOrigin(Vector2f(120.f,46.f));
    resumeSprite.setPosition(Vector2f(400.f, 320.f));

    if(!mainMenuTexture.loadFromFile(MAIN_MENU_BUTTON))
    {
        cerr << "Error loading texture: " << MAIN_MENU_BUTTON << endl;
        return false;
    }
    mainMenuSprite.setTexture(mainMenuTexture, true);
    mainMenuSprite.setTextureRect(IntRect({8,76},{240,92}));
    mainMenuSprite.setOrigin(Vector2f(120.f,46.f));
    mainMenuSprite.setPosition(Vector2f(400.f, 430.f));
    return true;
}

void centerText(Text& text, float x, float y)
{
    const FloatRect bounds = text.getLocalBounds();
    text.setPosition(Vector2f(x, y) - bounds.getCenter());
}

void drawGameName(RenderWindow& window, Font& font)
{
    Text gameName(font, "S I M O N", 72);
    gameName.setFillColor(MAIN_COLOUR);
    const FloatRect boundsGameName = gameName.getLocalBounds();
    gameName.setPosition(Vector2f(388.f - boundsGameName.getCenter().x, 40.f - boundsGameName.position.y));
    window.draw(gameName);
}

void drawScore(RenderWindow& window, Font& font, int score)
{
    Text scoreText(font, "Score: " + to_string(score), 28);
    scoreText.setFillColor(Color(255, 255, 255));
    scoreText.setPosition(Vector2f(20.f, 20.f));
    window.draw(scoreText);
}

void drawPauseBox(RenderWindow& window, Font& font, Sprite& resumeSprite, Sprite& mainMenuSprite)
{
    RectangleShape pauseBox(Vector2f(500.f, 500.f));
    pauseBox.setPosition(Vector2f(150.f, 125.f));
    pauseBox.setFillColor(BACKGROUND_COLOUR);
    pauseBox.setOutlineColor(MAIN_COLOUR);
    pauseBox.setOutlineThickness(3.f);

    Text pausedText(font, "PAUSED", 48);
    pausedText.setFillColor(MAIN_COLOUR);
    const FloatRect pausedBounds = pausedText.getLocalBounds();
    pausedText.setPosition(Vector2f(400.f - pausedBounds.getCenter().x, 155.f));

    window.draw(pauseBox);
    window.draw(pausedText);
    window.draw(resumeSprite);
    window.draw(mainMenuSprite);
}

void drawLoseScreen(RenderWindow& window, Font& font, int score)
{
    RectangleShape overlay(Vector2f(static_cast<float>(WINDOW_W), static_cast<float>(WINDOW_H)));
    overlay.setFillColor(Color(25, 25, 45, 200));
    window.draw(overlay);

    Text loseText(font, "You Lose!", 80);
    loseText.setFillColor(Color(255, 60, 60));
    centerText(loseText, WINDOW_W / 2.f, 300.f);
    window.draw(loseText);

    Text finalScore(font, "Final Score: " + to_string(score), 36);
    finalScore.setFillColor(Color(255, 255, 255));
    centerText(finalScore, WINDOW_W / 2.f, 400.f);
    window.draw(finalScore);

    Text clickText(font, "Click anywhere to return to the main menu", 24);
    clickText.setFillColor(MAIN_COLOUR);
    centerText(clickText, WINDOW_W / 2.f, 470.f);
    window.draw(clickText);
}

void drawButtonGlow(RenderWindow& window, Sprite buttons[4], int buttonIndex, const Color& glowColour)
{
    for(int i = 0; i < 4; i++)
    {
        buttons[i].setScale(Vector2f(1.f, 1.f));
        buttons[i].setColor(Color(255, 255, 255));
    }
    buttons[buttonIndex].setScale(Vector2f(0.85f, 0.85f));
    buttons[buttonIndex].setColor(Color(145, 145, 145));

    const FloatRect boundsButton = buttons[buttonIndex].getGlobalBounds();
    const float radiusGlow = max(boundsButton.size.x, boundsButton.size.y) * 0.52f;
    const Vector2f buttonCenter = boundsButton.getCenter();


    CircleShape glow(radiusGlow, 64);
    glow.setOrigin(Vector2f(radiusGlow, radiusGlow));
    glow.setPosition(buttonCenter);
    glow.setFillColor(glowColour);
    window.draw(glow);
}

int detectMouseHover(RenderWindow& window, Sprite buttons[4])
{
    const Vector2i mousePosition = Mouse::getPosition(window);
    const Vector2f mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));

    for(int i = 0; i < 4; i++)
    {
        const FloatRect boundsButton = buttons[i].getGlobalBounds();
        const Vector2f buttonCenter = boundsButton.getCenter();
        const float buttonRadius = min(boundsButton.size.x, boundsButton.size.y) * 0.35f;
        const float horizontalDistance = mousePositionF.x - buttonCenter.x;
        const float verticalDistance = mousePositionF.y - buttonCenter.y;

        if(horizontalDistance * horizontalDistance + verticalDistance * verticalDistance <= buttonRadius * buttonRadius)
        {
            return i;
        }
    }

    return -1;
}

int detectMouseClick(RenderWindow& window, Sprite buttons[4], const optional<Event>& event)
{
    if(event.has_value() && event->is<Event::MouseButtonPressed>())
    {
        return detectMouseHover(window, buttons);
    }

    return -1;
}

bool pauseButton(RenderWindow& window, Sprite& pauseSprite, const optional<Event>& event, Font& font, Sprite& resumeSprite, Sprite& mainMenuSprite, bool& acceptingInput, bool& patternDisplayed, Clock& roundClock)
{
    if(event.has_value() && event->is<Event::MouseButtonPressed>())
    {
        const Vector2i mousePosition = Mouse::getPosition(window);
        const Vector2f mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
        const FloatRect bounds = pauseSprite.getGlobalBounds();
        const Vector2f center = bounds.getCenter();
        const float radius = min(bounds.size.x, bounds.size.y) * 0.35f;
        const float horizontalDistance = mousePositionF.x - center.x;
        const float verticalDistance = mousePositionF.y - center.y;

        if(horizontalDistance * horizontalDistance + verticalDistance * verticalDistance <= radius * radius)
        {
            bool paused = true;
            acceptingInput = false;

            while(window.isOpen() && paused)
            {
                while(const optional pausedEvent = window.pollEvent())
                {
                    if(pausedEvent->is<Event::Closed>())
                    {
                        window.close();
                        return true;
                    }

                    if(pausedEvent->is<Event::MouseButtonPressed>())
                    {
                        const Vector2i clickPosition = Mouse::getPosition(window);
                        const Vector2f clickPositionF(static_cast<float>(clickPosition.x), static_cast<float>(clickPosition.y));

                        if(resumeSprite.getGlobalBounds().contains(clickPositionF))
                        {
                            paused = false;
                            acceptingInput = false;
                            patternDisplayed = false;
                            roundClock.restart();
                            break;
                        }

                        if(mainMenuSprite.getGlobalBounds().contains(clickPositionF))
                        {
                            paused = false;
                            goToMenu = true;
                            acceptingInput = false;
                            break;
                        }
                    }
                }

                if(window.isOpen() && paused)
                {
                    window.clear(BACKGROUND_COLOUR);
                    drawPauseBox(window, font, resumeSprite, mainMenuSprite);
                    window.display();
                }
            }
            return true;
        }
    }

    window.draw(pauseSprite);
    return false;
}

void clickFeedback(RenderWindow& window, Sprite buttons[4], int clickedButton, Time elapsed)
{
    if(clickedButton == -1)
    {
        return;
    }

    if(elapsed >= CLICK_FEEDBACK_DURATION)
    {
        buttons[clickedButton].setScale(Vector2f(1.f, 1.f));
        buttons[clickedButton].setColor(Color(255, 255, 255));
        return;
    }

    const float progress = elapsed.asSeconds() / CLICK_FEEDBACK_DURATION.asSeconds();
    const uint8_t opacity = static_cast<uint8_t>(125.f * (1.f - progress));
    drawButtonGlow(window, buttons, clickedButton, Color(BUTTON_GLOW_COLOURS[clickedButton].r, BUTTON_GLOW_COLOURS[clickedButton].g, BUTTON_GLOW_COLOURS[clickedButton].b, opacity));
}

vector<int> patternGenerate()
{
    return {rand() % 4};
}

void resetGame(Sprite buttons[4], vector<int>& pattern, int& score, int& inputIndex, int& feedbackButton, bool& acceptingInput, bool& patternDisplayed, bool& gameOver, Clock& roundClock)
{
    for(int i = 0; i < 4; i++)
    {
        buttons[i].setScale(Vector2f(1.f, 1.f));
        buttons[i].setColor(Color(255, 255, 255));
    }

    score = 0;
    inputIndex = 0;
    feedbackButton = -1;
    pattern = patternGenerate();
    acceptingInput = false;
    patternDisplayed = false;
    gameOver = false;
    roundClock.restart();
}

void displayPattern(RenderWindow& window, Font& font, Sprite buttons[4], Sprite& pauseSprite, Sprite& resumeSprite, Sprite& mainMenuSprite, const vector<int>& pattern, int difficulty, int score, bool& acceptingInput, bool& patternDisplayed, Clock& roundClock)
{
    float patternSpeed = 0.50f;

    if(difficulty == 0)
    {
        patternSpeed = 0.75f;
    }
    else if(difficulty == 2)
    {
        patternSpeed = 0.15f;
    }

    for(int i = 0; i < 4; i++)
    {
        buttons[i].setScale(Vector2f(1.f, 1.f));
    }

    for(int buttonIndex : pattern)
    {
        Clock glowClock;
        while(glowClock.getElapsedTime() < seconds(patternSpeed))
        {
            while(const optional event = window.pollEvent())
            {
                if(event->is<Event::Closed>())
                {
                    window.close();
                    return;
                }
                if(pauseButton(window, pauseSprite, event, font, resumeSprite, mainMenuSprite, acceptingInput, patternDisplayed, roundClock))
                {
                    return;
                }
            }

            const float progress = glowClock.getElapsedTime().asSeconds() / seconds(patternSpeed).asSeconds();
            const uint8_t opacity = static_cast<uint8_t>(125.f * (1.f - progress));

            window.clear(BACKGROUND_COLOUR);
            drawGameName(window, font);
            drawScore(window, font, score);
            drawButtonGlow(window, buttons, buttonIndex, Color(255, 255, 255, opacity));
            for(int i = 0; i < 4; i++)
            {
                window.draw(buttons[i]);
            }
            pauseButton(window, pauseSprite, nullopt, font, resumeSprite, mainMenuSprite, acceptingInput, patternDisplayed, roundClock);
            window.display();
        }

        buttons[buttonIndex].setScale(Vector2f(1.f, 1.f));
        buttons[buttonIndex].setColor(Color(255, 255, 255));
        Clock pauseClock;
        while(pauseClock.getElapsedTime() < seconds(0.25f))
        {
            while(const optional event = window.pollEvent())
            {
                if(event->is<Event::Closed>())
                {
                    window.close();
                    return;
                }
                if(pauseButton(window, pauseSprite, event, font, resumeSprite, mainMenuSprite, acceptingInput, patternDisplayed, roundClock))
                {
                    return;
                }
            }
        }
    }
    acceptingInput = patternDisplayed;
}