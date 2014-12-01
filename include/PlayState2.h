//Nome: Matthias Nunes

#ifndef PLAY_STATE2_H_
#define PLAY_STATE2_H_

#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"
#include <MapLoader.h>
#include <vector>
#include <SFML/Audio.hpp>

class PlayState2 : public cgf::GameState
{
    public:

        void init();
        void cleanup();

        void pause();
        void resume();

        void handleEvents(cgf::Game* game);
        void update(cgf::Game* game);
        void draw(cgf::Game* game);

        // Implement Singleton Pattern
        static PlayState2* instance()
        {
            return &m_PlayState;
        }

    protected:

        PlayState2() {}

    private:

        static PlayState2 m_PlayState;

        // Get a cell GID from the map (x and y are view coords)
        sf::Uint16 getCellFromMap(u_int8_t layernum, float x, float y);

        sf::SoundBuffer monkeySoundBuffer;
        sf::Sound monkeySound;

        sf::SoundBuffer bunnyDyingBuffer;
        sf::Sound bunnyDying;

        sf::Music music;

        // Centers the camera on the player position (if player is too close to the borders, stop)
        void centerMapOnPlayer();

        bool checkCollision(u_int8_t layer, cgf::Game* game, cgf::Sprite* obj);

        bool checkDeaths();

        void reviveBunnies();

        int x, y;
        int speedY;
        int dirx, diry;
        int deaths[10];
        bool jumping;
        cgf::Sprite player;
        cgf::Sprite enemy1;
        cgf::Sprite enemy2;
        cgf::Sprite enemy3;
        cgf::Sprite enemy4;
        cgf::Sprite enemy5;
        cgf::Sprite enemy6;
        cgf::Sprite enemy7;
        cgf::Sprite enemy8;
        cgf::Sprite enemy9;
        cgf::Sprite enemy10;
        std::vector<cgf::Sprite*> enemies;
        sf::RenderWindow* screen;
        cgf::InputManager* im;
        tmx::MapLoader* map;
        sf::Font font;
        sf::Text text;
};

#endif
