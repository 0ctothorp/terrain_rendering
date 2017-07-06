#include "topViewFb.hpp"
#include "glDebug.hpp"
#include "mainCamera.hpp"


TopViewFb::TopViewFb(int resWidth, int resHeight) : Framebuffer(resWidth, resHeight) {
    topViewProjMat = glm::perspective(glm::radians(60.0f), 
                                      (float)GetResWidth() / 
                                      (float)GetResHeight(), 
                                      0.1f, 3000.0f);
}

void TopViewFb::Draw(const LODPlane &lodPlane, TopCamera &topCam) const {
    Bind();
    GL_CHECK(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)); // we're not using the stencil buffer now
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, lodPlane.GetHeightmapTexture()));
    GL_CHECK(glUniformMatrix4fv(TileMaterial::shader->GetUniformLocation("projMat"), 
                                1, GL_FALSE, glm::value_ptr(topViewProjMat)));
    MainCamera *mainCam = MainCamera::GetInstance();
    glm::vec3 mainCamPos = mainCam->GetPosition();
    topCam.SetPosition(glm::vec3(mainCamPos.x, topCam.GetPosition().y, mainCamPos.z));
    lodPlane.DrawFrom(*mainCam, &topCam);
    Unbind();
}