#include "player.h"

Player::Player() {

}

void Player::update(float dt) {
	// Position
	if (this->move.x != 0.0f) // Right
		this->pos += this->right * (this->move.x * this->speed * dt);
	if (this->move.y != 0.0f) // Up
		this->pos += glm::vec3(0.f, 1.f, 0.f) * (this->move.y * this->speed * dt);
	if (this->move.z != 0.0f) // Forward
		this->pos += this->forward * (this->move.z * this->speed * dt);

	// Rotation
	this->forward = glm::vec3(
		-sinf(this->rot.y) * cosf(this->rot.x),
		sinf(this->rot.x),
		-cosf(this->rot.y) * cosf(this->rot.x)
	);
	this->right = glm::vec3(cosf(this->rot.y), 0.0f, -sinf(this->rot.y)); // glm::cross(this->forward, this->up);
	this->up = glm::cross(this->right, this->forward);
}
