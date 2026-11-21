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

const int WINDOW_W = 1280;
const int WINDOW_H = 720;
const string DIFFICULTIES[3] = {"Easy", "Medium", "Hard"};
const string BUTTONS[4] = {"buttonGreen.png", "buttonRed.png", "buttonYellow.png", "buttonBlue.png"};
const Color MAIN_TEXT_COLOUR(0, 255, 34);
const Color BUTTON_GLOW_COLOURS[4] = {Color(0, 255, 0),
                                      Color(255, 0, 0),
                                      Color(255, 255, 0),
                                      Color(0, 128, 255)};
const Time CLICK_FEEDBACK_DURATION = seconds(0.25f);

void createWindow(RenderWindow& window);
void loadButtons(Sprite buttons[4], Texture textures[4]);
bool loadFont(Font& font);
void drawGameName(RenderWindow& window, Font& font);
void drawButtonGlow(RenderWindow& window, Sprite buttons[4], int buttonIndex, const Color& glowColour);
int detectMouseHover(RenderWindow& window, Sprite buttons[4]);
int detectMouseClick(RenderWindow& window, Sprite buttons[4], const optional<Event>& event);
void clickFeedback(RenderWindow& window, Sprite buttons[4], int clickedButton, Time elapsed);
vector<int> patternGenerate(int n);
void displayPattern(RenderWindow& window, Font& font, Sprite buttons[4], const vector<int>& pattern);

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
    loadButtons(buttons, textures);
    int feedbackButton = -1;
    Clock feedbackClock;
    int round = 1;
    int inputIndex = 0;
    vector<int> pattern = patternGenerate(round);
    Clock roundClock;
    bool patternDisplayed = false;
    bool acceptingInput = false;

    while(window.isOpen())
    {
        while(const optional event = window.pollEvent())
        {
            if(event->is<Event::Closed>())
            {
                window.close();
            }

            const int clickedButton = detectMouseClick(window, buttons, event);
            if(clickedButton != -1 && acceptingInput)
            {
                feedbackButton = clickedButton;
                feedbackClock.restart();
                cout << "Button clicked: " << clickedButton << endl;

                if(clickedButton == pattern[inputIndex])
                {
                    inputIndex++;
                    if(inputIndex == static_cast<int>(pattern.size()))
                    {
                        round++;
                        pattern.push_back(rand() % 4);
                        inputIndex = 0;
                        acceptingInput = false;
                        patternDisplayed = false;
                        roundClock.restart();
                        cout << "Correct pattern! Starting round " << round << endl;
                    }
                }
                else
                {
                    round = 1;
                    pattern = patternGenerate(round);
                    inputIndex = 0;
                    acceptingInput = false;
                    patternDisplayed = false;
                    roundClock.restart();
                    cout << "Wrong button! Starting round " << round << endl;
                }
            }
        }

        if(!patternDisplayed && roundClock.getElapsedTime() >= seconds(0.7f))
        {
            patternDisplayed = true;
            displayPattern(window, font, buttons, pattern);
            acceptingInput = true;
        }

        detectMouseHover(window, buttons);
        window.clear(Color(0, 13, 56));
        drawGameName(window, font);
        clickFeedback(window, buttons, feedbackButton, feedbackClock.getElapsedTime());
        for(const Sprite& button : buttons)
        {
            window.draw(button);
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

void loadButtons(Sprite buttons[4], Texture textures[4])
{
    for(int i = 0; i < 4; ++i)
    {
        if(!textures[i].loadFromFile(BUTTONS[i]))
        {
            cerr << "Error loading texture: " << BUTTONS[i] << endl;
            exit(EXIT_FAILURE);
        }
        buttons[i].setTexture(textures[i], true);
        buttons[i].setOrigin(Vector2f(128.f, 128.f));
    }

    buttons[0].setPosition(Vector2f(228.f, 248.f)); // Green button
    buttons[1].setPosition(Vector2f(548.f, 248.f)); // Red button
    buttons[2].setPosition(Vector2f(228.f, 548.f)); // Yellow button
    buttons[3].setPosition(Vector2f(548.f, 548.f)); // Blue button
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
    const FloatRect bounds = gameName.getLocalBounds();
    gameName.setPosition(Vector2f(388.f - bounds.position.x - bounds.size.x / 2.f, 40.f - bounds.position.y));
    window.draw(gameName);
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

    const FloatRect bounds = buttons[buttonIndex].getGlobalBounds();
    const float radius = max(bounds.size.x, bounds.size.y) * 0.52f;
    const Vector2f center(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f);

    CircleShape glow(radius, 64);
    glow.setOrigin(Vector2f(radius, radius));
    glow.setPosition(center);
    glow.setFillColor(glowColour);
    window.draw(glow);
}

int detectMouseHover(RenderWindow& window, Sprite buttons[4])
{
    const Vector2i mousePosition = Mouse::getPosition(window);
    const Vector2f mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));

    for(int i = 0; i < 4; i++)
    {
        const FloatRect buttonBounds = buttons[i].getGlobalBounds();
        const Vector2f buttonCenter(buttonBounds.position.x + buttonBounds.size.x / 2.f, buttonBounds.position.y + buttonBounds.size.y / 2.f);
        const float buttonRadius = min(buttonBounds.size.x, buttonBounds.size.y) * 0.35f;
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

void clickFeedback(RenderWindow& window, Sprite buttons[4], int clickedButton, Time elapsed)
{
    if(clickedButton < 0 || clickedButton > 3)
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
    const uint8_t alpha = static_cast<uint8_t>(125.f * (1.f - progress));
    drawButtonGlow(window, buttons, clickedButton, Color(BUTTON_GLOW_COLOURS[clickedButton].r, BUTTON_GLOW_COLOURS[clickedButton].g, BUTTON_GLOW_COLOURS[clickedButton].b, alpha));
}

vector<int> patternGenerate(int n)
{
    vector<int> pattern;

    for(int i = 0; i < n; i++)
    {
        pattern.push_back(rand() % 4);
    }

    return pattern;
}

void displayPattern(RenderWindow& window, Font& font, Sprite buttons[4], const vector<int>& pattern)
{
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
        while(glowClock.getElapsedTime() < seconds(0.50f))
        {
            while(const optional event = window.pollEvent())
            {
                if(event->is<Event::Closed>())
                {
                    window.close();
                    return;
                }
            }

            const float progress = glowClock.getElapsedTime().asSeconds() / seconds(0.50f).asSeconds();
            const uint8_t alpha = static_cast<uint8_t>(125.f * (1.f - progress));

            window.clear(Color(0, 13, 56));
            drawGameName(window, font);
            drawButtonGlow(window, buttons, buttonIndex, Color(255, 255, 255, alpha));
            for(int i = 0; i < 4; i++)
            {
                window.draw(buttons[i]);
            }
            window.display();
        }

        buttons[buttonIndex].setScale(Vector2f(1.f, 1.f));
        buttons[buttonIndex].setColor(Color(255, 255, 255));
        sleep(seconds(0.25f));
    }
}