#include "object.h"
#include <glm/gtc/matrix_transform.hpp>


ruya::Object::Object()
    : mPosition(0.0f), mRotation(0.0f), mScale(1.0f), mColor(0.99f), mMesh(nullptr), mTexture(nullptr), mParent(nullptr), mMaterial(Materials::silver)
{
}

ruya::Object::~Object()
{
    
}


/*
* Get the model matrix that defines the transformation (scaling, rotation 
* and translation) of this object.
*/
mat4 ruya::Object::model_matrix()
{
    mat4 scalingMatrix(1.0f);
    mat4 rotationMatrix(1.0f);
    mat4 translationMatrix(1.0f);

    scalingMatrix = glm::scale(scalingMatrix, mScale);

    // rotate around custom axis
    //if (mRotation.x != 0 || mRotation.y != 0 || mRotation.z != 0)
    //    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation.w), glm::vec3(mRotation.x, mRotation.y, mRotation.z));

    // rotate separately around each axis
    if (mRotation.x != 0)   rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    if (mRotation.y != 0)   rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    if (mRotation.z != 0)   rotationMatrix = glm::rotate(rotationMatrix, glm::radians(mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    translationMatrix = glm::translate(translationMatrix, mPosition);

    return translationMatrix * rotationMatrix * scalingMatrix;
}

/*
* The inverse of the model matrix. 
*
* Translation is disregarded, the inverse of Scale * Rotation is returned. 
* Rotation and uniform scaling is orthonormal so the inverse of the model 
* matrix is just its transpose. 
* 
* If the scaling is non-uniform (different amount for x, y and/or z axis, then
* the inverse has to be calculated the normal way.
*/
mat4 ruya::Object::inverse_model_matrix()
{
    return model_matrix_and_inverse().second;
}

std::pair<mat4, mat4> ruya::Object::model_matrix_and_inverse()
{
    std::pair<mat4, mat4> model_invModel;
    float d1 = abs(mScale.x - mScale.y);
    float d2 = abs(mScale.y - mScale.z);
    float d3 = abs(mScale.x - mScale.z);

    model_invModel.first = model_matrix();

    float thresh = 0.001;
    if (d1 > thresh || d2 > thresh || d3 > thresh)
        model_invModel.second = glm::inverse(model_invModel.first);
    else
    {
        //model_invModel.second =  mat4(glm::transpose(glm::mat3(model_invModel.first)));
        //model_invModel.second[3][0] = -mPosition[0];
        //model_invModel.second[3][1] = -mPosition[1];
        //model_invModel.second[3][2] = -mPosition[2];
        //model_invModel.second[3][3] = 1.0f;
        model_invModel.second = glm::inverse(model_invModel.first);
        //TODO: optimize calculation of the inverse 
    }

    return model_invModel;
}

/*
* Adds xyzDegrees to the current rotation of the object. In other words:
* the object gets rotated further. For accumulating rotations.
*/
void ruya::Object::rotate(float xDegrees, float yDegrees, float zDegrees)
{
    mRotation.x = fmod(mRotation.x + xDegrees, 360);
    mRotation.y = fmod(mRotation.y + yDegrees, 360);
    mRotation.z = fmod(mRotation.z + zDegrees, 360);
}

/*
* Adds a child to this object
*   - an object can have only one parent
*   - if a child object gets assigned to a parent obj while already 
*       being the child of another, then the child removes itself from 
*       the old parent and gets added to the new one.
*/
void ruya::Object::add_child(Object* obj)
{
    mChildren.push_back(obj);
}

bool ruya::Object::remove_child(Object& obj)
{
    for (auto ichild = mChildren.begin(); ichild != mChildren.end(); ichild++)
    {
        Object& child = **ichild;

        if (child.mUUID == obj.mUUID)
        {
            mChildren.erase(ichild);
            return true;
        }
    }

    return false;
}

bool ruya::Object::operator==(const Object& other)
{
    return this->mUUID == other.mUUID;
}

/*
* Removes self from parent, then sets parent ptr to null
*/
void ruya::Object::dislodge_from_parent()
{
    if (mParent != nullptr)
    {
        mParent->remove_child(*this);
        mParent = nullptr;
    }
}

