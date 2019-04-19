
#include "PlayerConcreteFactory.h"
#include "Player.h"

Player* PlayerDefaultFactory::NewPlayer()
{
	return new Player();
}
