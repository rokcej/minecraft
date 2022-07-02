#pragma once

#include <glm/glm.hpp>
#include <src/entity.h>

class Camera : public Entity {
public:
	Camera();

	void Update();
	void SetAspectRatio(float aspect_ratio);
	const glm::mat4& GetProjViewMat() const;

private:
	void UpdateProjMat();

private:
	glm::mat4 proj_mat_;
	glm::mat4 proj_view_mat_;

	float fov_ = 90.0f;
	float aspect_ratio_ = 1.0f;

};
