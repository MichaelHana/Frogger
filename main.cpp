#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <math.h>

using namespace sf;

RenderWindow window(VideoMode(1000, 1000), "Frogger");
RectangleShape player;
Color grey(59, 58, 55);
Color lightblue(75, 181, 227);
Color green(75, 227, 98);
Color brown(84, 53, 12);

int unit = 10;
int rowLength = window.getSize().x;
int rowWidth = window.getSize().y / unit;

std::vector<RectangleShape> rows;
std::vector<RectangleShape> lilypads;
std::vector<RectangleShape> cars;
std::vector<RectangleShape> logs;
std::vector<RectangleShape> validLilypads;
std::vector<int> currentLogDirection; // Left == 0, Right == 1
std::vector<float> logSpeed;
std::vector<int> currentCarDirection; // Left == 0, Right == 1
std::vector<float> carSpeed;

void newRow()
{
    RectangleShape row;
    row.setSize(Vector2f(rowLength, rowWidth));
    row.setOutlineColor(Color::Black);
    row.setOutlineThickness(3);

    if (rows.size() == 0)
    {
        row.setPosition(0, window.getSize().y - row.getSize().y);
    }
    else
    {
        row.setPosition(0, rows.at(rows.size() - 1).getPosition().y - 100);
    }

    int i = rand() % 4;

    switch(i)
    {
    case 0://lily pads

        row.setFillColor(lightblue);
        {
            int j = rand() % 5 + 1;
            for (int y = 0; y <= j; y++)
            {
                RectangleShape lilypad;
                lilypad.setSize(Vector2f(60, 60));
                lilypad.setFillColor(green);
                lilypad.setOutlineColor(Color::Black);
                lilypad.setOutlineThickness(3);

                if (lilypads.size() > 0)
                {
                    if (lilypads.at(lilypads.size() - 1).getPosition().y == row.getPosition().y + 120)
                    {
                        std::vector<RectangleShape> temp;
                        std::vector<RectangleShape> validLilypadsLeft = validLilypads;
                        std::vector<RectangleShape> validLilypadsRight = validLilypads;

                        //Get lily pads that are next to each other and one row under the current row
                        for (int z = 0; z < lilypads.size(); z++)
                        {
                            if (lilypads.at(z).getPosition().y == row.getPosition().y + 120)
                            {
                                temp.push_back(lilypads.at(z));
                            }
                        }

                        //Get all valid lily pads to the right of the original
                        for (int a = 0; a < validLilypadsRight.size(); a++)
                        {
                            for (int b = 0; b < temp.size(); b++)
                            {
                                if (validLilypadsRight.at(a).getPosition().x == temp.at(b).getPosition().x + 100)
                                {
                                    validLilypadsRight.push_back(temp.at(b));
                                }
                            }
                        }

                        //Get all valid lily pads to the left of the original
                        for (int a = 0; a < validLilypadsLeft.size(); a++)
                        {
                            for (int b = 0; b < temp.size(); b++)
                            {
                                if (validLilypadsLeft.at(a).getPosition().x == temp.at(b).getPosition().x - 100)
                                {
                                    validLilypadsLeft.push_back(temp.at(b));
                                }
                            }
                        }

                        //Merging left and right sides
                        validLilypads.clear();
                        validLilypads.reserve(validLilypadsLeft.size() + validLilypadsRight.size());
                        validLilypads.insert(validLilypads.end(), validLilypadsLeft.begin(), validLilypadsLeft.end());
                        validLilypads.insert(validLilypads.end(), validLilypadsRight.begin(), validLilypadsRight.end());

                        //Setting the position of the new lily pad to have the same x coordinate as one of the lily pads on the previous row
                        if (validLilypads.size() > 1)
                        {
                            lilypad.setPosition(validLilypads.at(rand() % validLilypads.size()).getPosition().x, row.getPosition().y + 20);
                        }
                        else
                        {
                            lilypad.setPosition(validLilypads.at(0).getPosition().x, row.getPosition().y + 20);
                        }
                        validLilypads.clear();
                        validLilypads.push_back(lilypad);
                    }
                    else
                    {
                        lilypad.setPosition(100 * ceil(rand() % unit) + 20, row.getPosition().y + 20);
                    }
                }

                else
                {
                    lilypad.setPosition(100 * ceil(rand() % unit) + 20, row.getPosition().y + 20);
                    validLilypads.push_back(lilypad);
                }
                lilypads.push_back(lilypad);
            }
        }
        break;

    case 1://grass
        row.setFillColor(green);
        break;

    case 2://logs

        row.setFillColor(lightblue);

        {
            for (int y = 0; y < 7; y++)
            {
                RectangleShape log;
                log.setFillColor(brown);
                log.setOutlineColor(Color::Black);
                log.setOutlineThickness(3);
                log.setSize(Vector2f(((rand() % 5 + 1) * 100) - 40, 60));
                log.setPosition((y * 500) + 20, row.getPosition().y + 20);
                logs.push_back(log);
            }

            currentLogDirection.push_back(rand() % 2);


            float k = rand() % 4 + 1;
            logSpeed.push_back(k / 10);
        }

        break;

    case 3://cars

        row.setFillColor(grey);

        {
            for (int y = 0; y < 7; y++)
            {
                RectangleShape car;
                car.setFillColor(Color::Red);
                car.setOutlineColor(Color::Black);
                car.setOutlineThickness(3);
                car.setSize(Vector2f(((rand() % 5 + 1) * 100) - 40, 60));
                car.setPosition((y * 500) + 20, row.getPosition().y + 20);
                cars.push_back(car);
            }

            currentCarDirection.push_back(rand() % 2);


            float k = rand() % 4 + 2;
            carSpeed.push_back(k / 10);
        }

        break;
    }

    rows.push_back(row);
}

void moveMap(float offset)
{
    //Drawing the rows
    for (int x = 0; x < rows.size(); x++)
    {
        RectangleShape row = rows.at(x);
        if (row.getPosition().x <= window.getSize().x || row.getPosition().y <= window.getSize().y)
        {
            rows.at(x).move(0, offset);
        }
    }
    //Drawing Lily Pads
    for (int x = 0; x < lilypads.size(); x++)
    {
        lilypads.at(x).move(0, offset);
    }
    //Drawing Logs
    for (int x = 0; x < logs.size(); x++)
    {
        logs.at(x).move(0, offset);
    }
    //Drawing Cars
    for (int x = 0; x < cars.size(); x++)
    {
        cars.at(x).move(0, offset);
    }
}

bool logCheck()
{
    for (int x = 0; x < logs.size(); x++)
    {
        if (logs.at(x).getGlobalBounds().contains(player.getPosition()) &&
            not(logs.at(x).getGlobalBounds().contains(player.getPosition().x + player.getSize().x, player.getPosition().y)))
        {
            player.setPosition(logs.at(x).getPosition().x + logs.at(x).getSize().x - player.getSize().x, player.getPosition().y);
            return true;
        }
        else if (logs.at(x).getGlobalBounds().contains(player.getSize().x + player.getPosition().x, player.getPosition().y) &&
                 not(logs.at(x).getGlobalBounds().contains(player.getPosition())))
        {
            player.setPosition(logs.at(x).getPosition());
            return true;
        }
        else if (logs.at(x).getGlobalBounds().intersects(player.getGlobalBounds()))
        {
            return true;
        }
    }

    return false;
}

void adjustX()
{
    float currentX = player.getPosition().x;
    currentX = floor(currentX);
    int newX = round(currentX/100);
    newX = (newX * 100) + 20;
    player.setPosition(newX, player.getPosition().y);
}

int main()
{
    bool isDead = false;
    bool gameOverScreen = false;
    bool levelScreen = true;
    int score = 1;

    RectangleShape gameover;
    gameover.setFillColor(grey);
    gameover.setSize(Vector2f(window.getSize()));
    gameover.setPosition(0, 0);
    window.draw(gameover);

    Text gameoverText;
    Font arial;
    arial.loadFromFile("arial.ttf");
    gameoverText.setFont(arial);
    gameoverText.setCharacterSize(50);
    gameoverText.setPosition(400, 400);

    //Adding first 3 rows as grass
    for (int x = 0; x < 3; x++)
    {
        RectangleShape row;
        row.setSize(Vector2f(rowLength, rowWidth));
        row.setPosition(0, window.getSize().y / unit * (unit - (x + 1)));
        row.setFillColor(green);
        row.setOutlineColor(Color::Black);
        row.setOutlineThickness(3);
        rows.push_back(row);
    }

    //adding the next 7 roads to complete the screen
    for (int x = 0; x < 7; x++)
    {
       newRow();
    }

    //Initializing player
    Color darkGreen(22, 87, 28);
    Clock moveClock;

    player.setSize(Vector2f(60, 60));
    player.setFillColor(darkGreen);
    player.setPosition(window.getSize().x/2 + 20, rows.at(1).getPosition().y + 20);

    //Game Loop
    while (window.isOpen())
    {
        window.clear();
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if (levelScreen == true)
        {
            //Keyboard Input
            if (moveClock.getElapsedTime() >= seconds(0.15))
            {
                if (Keyboard::isKeyPressed(Keyboard::Up))
                {
                    if (player.getPosition().y >= rows.at(2).getPosition().y)
                    {
                        player.move(0, -100);
                    }
                    else
                    {
                        moveMap(100);
                        if (player.getPosition().y < rows.at(rows.size() - 7).getPosition().y)
                        {
                            newRow();
                            score++;
                        }
                    }

                    moveClock.restart();

                    //Jumping on log
                    bool onLog = logCheck();
                    //Adjusting x position to be a point on the graph of 100a + 20 = x
                    if (onLog == false)
                    {
                        adjustX();
                    }

                }
                else if (Keyboard::isKeyPressed(Keyboard::Down))
                {
                    if (player.getPosition().y >= rows.at(3).getPosition().y && player.getPosition().y <= rows.at(0).getPosition().y)
                    {
                        player.move(0, 100);
                        moveClock.restart();
                    }
                    else if (player.getPosition().y < rows.at(0).getPosition().y)
                    {
                        moveMap(-100);
                        moveClock.restart();
                    }

                    //Jumping on log
                    bool onLog = logCheck();

                    //Adjusting x position to be a point on the graph of 100a + 20 = x
                    if (onLog == false)
                    {
                        adjustX();
                    }
                }
                else if (Keyboard::isKeyPressed(Keyboard::Left) && player.getPosition().x >= 100)
                {
                    player.move(-100, 0);
                    moveClock.restart();
                    logCheck();
                }
                else if (Keyboard::isKeyPressed(Keyboard::Right) && player.getPosition().x <= window.getSize().x - 100)
                {
                    player.move(100, 0);
                    moveClock.restart();
                    logCheck();
                }
            }


            //Moving Logs
            int previousRowY;
            int cdTracker = 0;
            float diff;
            for (int x = 0; x < logs.size(); x++)
            {
                diff = player.getPosition().x - logs.at(x).getPosition().x;
                if (x != 0)
                {
                    if (logs.at(x).getPosition().y != logs.at(x-1).getPosition().y)
                    {
                        cdTracker++;
                    }
                }

                if (currentLogDirection.at(cdTracker) == 0)
                {
                    logs.at(x).move(-logSpeed.at(cdTracker), 0);
                    if (logs.at(x).getPosition().x + logs.at(x).getSize().x <= 0)
                    {
                        logs.at(x).setPosition(window.getSize().x + 3500 - logs.at(x).getSize().x, logs.at(x).getPosition().y);
                    }

                }
                else
                {
                    logs.at(x).move(logSpeed.at(cdTracker), 0);
                    if (logs.at(x).getPosition().x >= 3500)
                    {
                        logs.at(x).setPosition(-500, logs.at(x).getPosition().y);
                    }
                }

                //Moving player on log
                if (logs.at(x).getGlobalBounds().intersects(player.getGlobalBounds()))
                {
                    player.setPosition(logs.at(x).getPosition().x + diff, player.getPosition().y);
                    if (player.getPosition().x < 0)
                    {
                        //Temp
                        exit(1);
                    }
                }
            }

            //Moving cars
            cdTracker = 0;
            for (int x = 0; x < cars.size(); x++)
            {
                if (x != 0)
                {
                    if (cars.at(x).getPosition().y != cars.at(x-1).getPosition().y)
                    {
                        cdTracker++;
                    }
                }

                if (currentCarDirection.at(cdTracker) == 0)
                {
                    cars.at(x).move(-carSpeed.at(cdTracker), 0);
                    if (cars.at(x).getPosition().x + cars.at(x).getSize().x <= 0)
                    {
                        cars.at(x).setPosition(window.getSize().x + 3500 - cars.at(x).getSize().x, cars.at(x).getPosition().y);
                    }
                }
                else
                {
                    cars.at(x).move(carSpeed.at(cdTracker), 0);
                    if (cars.at(x).getPosition().x >= 3500)
                    {
                        cars.at(x).setPosition(-500, cars.at(x).getPosition().y);
                    }
                }
            }

            //Drawing the rows
            for (int x = 0; x < rows.size(); x++)
            {
                RectangleShape row = rows.at(x);
                if (row.getPosition().x <= window.getSize().x || row.getPosition().y <= window.getSize().y)
                {
                    window.draw(row);
                }
                if (player.getGlobalBounds().intersects(rows.at(x).getGlobalBounds()) &&
                    rows.at(x).getFillColor() == lightblue)
                {
                    isDead = true;
                }
            }
            //Drawing Lily Pads
            for (int x = 0; x < lilypads.size(); x++)
            {
                window.draw(lilypads.at(x));
                if (player.getGlobalBounds().intersects(lilypads.at(x).getGlobalBounds()))
                {
                    isDead = false;
                }
            }
            //Drawing Logs
            for (int x = 0; x < logs.size(); x++)
            {
                window.draw(logs.at(x));
                if (player.getGlobalBounds().intersects(logs.at(x).getGlobalBounds()))
                {
                    isDead = false;
                }
            }
            //Drawing Cars
            for (int x = 0; x < cars.size(); x++)
            {
                window.draw(cars.at(x));
                if (player.getGlobalBounds().intersects(cars.at(x).getGlobalBounds()))
                {
                    isDead = true;
                }
            }

            //Check for death
            if (isDead == true)
            {
                gameOverScreen = true;
                levelScreen = false;
                std::string s = "Score: " + std::to_string(score);
                gameoverText.setString(s);
                moveClock.restart();
            }

            window.draw(player);
        }

        else if (gameOverScreen == true)
        {
            window.draw(gameover);
            window.draw(gameoverText);

            if (moveClock.getElapsedTime() >= seconds(0.4) && Keyboard::isKeyPressed(Keyboard::Up))
            {
                cars.clear();
                lilypads.clear();
                carSpeed.clear();
                logSpeed.clear();
                currentCarDirection.clear();
                currentLogDirection.clear();
                rows.clear();
                logs.clear();
                validLilypads.clear();

                main();
            }
        }
        window.display();
    }

    return 0;
}
