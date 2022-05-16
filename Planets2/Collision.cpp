#include "Collision.h"
#include "Body.h"

Collision::Collision(std::pair<Body*, Body*> collided) : bigger(*collided.first), smaller(*collided.second)
{}
