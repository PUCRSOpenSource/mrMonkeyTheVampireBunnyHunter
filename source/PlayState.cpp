//Nome: Matthias Nunes

#include <iostream>
#include <cmath>
#include "Game.h"
#include "PlayState.h"
#include "InputManager.h"

PlayState PlayState::m_PlayState;

using namespace std;

void PlayState::init()
{
    player.loadXML("data/img/monkey.xml");
    player.setPosition(50,100);
    player.setScale(0.3, 0.3);
    player.loadAnimation("data/img/monkeyanim.xml");
    player.setAnimRate(9);
    player.setAnimation("idle");
    player.play();

    enemy.loadXML("data/img/bunny-sprite.xml");
    enemy.setPosition(100,100);
    enemy.loadAnimation("data/img/bunnyanim.xml");
    enemy.setAnimRate(4);
    enemy.setScale(0.3, 0.3);

    //Start enemy
    enemy.setXspeed(180);
    enemy.setAnimation("walk");
    enemy.play();

    enemies.push_back(&enemy1);
    enemies.push_back(&enemy2);
    //enemies.push_back(&enemy3);
    //enemies.push_back(&enemy4);
    //enemies.push_back(&enemy5);

    for (int i = 0; i < enemies.size(); i++)
    {
        enemies[i]->loadXML("data/img/bunny-sprite.xml");
        enemies[i]->loadAnimation("data/img/bunnyanim.xml");
        enemies[i]->setAnimRate(4);
        enemies[i]->setScale(0.2, 0.2);
        enemies[i]->setXspeed(-100);
        enemies[i]->setAnimation("walk");
        enemies[i]->play();
    }

    enemy1.setPosition(280,218);
    enemy2.setPosition(280,475);
    //enemy3.setPosition(100,100);
    //enemy4.setPosition(100,100);
    //enemy5.setPosition(100,100);

    map = new tmx::MapLoader("data/maps");
    map->Load("dungeon-tilesets2.tmx");

    dirx = 0; // direção do sprite: para a direita (1), esquerda (-1)
    diry = 0;
    speedY= 0;

    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("up", sf::Keyboard::Up);
    im->addKeyInput("down", sf::Keyboard::Down);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("zoomin", sf::Keyboard::Z);
    im->addKeyInput("zoomout", sf::Keyboard::X);
    im->addMouseInput("rightclick", sf::Mouse::Right);

    if (!font.loadFromFile("data/fonts/arial.ttf"))
    {
        cout << "Cannot load arial.ttf font!" << endl;
        exit(1);
    }

    // select the font
    text.setFont(font);
    text.setString(L"Teste de texto gráfico");
    text.setCharacterSize(24); // in pixels, not points!
    text.setColor(sf::Color::Red);
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);

	cout << "PlayState Init Successful" << endl;
}

void PlayState::cleanup()
{
	cout << "PlayState Clean Successful" << endl;
	delete map;
}

void PlayState::pause()
{
	cout << "PlayState Paused" << endl;
}

void PlayState::resume()
{
	cout << "PlayState Resumed" << endl;
}

void PlayState::handleEvents(cgf::Game* game)
{
    sf::Event event;
    sf::View view = screen->getView();

    while (screen->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            game->quit();
        if(event.type == sf::Event::KeyPressed)
            if(event.key.code == sf::Keyboard::S)
                game->toggleStats();
    }

    dirx = diry = 0;

    if(im->testEvent("up")) {
        if(player.getYspeed() >= 0) {
            //player.setAnimation("arms-up");
            //player.play();
        }
        if (speedY >= 0)
                speedY = -300;
    }

    if(im->testEvent("left")) {
        if(player.getXspeed() >= 0) {
            player.setAnimation("walk-left");
            player.play();
        }
        dirx = -1;
    }
    else
    if(im->testEvent("right")) {
        if(player.getXspeed() <= 0) {
            player.setAnimation("walk-right");
            player.play();
        }
        dirx = 1;
    }

    if(im->testEvent("down")) {
        if(player.getYspeed() <= 0) {
            player.setAnimation("arms-up");
            player.play();
        }
        diry = 1;
    }

    if(!dirx && !diry)
    {
        player.setCurrentFrame(0);
        player.pause();
    }

    if(im->testEvent("quit") || im->testEvent("rightclick"))
        game->quit();

    if(im->testEvent("zoomin"))
    {
        view.zoom(1.01);
        screen->setView(view);
    }
    else if(im->testEvent("zoomout"))
    {
        view.zoom(0.99);
        screen->setView(view);
    }

    player.setXspeed(dirx*100);
    player.setYspeed(speedY);
}

void PlayState::update(cgf::Game* game)
{
    screen = game->getScreen();

    if (checkCollision(2, game, &enemy))
    {
        if (enemy.getXspeed() >= 0)
        {
            enemy.setXspeed(-180);
            enemy.setAnimation("walk");
            enemy.play();
        }
        else
        {
            enemy.setXspeed(180);
            enemy.setAnimation("die");
            enemy.play();
        }
    }

    //if (player.bboxCollision(enemy))
        //cout  << "MA - CA - CO" << endl;

    if (speedY < 400)
            speedY += 15;
    if (checkCollision(2, game, &player))
            speedY = 0; // 3a. camada

    for (int i = 0; i < enemies.size(); i++)
    {
        if (checkCollision(3, game, enemies[i]))
        {
            if (enemies[i]->getXspeed() >= 0)
            {
                enemies[i]->setXspeed(-100);
                enemies[i]->setMirror(false);
            }
            else
            {
                enemies[i]->setXspeed(100);
                enemies[i]->setMirror(true);
            }
        }
    }

    centerMapOnPlayer();
}

bool PlayState::checkCollision(u_int8_t layer, cgf::Game* game, cgf::Sprite* obj)
{
    int i, x1, x2, y1, y2;
    bool bump = false;

    // Get the limits of the map
    sf::Vector2u mapsize = map->GetMapSize();
    // Get the width and height of a single tile
    sf::Vector2u tilesize = map->GetMapTileSize();

    mapsize.x /= tilesize.x;
    mapsize.y /= tilesize.y;
    mapsize.x--;
    mapsize.y--;

    // Get the height and width of the object (in this case, 100% of a tile)
    sf::Vector2u objsize = obj->getSize();
    objsize.x *= obj->getScale().x;
    objsize.y *= obj->getScale().y;

    float px = obj->getPosition().x;
    float py = obj->getPosition().y;

    double deltaTime = game->getUpdateInterval();

    sf::Vector2f offset(obj->getXspeed()/1000 * deltaTime, obj->getYspeed()/1000 * deltaTime);

    float vx = offset.x;
    float vy = offset.y;

    // Test the horizontal movement first
    i = objsize.y > tilesize.y ? tilesize.y : objsize.y;

    for (;;)
    {
        x1 = (px + vx) / tilesize.x;
        x2 = (px + vx + objsize.x - 1) / tilesize.x;

        y1 = (py) / tilesize.y;
        y2 = (py + i - 1) / tilesize.y;

        if (x1 >= 0 && x2 < mapsize.x && y1 >= 0 && y2 < mapsize.y)
        {
            if (vx > 0)
            {
                // Trying to move right

                int upRight   = getCellFromMap(layer, x2*tilesize.x, y1*tilesize.y);
                int downRight = getCellFromMap(layer, x2*tilesize.x, y2*tilesize.y);
                if (upRight || downRight)
                {
                    // Place the player as close to the solid tile as possible
                    px = x2 * tilesize.x;
                    px -= objsize.x;// + 1;
                    vx = 0;
                    bump = true;
                }
            }

            else if (vx < 0)
            {
                // Trying to move left

                int upLeft   = getCellFromMap(layer, x1*tilesize.x, y1*tilesize.y);
                int downLeft = getCellFromMap(layer, x1*tilesize.x, y2*tilesize.y);
                if (upLeft || downLeft)
                {
                    // Place the player as close to the solid tile as possible
                    px = (x1+1) * tilesize.x;
                    vx = 0;
                    bump = true;
                }
            }
        }

        if (i == objsize.y) // Checked player height with all tiles ?
        {
            break;
        }

        i += tilesize.y; // done, check next tile upwards

        if (i > objsize.y)
        {
            i = objsize.y;
        }
    }

    // Now test the vertical movement

    i = objsize.x > tilesize.x ? tilesize.x : objsize.x;

    for (;;)
    {
        x1 = (px / tilesize.x);
        x2 = ((px + i-1) / tilesize.x);

        y1 = ((py + vy) / tilesize.y);
        y2 = ((py + vy + objsize.y-1) / tilesize.y);

        if (x1 >= 0 && x2 < mapsize.x && y1 >= 0 && y2 < mapsize.y)
        {
            if (vy > 0)
            {
                // Trying to move down
                int downLeft  = getCellFromMap(layer, x1*tilesize.x, y2*tilesize.y);
                int downRight = getCellFromMap(layer, x2*tilesize.x, y2*tilesize.y);
                if (downLeft || downRight)
                {
                    // Place the player as close to the solid tile as possible
                    py = y2 * tilesize.y;
                    py -= objsize.y;
                    vy = 0;
                    bump = true;
                }
            }

            else if (vy < 0)
            {
                // Trying to move up

                int upLeft  = getCellFromMap(layer, x1*tilesize.x, y1*tilesize.y);
                int upRight = getCellFromMap(layer, x2*tilesize.x, y1*tilesize.y);
                if (upLeft || upRight)
                {
                    // Place the player as close to the solid tile as possible
                    py = (y1 + 1) * tilesize.y;
                    vy = 0;
                    bump = true;
                }
            }
        }

        if (i == objsize.x)
        {
            break;
        }

        i += tilesize.x; // done, check next tile to the right

        if (i > objsize.x)
        {
            i = objsize.x;
        }
    }

    // Now apply the movement and animation

    obj->setPosition(px+vx,py+vy);
    px = obj->getPosition().x;
    py = obj->getPosition().y;

    obj->update(deltaTime, false); // only update animation

    // Check collision with edges of map
    if (px < 0)
        obj->setPosition(px,py);
    else if (px + objsize.x >= mapsize.x * tilesize.x)
        obj->setPosition(mapsize.x*tilesize.x - objsize.x - 1,py);

    if(py < 0)
        obj->setPosition(px,0);
    else if(py + objsize.y >= mapsize.y * tilesize.y)
        obj->setPosition(px, mapsize.y*tilesize.y - objsize.y - 1);

    return bump;
}

sf::Uint16 PlayState::getCellFromMap(uint8_t layernum, float x, float y)
{
    auto& layers = map->GetLayers();
    tmx::MapLayer& layer = layers[layernum];
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2u tilesize = map->GetMapTileSize();
    mapsize.x /= tilesize.x;
    mapsize.y /= tilesize.y;
    int col = floor(x / tilesize.x);
    int row = floor(y / tilesize.y);
    return layer.tiles[row*mapsize.x + col].gid;
}

void PlayState::centerMapOnPlayer()
{
    sf::View view = screen->getView();
    sf::Vector2u mapsize = map->GetMapSize();
    sf::Vector2f viewsize = view.getSize();
    viewsize.x /= 2;
    viewsize.y /= 2;
    sf::Vector2f pos = player.getPosition();

    float panX = viewsize.x; // minimum pan
    if(pos.x >= viewsize.x)
        panX = pos.x;

    if(panX >= mapsize.x - viewsize.x)
        panX = mapsize.x - viewsize.x;

    float panY = viewsize.y; // minimum pan
    if(pos.y >= viewsize.y)
        panY = pos.y;

    if(panY >= mapsize.y - viewsize.y)
        panY = mapsize.y - viewsize.y;

    view.setCenter(sf::Vector2f(panX,panY));
    screen->setView(view);
}

void PlayState::draw(cgf::Game* game)
{
    //sf::View view = screen->getView();

    screen->clear(sf::Color(0,0,0));

    map->Draw(*screen, 0);
    screen->draw(enemy);
    for (int i = 0; i < enemies.size(); i++)
        screen->draw(*enemies[i]);
    screen->draw(player);
    map->Draw(*screen, 1);

    screen->draw(text);
}
