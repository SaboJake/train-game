#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"


namespace implemented
{
    class MyCamera
    {
     public:
        MyCamera()
        {
            position    = glm::vec3(0, 2, 5);
            forward     = glm::vec3(0, 0, -1);
            up          = glm::vec3(0, 1, 0);
            right       = glm::vec3(1, 0, 0);
            distanceToTarget = 2;
        }

        MyCamera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            Set(position, center, up);
        }

        ~MyCamera()
        { }

        // Update camera
        void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            this->position = position;
            forward     = glm::normalize(center - position);
            right       = glm::cross(forward, up);
            this->up    = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            // Translates the camera using the `dir` vector computed from
            // `forward`. Movement will always keep the camera at the same
            // height. For example, if you rotate your head up/down, and then
            // walk forward, then you will still keep the same relative
            // distance (height) to the ground!
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            // TODO(student): Translate the camera using the `forward` vector.
            // What's the difference between `TranslateForward()` and
            // `MoveForward()`?
			position += glm::normalize(forward) * distance;

        }

        void TranslateUpward(float distance)
        {
            // TODO(student): Translate the camera using the `up` vector.
			position += glm::normalize(up) * distance;
        }

        void TranslateRight(float distance)
        {
            // TODO(student): See instructions below. Read the entire thing!
            // You need to translate the camera using the `right` vector.
            // Usually, however, translation using camera's `right` vector
            // is not very useful, because if the camera is rotated around the
            // `forward` vector, then the translation on the `right` direction
            // will have an undesired effect, more precisely, the camera will
            // get closer or farther from the ground. The solution is to
            // actually use the projected `right` vector (projected onto the
            // ground plane), which makes more sense because we will keep the
            // same distance from the ground plane.
			position += glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward)) * distance;

        }

        void RotateFirstPerson_OX(float angle)
        {
            // TODO(student): Compute the new `forward` and `up` vectors.
            // Don't forget to normalize the vectors! Use `glm::rotate()`.
            forward = glm::normalize(glm::rotate(glm::quat(1, 0, 0, 0), angle, right) * glm::vec3(forward));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateFirstPerson_OY(float angle)
        {
            // TODO(student): Compute the new `forward`, `up` and `right`
            // vectors. Use `glm::rotate()`. Don't forget to normalize the
            // vectors!
            forward = glm::normalize(glm::rotate(glm::quat(1, 0, 0, 0), angle, glm::vec3(0, 1, 0)) * glm::vec3(forward));
            right = glm::normalize(glm::rotate(glm::quat(1, 0, 0, 0), angle, glm::vec3(0, 1, 0)) * glm::vec3(right));
            up = glm::normalize(glm::cross(right, forward));

        }

        void RotateFirstPerson_OZ(float angle)
        {
            // TODO(student): Compute the new `right` and `up`. This time,
            // `forward` stays the same. Use `glm::rotate()`. Don't forget
            // to normalize the vectors!
            right = glm::normalize(glm::rotate(glm::quat(1, 0, 0, 0), angle, forward) * glm::vec3(right));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateThirdPerson_OX(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OX axis. Use `distanceToTarget` as translation distance.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OY(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OY axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateForward(-distanceToTarget);

        }

        void RotateThirdPerson_OZ(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OZ axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);

        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

        void OnInputUpdate(WindowObject* window, float deltaTime, int mods)
        {
			speedUp = window->KeyHold(GLFW_KEY_LEFT_SHIFT);
            float cameraSpeed = 10.0f;
            float angularSpeed = 100.0f;
            if (speedUp) {
                cameraSpeed *= 2;
            }
            // move the camera only if MOUSE_RIGHT button is pressed
            if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
            {
                if (window->KeyHold(GLFW_KEY_W)) {
                    // TODO(student): Translate the camera forward
                    TranslateForward(cameraSpeed * deltaTime);
                }

                if (window->KeyHold(GLFW_KEY_A)) {
                    // TODO(student): Translate the camera to the left
                    TranslateRight(cameraSpeed * deltaTime);
                }

                if (window->KeyHold(GLFW_KEY_S)) {
                    // TODO(student): Translate the camera backward
                    TranslateForward(-cameraSpeed * deltaTime);
                }

                if (window->KeyHold(GLFW_KEY_D)) {
                    // TODO(student): Translate the camera to the right
                    TranslateRight(-cameraSpeed * deltaTime);
                }

                if (window->KeyHold(GLFW_KEY_Q)) {
                    // TODO(student): Translate the camera downward
                    TranslateUpward(-cameraSpeed * deltaTime);
                }

                if (window->KeyHold(GLFW_KEY_E)) {
                    // TODO(student): Translate the camera upward
                    TranslateUpward(cameraSpeed * deltaTime);
                }
            }

            // TODO(student): Change projection parameters. Declare any extra
            // variables you might need in the class header. Inspect this file
            // for any hardcoded projection arguments (can you find any?) and
            // replace them with those extra variables.
            if (window->KeyHold(GLFW_KEY_Z)) {
                fov += 0.5f * deltaTime;
                fov = glm::min(fov, RADIANS(120.0f));
            }
            if (window->KeyHold(GLFW_KEY_X)) {
                fov -= 0.5f * deltaTime;
                fov = glm::max(fov, RADIANS(10.0f));
            }
        }

        void OnMouseMove(WindowObject* window, int mouseX, int mouseY, int deltaX, int deltaY)
        {
            if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
            {
                float sensivityOX = 0.0005f;
                float sensivityOY = 0.0005f;

                //renderCameraTarget = false;
                // TODO(student): Rotate the camera in first-person mode around
                // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
                // variables for setting up the rotation speed.
                float rotateSpeed = 10.0f;

                //cout << "Rotate first person" << endl;

                RotateFirstPerson_OX(-rotateSpeed * sensivityOX * deltaY);
                RotateFirstPerson_OY(-rotateSpeed * sensivityOY * deltaX);

            }
        }

        void UpdateProjectionMatrix(float aspectRatio)
        {
            projectionMatrix = glm::perspective(fov, aspectRatio, zNear, zFar);
		}

     public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
    private:
		float fov;
		float zNear;
        float zFar;
		bool speedUp = false;
		
		glm::mat4 projectionMatrix;

    public:
        float getFov() const { return fov; }
        void setFov(float fov) { this->fov = fov; }
        float getzNear() const { return zNear; }
        void setzNear(float zNear) { this->zNear = zNear; }
        float getzFar() const { return zFar; }
		void setzFar(float zFar) { this->zFar = zFar; }

		glm::mat4 GetProjectionMatrix() const { return projectionMatrix; }
		void SetProjectionMatrix(const glm::mat4& matrix) { projectionMatrix = matrix; }
    };
}   // namespace implemented
