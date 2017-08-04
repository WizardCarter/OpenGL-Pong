#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "Sprite.h"
#include "Texture.h"
#include "Shader.h"

SpriteRenderer::SpriteRenderer(Shader shader, glm::mat4 proj)
{
    this->shader = shader;
    this->projection = proj;
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);
    glDeleteVertexArrays(1, &quadVAO); //get rid of buffers to save memory
}

void SpriteRenderer::initRenderData()
{
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO); //generate storage buffers
    glGenBuffers(1, &quadEBO);
    float vertices[] = {
        0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 1.0,
        1.0, -1.0, 1.0, 0.0,
        0.0, -1.0, 0.0, 0.0
    };
    int elements[] = {
        0, 1, 2,
        2, 3, 0
    };
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO); //store the data for the vertices
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO); //and for the elements
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindVertexArray(0);
}

void SpriteRenderer::drawSprite(Texture2D &texture, Sprite sprite)
{
    shader.Use(); //use the shader
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(sprite.position, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f*sprite.size.x, 0.5f*sprite.size.y, 0.0f));
    model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f*sprite.size.x, 0.5f*sprite.size.y, 0.0f));

    model = glm::scale(model, glm::vec3(sprite.size, 1.0f)); //scale to the appropriate size

    shader.SetMatrix4("model", model);
    shader.SetMatrix4("proj", projection);
    shader.SetVector3f("color", sprite.color.x, sprite.color.y, sprite.color.z);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind(); //bind the texture

    glBindVertexArray(this->quadVAO); //draw
    //glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::drawSprite(Texture2D &texture, RSprite sprite)
{
    shader.Use(); //use the shader
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(sprite.position, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f*sprite.radius, 0.5f*sprite.radius, 0.0f));
    model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f*sprite.radius, 0.5f*sprite.radius, 0.0f));

    model = glm::scale(model, glm::vec3(sprite.radius, sprite.radius, 1.0f)); //scale to the appropriate size

    shader.SetMatrix4("model", model);
    shader.SetMatrix4("proj", projection);
    shader.SetVector3f("color", sprite.color.x, sprite.color.y, sprite.color.z);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind(); //bind the texture

    glBindVertexArray(this->quadVAO); //draw
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::drawSpriteNoTexture(Sprite sprite)
{
    shader.Use();
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(sprite.position, 0.0));

    model = glm::translate(model, glm::vec3(0.5f*sprite.size, 0.0));
    model = glm::rotate(model, sprite.rotation, glm::vec3(0.0, 0.0, 1.0));
    model = glm::translate(model, glm::vec3(-0.5f*sprite.size, 0.0));

    model = glm::scale(model, glm::vec3(sprite.size, 1.0));

    shader.SetMatrix4("model", model);
    shader.SetMatrix4("proj", projection);
    shader.SetVector3f("color", sprite.color);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(this->quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

bool checkCollision(Sprite &one, Sprite &two)
{
    bool x = (one.position.x + one.size.x >= two.position.x) && (two.position.x
    + two.size.x >= one.position.x);

    bool y = (one.position.y + one.size.y >= two.position.y) && (two.position.y
    + two.size.y >= one.position.y);

    return (x&&y);
}

bool checkCollision(Sprite& one, RSprite& two)
{
    // Get center point circle first
    glm::vec2 center(two.position + two.radius);
    // Calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(one.size.x / 2, one.size.y / 2);
    glm::vec2 aabb_center(
        one.position.x + aabb_half_extents.x,
        one.position.y + aabb_half_extents.y
    );
    // Get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // Add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // Retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    return glm::length(difference) <= two.radius;
}
