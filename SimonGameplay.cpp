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
void loadbuttons(sf::Texture textures[4]);

int main()
{
    sf::RenderWindow window;
}

void createWindow(sf::RenderWindow& window)
{
    window.create(sf::VideoMode(WINDOW_W, WINDOW_H), "Simon Game");
    window.setFramerateLimit(60);
}

void loadbuttons(sf::Texture textures[4])
{
    for (int i = 0; i < 4; ++i)
    {
        if (!textures[i].loadFromFile(BUTTONS[i]))
        {
            cerr << "Error loading texture: " << BUTTONS[i] << endl;
            exit(EXIT_FAILURE);
        }
    }
}