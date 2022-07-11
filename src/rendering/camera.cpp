#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {
	UpdateProjMat();
}

void Camera::Update() {
	glm::vec3 forward = GetForward();
	glm::mat4 view_mat = glm::lookAt(
		position_,
		position_ + forward,
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	proj_view_mat_ = proj_mat_ * view_mat;
}

void Camera::SetAspectRatio(float aspect_ratio) {
	if (aspect_ratio_ != aspect_ratio) {
		aspect_ratio_ = aspect_ratio;
		UpdateProjMat();
	}
}

const glm::mat4& Camera::GetProjViewMat() const {
	return proj_view_mat_;
}

void Camera::UpdateProjMat() {
	proj_mat_ = glm::perspective(glm::radians(fov_), aspect_ratio_, 0.1f, 256.0f);
}
