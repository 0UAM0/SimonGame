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

void createWindow(RenderWindow& window);
void loadButtons(Sprite buttons[4], Texture textures[4]);

int main()
{
    RenderWindow window;
    createWindow(window);
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

        window.clear(Color(0, 13, 56));
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

    buttons[0].setPosition(Vector2f(100.f, 70.f)); // Green button
    buttons[1].setPosition(Vector2f(425.f, 70.f)); // Red button
    buttons[2].setPosition(Vector2f(100.f, 400.f)); // Yellow button
    buttons[3].setPosition(Vector2f(425.f, 400.f)); // Blue button
}