#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "Shader.h"

using namespace std;

//TODO - Set up Particle System and test it

class ParticleSystem
{
    public:
        ParticleSystem(glm::vec2 pos, Shader s, unsigned int particleNum, glm::mat4 proj, float lifeT, glm::vec2 ballVel);
        ~ParticleSystem();
        glm::vec2 position;
        Shader shader;
        GLuint vao;
        GLuint vbo;
        glm::mat4 proj;
        unsigned int particleNum;

        float particleLife; //particle lifetime in seconds

        struct Particle {
            glm::vec2 position;
            glm::vec2 scale;
            float alpha = 1.0;
            float lifetime;
            glm::vec3 color;
            glm::vec2 velocity;
        };
        Particle *particles;

        void update(float dt, glm::vec2 ballVel);
        void render();
    protected:
        void resetParticle(int index, glm::vec2 velocity);
};

#endif // PARTICLESYSTEM_H
