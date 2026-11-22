#include <SFML/Graphics.hpp>
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
const string DIFFICULTIES[3] = {"Easy", "Medium", "Hard"};
const string BUTTONS[4] = {"buttonGreen.png", "buttonRed.png", "buttonYellow.png", "buttonBlue.png"};
const string PAUSE_BUTTON = "buttonPause.png";
const Color MAIN_TEXT_COLOUR(0, 255, 34);
const Color BUTTON_GLOW_COLOURS[4] = {Color(0, 255, 0),
                                      Color(255, 0, 0),
                                      Color(255, 255, 0),
                                      Color(0, 0, 255)};
const Time CLICK_FEEDBACK_DURATION = seconds(0.25f);

void createWindow(RenderWindow& window);
bool loadButtons(Sprite buttons[4], Texture textures[4], Texture& pauseTexture, Sprite& pauseSprite);
bool loadFont(Font& font);
void drawGameName(RenderWindow& window, Font& font);
void drawScore(RenderWindow& window, Font& font, int score);
void drawButtonGlow(RenderWindow& window, Sprite buttons[4], int buttonIndex, const Color& glowColour);
int detectMouseHover(RenderWindow& window, Sprite buttons[4]);
int detectMouseClick(RenderWindow& window, Sprite buttons[4], const optional<Event>& event);
bool pauseButton(RenderWindow& window, Sprite& pauseSprite, const optional<Event>& event,
                 Font& font, Sprite buttons[4], const vector<int>& pattern, int difficulty,
                 int score, bool& acceptingInput, bool& patternDisplayed);
void clickFeedback(RenderWindow& window, Sprite buttons[4], int clickedButton, Time elapsed);
vector<int> patternGenerate(int patternLength);
int checkDifficulty(const string& difficulty = DIFFICULTIES[1]);
void displayPattern(RenderWindow& window, Font& font, Sprite buttons[4], Sprite& pauseSprite, const vector<int>& pattern, int difficulty, int score, bool& acceptingInput, bool& patternDisplayed);

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    RenderWindow window;
    createWindow(window);
    Font font;
    if(!loadFont(font))
    {
        cout << "Error loading font! Place a font file named font.ttf next to the program." << endl;
        return 1;
    }
    Texture textures[4];
    Sprite buttons[4] = {Sprite(textures[0]),
                         Sprite(textures[1]),
                         Sprite(textures[2]),
                         Sprite(textures[3])};
    Texture pauseTexture;
    Sprite pauseSprite(pauseTexture);
    if(!loadButtons(buttons, textures, pauseTexture, pauseSprite))
    {
        return 1;
    }
    int feedbackButton = -1;
    Clock feedbackClock;
    int round = 1;
    int score = 0;
    int inputIndex = 0;
    vector<int> pattern = patternGenerate(round);
    Clock roundClock;
    bool patternDisplayed = false;
    bool acceptingInput = false;
    int difficulty = checkDifficulty(DIFFICULTIES[1]); //Nauman: Manual input for difficulty, implement with Talha's main menu.

    while(window.isOpen())
    {
        while(const optional event = window.pollEvent())
        {
            if(event->is<Event::Closed>())
            {
                window.close();
            }

            if(pauseButton(window, pauseSprite, event, font, buttons, pattern, difficulty, score, acceptingInput, patternDisplayed))
            {
                continue;
            }

            const int clickedButton = detectMouseClick(window, buttons, event);
            if(clickedButton != -1)
            {
                feedbackButton = clickedButton;
                feedbackClock.restart();
                cout << "Button clicked: " << clickedButton << endl; //Temporary output for debugging.

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
                        round++;
                        pattern.push_back(rand() % 4);
                        inputIndex = 0;
                        acceptingInput = false;
                        patternDisplayed = false;
                        roundClock.restart();
                        cout << "Correct pattern! Starting round " << round << endl; //Temporary output for debugging.
                    }
                }
                else
                {
                    round = 1;
                    score = 0;
                    pattern = patternGenerate(round);
                    inputIndex = 0;
                    acceptingInput = false;
                    patternDisplayed = false;
                    roundClock.restart();
                    cout << "Wrong button! Starting round " << round << endl; //Temporary output for debugging.
                }
            }
        }

        if(!patternDisplayed && roundClock.getElapsedTime() >= seconds(0.7f))
        {
            patternDisplayed = true;
            displayPattern(window, font, buttons, pauseSprite, pattern, difficulty, score, acceptingInput, patternDisplayed);
            acceptingInput = true;
        }

        detectMouseHover(window, buttons);
        window.clear(Color(0, 13, 56));
        drawGameName(window, font);
        drawScore(window, font, score);

        clickFeedback(window, buttons, feedbackButton, feedbackClock.getElapsedTime());
        for(int i = 0; i < 4; i++)
            window.draw(buttons[i]);
        pauseButton(window, pauseSprite, nullopt, font, buttons, pattern, difficulty, score, acceptingInput, patternDisplayed);
        window.display();
    }

    return 0;
}

void createWindow(RenderWindow& window)
{
    window.create(VideoMode(Vector2u(WINDOW_W, WINDOW_H)), "Simon Game");
    window.setFramerateLimit(60);
}

bool loadButtons(Sprite buttons[4], Texture textures[4], Texture& pauseTexture, Sprite& pauseSprite)
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
    pauseSprite.setScale(Vector2f(1.f, 1.f));
    pauseSprite.setColor(Color(255, 255, 255));
    return true;
}

bool loadFont(sf::Font &font)
{
    string paths[2] = {"font.ttf", "C:/Windows/Fonts/arial.ttf"};

    for(int i = 0; i < 2; i++)
    {
        if(font.openFromFile(paths[i]))
            return true;
    }

    return false;
}

void drawGameName(RenderWindow& window, Font& font)
{
    Text gameName(font, "S I M O N", 72);
    gameName.setFillColor(MAIN_TEXT_COLOUR);
    const FloatRect boundsGameName = gameName.getLocalBounds();
    gameName.setPosition(Vector2f(388.f - boundsGameName.position.x - boundsGameName.size.x / 2.f, 40.f - boundsGameName.position.y));
    window.draw(gameName);
}

void drawScore(RenderWindow& window, Font& font, int score)
{
    Text scoreText(font, "Score: " + to_string(score), 28);
    scoreText.setFillColor(Color(255, 255, 255));
    scoreText.setPosition(Vector2f(20.f, 20.f));
    window.draw(scoreText);
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
    const Vector2f buttonCenter(boundsButton.position.x + boundsButton.size.x / 2.f, boundsButton.position.y + boundsButton.size.y / 2.f);


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
        const Vector2f buttonCenter(boundsButton.position.x + boundsButton.size.x / 2.f, boundsButton.position.y + boundsButton.size.y / 2.f);
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

bool pauseButton(RenderWindow& window, Sprite& pauseSprite, const optional<Event>& event, Font& font, Sprite buttons[4], const vector<int>& pattern, int difficulty, int score, bool& acceptingInput, bool& patternDisplayed)
{
    static Clock feedbackClock;
    static bool paused = false;
    pauseSprite.setOrigin(Vector2f(32.f, 32.f));
    pauseSprite.setPosition(Vector2f(768.f, 32.f));
    pauseSprite.setScale(Vector2f(1.f, 1.f));

    if(event.has_value() && event->is<Event::MouseButtonPressed>())
    {
        const Vector2i mousePosition = Mouse::getPosition(window);
        const Vector2f mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
        const FloatRect bounds = pauseSprite.getGlobalBounds();
        const Vector2f center(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f);
        const float radius = min(bounds.size.x, bounds.size.y) * 0.35f;
        const float horizontalDistance = mousePositionF.x - center.x;
        const float verticalDistance = mousePositionF.y - center.y;

        if(horizontalDistance * horizontalDistance + verticalDistance * verticalDistance <= radius * radius)
        {
            feedbackClock.restart();
            pauseSprite.setScale(Vector2f(1.f, 1.f));
            pauseSprite.setColor(Color(145, 145, 145));

            if(!paused)
            {
                paused = true;
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
                            continue;
                        }
                    }

                    if(window.isOpen() && paused)
                    {
                        window.clear(Color(0, 13, 56));
                        drawGameName(window, font);
                        drawScore(window, font, score);
                        for(int i = 0; i < 4; i++) window.draw(buttons[i]);
                        pauseSprite.setColor(Color(145, 145, 145));
                        window.draw(pauseSprite);
                        window.display();
                    }
                }
            }
            return true;
        }
    }

    if(feedbackClock.getElapsedTime() >= CLICK_FEEDBACK_DURATION)
    {
        pauseSprite.setScale(Vector2f(1.f, 1.f));
        pauseSprite.setColor(Color(255, 255, 255));
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

vector<int> patternGenerate(int patternLength)
{
    vector<int> pattern;

    for(int i = 0; i < patternLength; i++)
    {
        pattern.push_back(rand() % 4);
    }

    return pattern;
}

int checkDifficulty(const string& difficulty)
{
    if(difficulty == DIFFICULTIES[0])
    {
        return 0;
    }
    if(difficulty == DIFFICULTIES[1])
    {
        return 1;
    }
    if(difficulty == DIFFICULTIES[2])
    {
        return 2;
    }

    return 1;
}

void displayPattern(RenderWindow& window, Font& font, Sprite buttons[4], Sprite& pauseSprite, const vector<int>& pattern, int difficulty, int score, bool& acceptingInput, bool& patternDisplayed)
{
    float patternSpeed = 0.50f;

    if(difficulty == 0)
    {
        patternSpeed = 0.75f;
    }
    else if(difficulty == 2)
    {
        patternSpeed = 0.25f;
    }

    for(int i = 0; i < 4; i++)
    {
        buttons[i].setScale(Vector2f(1.f, 1.f));
    }

    for(int buttonIndex : pattern)
    {
        if(buttonIndex < 0 || buttonIndex > 3)
        {
            continue;
        }

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
                if(pauseButton(window, pauseSprite, event, font, buttons, pattern, difficulty, score, acceptingInput, patternDisplayed))
                {
                    return;
                }
            }

            const float progress = glowClock.getElapsedTime().asSeconds() / seconds(patternSpeed).asSeconds();
            const uint8_t opacity = static_cast<uint8_t>(125.f * (1.f - progress));

            window.clear(Color(0, 13, 56));
            drawGameName(window, font);
            drawScore(window, font, score);
            drawButtonGlow(window, buttons, buttonIndex, Color(255, 255, 255, opacity));
            for(int i = 0; i < 4; i++)
            {
                window.draw(buttons[i]);
            }
            pauseButton(window, pauseSprite, nullopt, font, buttons, pattern, difficulty, score, acceptingInput, patternDisplayed);
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
                if(pauseButton(window, pauseSprite, event, font, buttons, pattern, difficulty, score, acceptingInput, patternDisplayed))
                {
                    return;
                }
            }
        }
    }
}