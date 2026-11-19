#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <optional>
using namespace std;

const int WINDOW_W = 800;
const int WINDOW_H = 600;
const string DIFFICULTIES[3] = {"Easy", "Medium", "Hard"};
const string BUTTONS[4] = {"buttonGreen.png", "buttonRed.png", "buttonYellow.png", "buttonBlue.png"};

void createWindow(sf::RenderWindow& window);
void loadButtons(sf::Sprite buttons[4], sf::Texture textures[4]);

int main()
{
    sf::RenderWindow window;
    createWindow(window);
    sf::Texture textures[4];
    sf::Sprite buttons[4] = {sf::Sprite(textures[0]),
                             sf::Sprite(textures[1]),
                             sf::Sprite(textures[2]),
                             sf::Sprite(textures[3])};
    loadButtons(buttons, textures);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear(sf::Color::White);
        for (const sf::Sprite& button : buttons)
        {
            window.draw(button);
        }
        window.display();
    }

    return 0;
}

void createWindow(sf::RenderWindow& window)
{
    window.create(sf::VideoMode(sf::Vector2u(WINDOW_W, WINDOW_H)), "Simon Game");
    window.setFramerateLimit(60);
}

void loadButtons(sf::Sprite buttons[4], sf::Texture textures[4])
{
    for (int i = 0; i < 4; ++i)
    {
        if (!textures[i].loadFromFile(BUTTONS[i]))
        {
            cerr << "Error loading texture: " << BUTTONS[i] << endl;
            exit(EXIT_FAILURE);
        }
        buttons[i].setTexture(textures[i]);
    }

    buttons[0].setPosition(sf::Vector2f(100.f, 100.f)); // Green button
    buttons[1].setPosition(sf::Vector2f(500.f, 100.f)); // Red button
    buttons[2].setPosition(sf::Vector2f(100.f, 400.f)); // Yellow button
    buttons[3].setPosition(sf::Vector2f(500.f, 400.f)); // Blue button
}