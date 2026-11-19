#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstdint>
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
}

void createWindow(sf::RenderWindow& window)
{
    window.create(sf::VideoMode(WINDOW_W, WINDOW_H), "Simon Game");
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

    buttons[0].setPosition(100, 100); // Green button
    buttons[1].setPosition(500, 100); // Red button
    buttons[2].setPosition(100, 400); // Yellow button
    buttons[3].setPosition(500, 400); // Blue button
}