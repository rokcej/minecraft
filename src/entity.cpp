#include "entity.h"

void Entity::SetPosition(const glm::vec3& position) {
	position_ = position;
}

void Entity::SetRotation(const glm::vec3& rotation) {
	rotation_ = rotation;
}

const glm::vec3& Entity::GetPosition() const {
	return position_;
}

const glm::vec3& Entity::GetRotation() const {
	return rotation_;
}
