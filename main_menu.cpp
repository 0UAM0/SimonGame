#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;

int main()
{
    // Create window
    sf::RenderWindow window(sf::VideoMode({800, 750}), "SFML Project");
    window.setFramerateLimit(60);

    // Screens
    // 0 = Main Menu
    // 1 = Difficulty Menu
    // 2 = Game
    // 3 = Credits
    int screen = 0;

    // Difficulty
    // 1 = Easy, 2 = Medium, 3 = Hard
    int difficulty = 0;

    // Load font
    sf::Font font;
    if (!font.openFromFile("font.ttf"))
    {
        cout << "Font not found!" << endl;
        return -1;
    }

    // Titles
    sf::Text title(font, "MY 2D GAME", 50);
    title.setPosition({245, 100});

    sf::Text difficultyTitle(font, "SELECT DIFFICULTY", 40);
    difficultyTitle.setPosition({220, 120});

    // Main menu buttons
    sf::RectangleShape menuButtons[3];
    sf::Text menuText[3] = {
        sf::Text(font, "START GAME", 25),
        sf::Text(font, "CREDITS", 25),
        sf::Text(font, "EXIT", 25)
    };

    // Difficulty buttons
    sf::RectangleShape levelButtons[3];
    sf::Text levelText[3] = {
        sf::Text(font, "EASY", 25),
        sf::Text(font, "MEDIUM", 25),
        sf::Text(font, "HARD", 25)
    };

    // Set size, position and color of all buttons
    for (int i = 0; i < 3; i++)
    {
        menuButtons[i].setSize({250, 60});
        menuButtons[i].setPosition({275, 250 + i * 85.f});
        menuButtons[i].setFillColor(sf::Color(50, 100, 180));

        levelButtons[i].setSize({250, 60});
        levelButtons[i].setPosition({275, 250 + i * 85.f});
        levelButtons[i].setFillColor(sf::Color(50, 100, 180));
    }

    // Text positions (inside the buttons)
    menuText[0].setPosition({325, 263});
    menuText[1].setPosition({345, 348});
    menuText[2].setPosition({370, 433});

    levelText[0].setPosition({368, 263});
    levelText[1].setPosition({348, 348});
    levelText[2].setPosition({365, 433});

    // Game screen text
    sf::Text gameText(font, "GAME STARTED!", 45);
    gameText.setFillColor(sf::Color::Green);
    gameText.setPosition({230, 250});

    // Credits text
    sf::Text credits(font, "Developed by\n\nMuhammad Talha\nUmer Assad\nMuhammad Nauman", 25);
    credits.setPosition({300, 180});

    // Back message
    sf::Text backText(font, "Press ESC to go back", 20);
    backText.setPosition({310, 650});

    // Main loop
    while (window.isOpen())
    {
        // Get mouse position
        sf::Vector2f mouse(sf::Mouse::getPosition(window));

        // Handle events
        while (const auto event = window.pollEvent())
        {
            // Close window
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Mouse click
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                // Main menu
                if (screen == 0)
                {
                    if (menuButtons[0].getGlobalBounds().contains(mouse))
                        screen = 1;   // go to difficulty menu

                    if (menuButtons[1].getGlobalBounds().contains(mouse))
                        screen = 3;   // go to credits

                    if (menuButtons[2].getGlobalBounds().contains(mouse))
                        window.close();
                }

                // Difficulty menu
                else if (screen == 1)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        if (levelButtons[i].getGlobalBounds().contains(mouse))
                        {
                            difficulty = i + 1;
                            screen = 2;   // start game
                        }
                    }
                }

                // Credits
                else if (screen == 3)
                {
                    screen = 0;
                }
            }

            // ESC key goes back to main menu
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    screen = 0;
                }
            }
        }

        // Clear screen
        window.clear(sf::Color(25, 25, 45));

        // MAIN MENU
        if (screen == 0)
        {
            window.draw(title);

            for (int i = 0; i < 3; i++)
            {
                // Change color when mouse is on button
                if (menuButtons[i].getGlobalBounds().contains(mouse))
                    menuButtons[i].setFillColor(sf::Color(100, 150, 230));
                else
                    menuButtons[i].setFillColor(sf::Color(50, 100, 180));

                window.draw(menuButtons[i]);
                window.draw(menuText[i]);
            }
        }

        // DIFFICULTY MENU
        else if (screen == 1)
        {
            window.draw(difficultyTitle);

            for (int i = 0; i < 3; i++)
            {
                if (levelButtons[i].getGlobalBounds().contains(mouse))
                    levelButtons[i].setFillColor(sf::Color(100, 150, 230));
                else
                    levelButtons[i].setFillColor(sf::Color(50, 100, 180));

                window.draw(levelButtons[i]);
                window.draw(levelText[i]);
            }

            window.draw(backText);
        }

        // GAME
        else if (screen == 2)
        {
            // Gameplay code goes here
            // use the difficulty variable (1, 2 or 3)
            window.draw(gameText);
            window.draw(backText);
        }

        // CREDITS
        else if (screen == 3)
        {
            window.draw(credits);
            window.draw(backText);
        }

        // Show everything
        window.display();
    }

    return 0;
}