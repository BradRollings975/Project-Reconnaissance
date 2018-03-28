// Stealth Game.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Game.h"
#include <SFML\Audio.hpp>

using namespace tle;

sf::SoundBuffer buffer;
sf::Sound sound;

bool MenuSound()
{
	if (!buffer.loadFromFile("Stealth Music.ogg"))
	{
		return false;
	}

	sound.setBuffer(buffer);
	sound.play();
	sound.setLoop(true);
}

void main()
{
	Game *g1 = new Game();
	MenuSound();
	g1->run();
	delete g1;
}


