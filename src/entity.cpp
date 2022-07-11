#include "entity.h"

#include <cmath>

glm::vec3 Entity::GetForward() const {
	// TODO: Consider storing this vector
	const float cos_rot_x = std::cos(rotation_.x);
	return glm::vec3(
		-std::sin(rotation_.y) * cos_rot_x,
		+std::sin(rotation_.x),
		-std::cos(rotation_.y) * cos_rot_x
	);
}

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
