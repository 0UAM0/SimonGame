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
void clickFeedback(RenderWindow& window, Sprite buttons[4], int clickedButton, Time elapsed);
int detectMouseHover(RenderWindow& window, Sprite buttons[4]);
int detectMouseClick(RenderWindow& window, Sprite buttons[4], const optional<Event>& event);
vector<int> patternGenerate(int n);

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

    while(window.isOpen())
    {
        while(const optional event = window.pollEvent())
        {
            if(event->is<Event::Closed>())
            {
                window.close();
            }

            const int clickedButton = detectMouseClick(window, buttons, event);
            if(clickedButton != -1)
            {
                feedbackButton = clickedButton;
                feedbackClock.restart();
                cout << "Button clicked: " << clickedButton << endl;
            }
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
    string paths[5] = {"font.ttf", "arial.ttf", "C:/Windows/Fonts/arial.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"};

    for(int i = 0; i < 5; i++)
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
    const float buttonGridCenterX = (100.f + 420.f + 256.f) / 2.f;
    gameName.setPosition(Vector2f(buttonGridCenterX - bounds.position.x - bounds.size.x / 2.f, 40.f - bounds.position.y));
    window.draw(gameName);
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
        return;
    }

    for(int i = 0; i < 4; ++i)
    {
        buttons[i].setScale(Vector2f(1.f, 1.f));
    }

    if(elapsed.asSeconds() < CLICK_FEEDBACK_DURATION.asSeconds() / 2.f)
    {
        buttons[clickedButton].setScale(Vector2f(0.85f, 0.85f));
    }

    const FloatRect buttonBounds = buttons[clickedButton].getGlobalBounds();
    const Vector2f buttonCenter(buttonBounds.position.x + buttonBounds.size.x / 2.f, buttonBounds.position.y + buttonBounds.size.y / 2.f);
    const float progress = elapsed.asSeconds() / CLICK_FEEDBACK_DURATION.asSeconds();
    const uint8_t alpha = static_cast<uint8_t>(125.f * (1.f - progress));
    const float glowRadius = max(buttonBounds.size.x, buttonBounds.size.y) * 0.52f;

    CircleShape glow(glowRadius, 64);
    glow.setOrigin(Vector2f(glowRadius, glowRadius));
    glow.setPosition(buttonCenter);
    glow.setFillColor(Color(BUTTON_GLOW_COLOURS[clickedButton].r, BUTTON_GLOW_COLOURS[clickedButton].g, BUTTON_GLOW_COLOURS[clickedButton].b, alpha));
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

vector<int> patternGenerate(int n)
{
    vector<int> pattern;

    for(int i = 0; i < n; i++)
    {
        pattern.push_back(rand() % 4);
    }

    return pattern;
}