#include "ParticleSystem.h"

#include <SFML/System.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "Shader.h"

ParticleSystem::ParticleSystem(glm::vec2 pos, Shader s, unsigned int particleNum, glm::mat4 proj, float lifeT, glm::vec2 ballVel)
{
    position = pos; //set up class variables
    shader = s;
    particles = new Particle[particleNum];
    this->proj = proj;
    this->particleNum = particleNum;
    particleLife = lifeT;

    for (int i=0;i<particleNum;i++) { //initialize the particles
        resetParticle(i, ballVel);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float vertices[] = {
        0.0, 0.0
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    delete particles;
}

void ParticleSystem::resetParticle(int index, glm::vec2 velocity)
{
    Particle& p = particles[index];
    p.lifetime = particleLife;
    p.alpha = 1.0;
    p.position = this->position;
    p.color = glm::vec3(0.0, 0.0, 1.0);
    p.scale = glm::vec2(1.0, 1.0);
    p.velocity = -1.0f*velocity;
}

void ParticleSystem::update(float dt, glm::vec2 ballVel)
{
    for (int i=0;i<particleNum;i++) {
        Particle& p = particles[i];

        p.lifetime -= dt;

        if (p.lifetime <= 0) {
            resetParticle(i, ballVel);
        }

        p.alpha = p.lifetime/particleLife;
        p.position += -1.0f*ballVel*dt;
    }
}

void ParticleSystem::render()
{
    shader.Use(); //use the shader
    shader.SetMatrix4("proj", proj); //set the projection matrix
    for (int i=0;i<particleNum;i++) { //for each particle
        glm::mat4 model;

        model = glm::translate(model, glm::vec3(particles[i].position, 0.0));
        model = glm::scale(model, glm::vec3(particles[i].scale, 0.0)); //set its model matrix

        shader.SetMatrix4("model", model); //set the particle's alpha and color
        shader.SetFloat("alpha", particles[i].alpha);
        shader.SetVector3f("color", particles[i].color);

        glBindVertexArray(vao); //bind and draw
        glDrawArrays(GL_POINTS, 0, 1);
    }
    glBindVertexArray(0); //unbind at the end
}
