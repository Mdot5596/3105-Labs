#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H
#include "helper/scene.h"
#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/torus.h"
#include "helper/teapot.h"
#include "glm/glm.hpp"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/cube.h"
#include "helper/sphere.h"
class SceneBasic_Uniform : public Scene
{
private:
  //  Cube cube;
 //   Sphere sphere;
//   std::unique_ptr<ObjMesh> spot;
    GLuint deferredFBO;
    GLuint quad;

    Torus torus;
    Plane plane;
    Teapot teapot;

    float tPrev;
    float angle;
    float rotSpeed;
    GLSLProgram prog;
    void setMatrices();
    void compile();

    void setupFBO();
    void pass1();
    void pass2();

    void createGBufTex(GLenum, GLenum, GLuint&);


public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
