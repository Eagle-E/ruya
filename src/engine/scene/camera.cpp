#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

ruya::Camera::Camera() : mHorizontalAngle(0), mVerticalAngle(0), mFov(45.0f)
{
    mCamPos = glm::vec3(0.0f, 0.0f, 10.0f);
    mCamFront = glm::vec3(0.0f, 0, -1.0f);
    mCamUp = glm::vec3(0.0f, 1.0f, 0.0f);
}


/*
* Update the angle at which the camera is looking at
*/
void ruya::Camera::update_angle(float horizontalAngle, float verticalAngle)
{
    float dx = horizontalAngle;
    float dy = verticalAngle;
    if (dx == 0 && dy == 0)
        return;

    // update angle if result is within minimum and maximum angles
    if(MIN_VERTICAL_ANGLE < mVerticalAngle-dy && mVerticalAngle-dy < MAX_VERTICAL_ANGLE)
        mVerticalAngle -= dy;

    if(MIN_HORIZONTAL_ANGLE < mHorizontalAngle+dx && mHorizontalAngle+dx < MAX_HORIZONTAL_ANGLE)
        mHorizontalAngle += dx;
    else
        mHorizontalAngle = 0;

    // update view vertex
    this->mCamFront.x = sin(mHorizontalAngle) * cos(mVerticalAngle);
    this->mCamFront.z = -cos(mHorizontalAngle) * cos(mVerticalAngle);
    this->mCamFront.y = sin(mVerticalAngle);
}

float ruya::Camera::horizontal_angle() const
{
    return mHorizontalAngle;
}

float ruya::Camera::vertical_angle() const
{
    return mVerticalAngle;
}

glm::mat4 ruya::Camera::view_matrix() const
{
    return glm::lookAt(mCamPos, mCamPos + mCamFront, mCamUp);
}


glm::vec3 ruya::Camera::cam_front() const
{
    return mCamFront;
}

glm::vec3 ruya::Camera::cam_up() const
{
    return mCamUp;
}


glm::vec3 ruya::Camera::position() const
{
    return mCamPos;
}



/*
void ruya::Camera::go_forward()
{
    mCamPos = mCamPos + mCamFront * mCamSpeed;
}

void ruya::Camera::go_backward()
{
    mCamPos = mCamPos - mCamFront * mCamSpeed;
}

void ruya::Camera::go_right()
{
    mCamPos = mCamPos + glm::normalize(glm::cross(mCamFront, mCamUp)) * mCamSpeed;
}

void ruya::Camera::go_left()
{
    mCamPos = mCamPos - glm::normalize(glm::cross(mCamFront, mCamUp)) * mCamSpeed;
}

void ruya::Camera::go_up()
{
    glm::vec3 right = glm::cross(mCamFront, mCamUp);
    mCamPos = mCamPos + glm::normalize(glm::cross(right, mCamFront)) * mCamSpeed;
}

void ruya::Camera::go_down()
{
    glm::vec3 right = glm::cross(mCamFront, mCamUp);
    mCamPos = mCamPos - glm::normalize(glm::cross(right, mCamFront)) * mCamSpeed;
}
*/

