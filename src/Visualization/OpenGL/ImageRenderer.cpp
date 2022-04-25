#include "ImageRenderer.h"
#include "Loader.h"

using namespace GL;

ImageRenderer::ImageRenderer()
{
}

void ImageRenderer::init(Loader* loader, AudioVisualizerInfo visualizerInfo)
{
    quadID = loader->createVAO();
    loader->unbindVAO();
    isScaling = visualizerInfo.scaling;
    textureID = loader->loadTexture(visualizerInfo.imageURL);
    GL_CHECK();
    float aspect = visualizerInfo.screenDimensions.x / (float)visualizerInfo.screenDimensions.y;
    projectionMatrix = glm::perspective(FOV, aspect, NEAR_PLANE, FAR_PLANE);
    viewMatrix = glm::lookAt(glm::vec3(0, 0, -1.f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    GL_CHECK();
    shader = new ImageShader();
    shader->start();
    shader->loadProjection(projectionMatrix);
    shader->loadViewMatrix(viewMatrix);
    shader->loadIntensityMultipliers(visualizerInfo.intensityScale, visualizerInfo.intensityOffset);
    shader->stop();
    GL_CHECK();
}

void ImageRenderer::render(float bass)
{
    shader->start();
    transformationMatrix = glm::identity<glm::mat4>();
    if (isScaling)
    {
        transformationMatrix = glm::scale(transformationMatrix, glm::vec3(1 + bass, 1 + bass, 1));
    }
    shader->loadTransformationMatrix(transformationMatrix);

    glBindVertexArray(quadID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->loadTextures();
    shader->loadBass(bass);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader->stop();
}
