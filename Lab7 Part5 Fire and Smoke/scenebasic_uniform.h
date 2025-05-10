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
#include "helper/teapotpatch.h"
#include "helper/random.h"
#include "helper/grid.h"
#include "helper/particleutils.h"

class SceneBasic_Uniform : public Scene
{
private:
  //Cube cube;
  //Sphere sphere;
   //std::unique_ptr<ObjMesh> ogre;
    Torus torus;
   // Plane plane;
    //Teapot teapot;
   // TeapotPatch teapot;
    float tPrev;
    float angle;
    float time, deltaT;
    float particleLifetime;
    int nParticles;
//    float rotSpeed;
    GLSLProgram prog, flatProg;
    Random rand;
    //Particle buffer
    GLuint posBuf[2], velBuf[2], age[2], rotation[2];
    //VAO
    GLuint particleArray[2];
    //Transform feedback
    GLuint feedback[2];
    GLuint drawBuf;
    Grid grid;
    glm::vec3 emitterPos, emitterDir;

    void initBuffers();
  //  float randFloat();

  //  glm::mat4 viewport;
    void setMatrices(GLSLProgram &);
    void compile();


public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
