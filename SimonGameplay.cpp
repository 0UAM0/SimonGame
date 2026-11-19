#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
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

void createWindow(RenderWindow& window);
void loadButtons(Sprite buttons[4], Texture textures[4]);
bool loadFont(Font& font);
void drawGameName(RenderWindow& window, Font& font);
int detectMouseHover(RenderWindow& window, Sprite buttons[4]);

int main()
{
    RenderWindow window;
    createWindow(window);
    Font font;
    if (!loadFont(font))
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

    while (window.isOpen())
    {
        while (const optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }
        }

        detectMouseHover(window, buttons);
        window.clear(Color(0, 13, 56));
        drawGameName(window, font);
        for (const Sprite& button : buttons)
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
    for (int i = 0; i < 4; ++i)
    {
        if (!textures[i].loadFromFile(BUTTONS[i]))
        {
            cerr << "Error loading texture: " << BUTTONS[i] << endl;
            exit(EXIT_FAILURE);
        }
        buttons[i].setTexture(textures[i], true);
    }

    buttons[0].setPosition(Vector2f(100.f, 120.f)); // Green button
    buttons[1].setPosition(Vector2f(420.f, 120.f)); // Red button
    buttons[2].setPosition(Vector2f(100.f, 420.f)); // Yellow button
    buttons[3].setPosition(Vector2f(420.f, 420.f)); // Blue button
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
    Text gameName(font, "SIMON", 72);
    gameName.setFillColor(MAIN_TEXT_COLOUR);
    const FloatRect bounds = gameName.getLocalBounds();
    const float buttonGridCenterX = (100.f + 420.f + 256.f) / 2.f;
    gameName.setPosition(Vector2f(buttonGridCenterX - bounds.position.x - bounds.size.x / 2.f, 40.f - bounds.position.y));
    window.draw(gameName);
}

int detectMouseHover(RenderWindow& window, Sprite buttons[4])
{
    const Vector2i mousePosition = Mouse::getPosition(window);
    const Vector2f mousePositionF(
        static_cast<float>(mousePosition.x),
        static_cast<float>(mousePosition.y));

    for (int i = 0; i < 4; i++)
    {
        if (buttons[i].getGlobalBounds().contains(mousePositionF))
        {
            return i;
        }
    }

    return -1;
}