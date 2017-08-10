#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "topViewFb.hpp"
#include "mainCamera.hpp"
#include "glDebug.hpp"


TopViewFb::TopViewFb(int resWidth, int resHeight) 
: Framebuffer(resWidth, resHeight) {}

void TopViewFb::Draw(const LODPlane &lodPlane, TopCamera* topCam, MainCamera* mainCam) {
    Bind();
    GL_CHECK(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)); 
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, lodPlane.GetHeightmapTexture()));
    lodPlane.shader.UniformMatrix4fv("projMat", topCam->projectionMatrix);
    topCam->SetPosition(glm::vec3(mainCam->GetPosition().x, topCam->GetPosition().y, 
                                  mainCam->GetPosition().z));
    lodPlane.DrawFrom(*mainCam, topCam);
    Unbind();
}