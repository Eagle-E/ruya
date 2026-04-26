#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;
using glm::mat4;


namespace ruya::math
{
    
    struct Transform
    {
        vec3 position {0.0f, 0.0f, 0.0f};
        vec3 rotation {0.0f, 0.0f, 0.0f};
        vec3 scale    {1.0f, 1.0f, 1.0f};


        /* Rotate this transform, x, y and z are in degrees.
        */
        void rotate(float x, float y, float z)
        {
            rotation.x = fmod(rotation.x + x, 360);
            rotation.y = fmod(rotation.y + y, 360);
            rotation.z = fmod(rotation.z + z, 360);
        }
        void rotate_x(float degrees) { rotation.x = fmod(rotation.x + degrees, 360); } // idem rotate() but on 1 axis
        void rotate_y(float degrees) { rotation.y = fmod(rotation.y + degrees, 360); } 
        void rotate_z(float degrees) { rotation.z = fmod(rotation.z + degrees, 360); }
    };



    inline mat4 model_matrix(const Transform & t)
    {
        mat4 I(1.0f); // identity matrix

        // scale matrix
        mat4 S = glm::scale(I, t.scale);

        // rotation matrix
        mat4 R = I;
        vec3 r = t.rotation;
        vec3 x_axis(1.0f, 0.0f, 0.0f);
        vec3 y_axis(0.0f, 1.0f, 0.0f);
        vec3 z_axis(0.0f, 0.0f, 1.0f);
        // rotate separately around each axis
        // if (r.x != 0)   R = glm::rotate(R, glm::radians(r.x), x_axis);
        // if (r.y != 0)   R = glm::rotate(R, glm::radians(r.y), y_axis);
        // if (r.z != 0)   R = glm::rotate(R, glm::radians(r.z), z_axis);
        R = glm::rotate(R, glm::radians(r.x), x_axis);
        R = glm::rotate(R, glm::radians(r.y), y_axis);
        R = glm::rotate(R, glm::radians(r.z), z_axis);

        // translation matrix
        mat4 T = glm::translate(I, t.position);

        return T * R * S;
    }
    
} // namespace ruya

#endif //TRANSFORM_H
