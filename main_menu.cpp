#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
using namespace std;   
int main()
{
    // Create window
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "SFML Project"
    );

    window.setFramerateLimit(60);

    // Game states
    int gameState = 0;
    // 0 = Main Menu
    // 1 = Game Screen
    // 2 = Credits

    // Load font
    sf::Font font;

    if (!font.openFromFile("times.ttf"))
    {
        cout << "Font not found!" << endl;
        return -1;
    }

    // Title
    sf::Text title(font, "MY 2D GAME", 50);
    title.setFillColor(sf::Color::White);
    title.setPosition({245, 100});

    // Button names
    string names[3] = {
        "START GAME",
        "CREDITS",
        "EXIT"
    };

    // Create buttons
    sf::RectangleShape buttons[3];
    sf::Text buttonText[3] = {
        sf::Text(font, "START GAME", 25),
        sf::Text(font, "CREDITS", 25),
        sf::Text(font, "EXIT", 25)
    };

    for (int i = 0; i < 3; i++)
{
    buttons[i].setSize(sf::Vector2f(250.f, 60.f));

    buttons[i].setPosition(sf::Vector2f(
        275.f, 250.f + i * 85.f
    ));

    buttons[i].setFillColor(sf::Color(50, 100, 180));

    buttonText[i].setFillColor(sf::Color::White);

    buttonText[i].setPosition(sf::Vector2f(
        325.f, 263.f + i * 85.f
    ));
}
    // Main game loop
    while (window.isOpen())
    {
        // Handle events
        while (const auto event = window.pollEvent())
        {
            // Close window
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Mouse click
            if (const auto* mouseClick =
                event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseClick->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f mouse(
                        static_cast<float>(mouseClick->position.x),
                        static_cast<float>(mouseClick->position.y)
                    );

                    if (gameState == 0)
                    {
                        for (int i = 0; i < 3; i++)
                        {
                            if (buttons[i].getGlobalBounds().contains(mouse))
                            {
                                if (i == 0)
                                    gameState = 1;

                                else if (i == 1)
                                    gameState = 2;

                                else if (i == 2)
                                    window.close();
                            }
                        }
                    }
                    else if (gameState == 2)
                    {
                        gameState = 0;
                    }
                }
            }

            // Keyboard input
            if (const auto* keyPress =
                event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPress->code == sf::Keyboard::Key::Escape)
                {
                    gameState = 0;
                }
            }
        }

        // Clear screen
        window.clear(sf::Color(25, 25, 45));

        // MAIN MENU
        if (gameState == 0)
        {
            window.draw(title);

            sf::Vector2i mouse = sf::Mouse::getPosition(window);

            for (int i = 0; i < 3; i++)
            {
                if (buttons[i].getGlobalBounds().contains(
                sf::Vector2f(
            static_cast<float>(mouse.x),
            static_cast<float>(mouse.y)
            )))
                {
                    buttons[i].setFillColor(
                        sf::Color(100, 150, 230)
                    );
                }
                else
                {
                    buttons[i].setFillColor(
                        sf::Color(50, 100, 180)
                    );
                }

                window.draw(buttons[i]);
                window.draw(buttonText[i]);
            }
        }

        // GAME SCREEN
        else if (gameState == 1)
        {
            sf::Text gameText(font, "GAME STARTED!", 45);
            gameText.setFillColor(sf::Color::Green);
            gameText.setPosition({230, 250});

            sf::Text instruction(
                font,
                "Press ESC to return to menu",
                20
            );

            instruction.setFillColor(sf::Color::White);
            instruction.setPosition({250, 330});

            window.draw(gameText);
            window.draw(instruction);
        }

        // CREDITS SCREEN
        else if (gameState == 2)
        {
            sf::Text credits(
                font,
               "Developed by\n\n"
                "Muhammad Talha\n"
                "Umer Assad\n"
                "Muhammad Nauman",
                25
            );

            credits.setFillColor(sf::Color::White);
            credits.setPosition({190, 180});

            window.draw(credits);
        }

        // Display everything
        window.display();
    }

    return 0;
}
