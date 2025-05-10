#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;
#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() :
   angle(0.0f), drawBuf(1), time(0.0f),deltaT(0), particleLifetime(10.5f),
    nParticles(500), emitterPos(0.0f), emitterDir(0,1,0), torus(0.7f*0.1f,0.3f*0.1f,20,20)
{
   // ogre = ObjMesh::loadWithAdjacency("media/bs_ears.obj");
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  //  glEnable(GL_BLEND);
  //  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    
    glActiveTexture(GL_TEXTURE0);
   // float c = 3.5f;
   // projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);
    angle = glm::half_pi<float>();

   // Texture::loadTexture("media/texture/bluewater.png");
   // glActiveTexture(GL_TEXTURE1);
    model = mat4(1.0f);
    ParticleUtils::createRandomTex1D(nParticles * 4);
    initBuffers();

    prog.use();
    prog.setUniform("RandomTex", 0);
    //prog.setUniform("ParticleTex", 0);
    prog.setUniform("ParticleLifetime", particleLifetime);
    //prog.setUniform("ParticleSize", 0.05f);
    prog.setUniform("Accel", vec3(0.0f, -0.4f, 0.0f));
    prog.setUniform("EmitterPos", emitterPos);
    prog.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));
    //Light info
    prog.setUniform("Light.La", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Light.L", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Light.Intensity", vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));

    flatProg.use();
    flatProg.setUniform("Color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));


}




void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        GLuint progHandle = prog.getHandle();
        const char* outputNames[] = { "Position", "Velocity", "Age", "Rotation"};
        glTransformFeedbackVaryings(progHandle, 4, outputNames, GL_SEPARATE_ATTRIBS);


        prog.link();
        prog.use();
        
       flatProg.compileShader("shader/solid.vert");
       flatProg.compileShader("shader/solid.frag");
       flatProg.link();
       
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
   /*
   float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += rotSpeed * deltaT;
    if (angle > glm::two_pi<float>())angle -= glm::two_pi<float>();
    */
    deltaT = t - time;
    time = t;
    angle = std::fmod(angle + 0.01f, glm::two_pi<float>());
}

void SceneBasic_Uniform::render()
{

    prog.use();
    prog.setUniform("Time", time);
    prog.setUniform("DeltaT", deltaT);
    prog.setUniform("Pass", 1);

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
    glBeginTransformFeedback(GL_POINTS);
    glBindVertexArray(particleArray[1 - drawBuf]);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glVertexAttribDivisor(3, 0);
    glVertexAttribDivisor(4, 0);
    glVertexAttribDivisor(5, 0);
    glVertexAttribDivisor(6, 0);
    glDrawArrays(GL_POINTS, 0, nParticles);
    glBindVertexArray(0);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    prog.setUniform("Pass", 2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vec3 cameraPos(3.0f*cos(angle), 1.5f, 3.0f*sin(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
   //projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
    prog.setUniform("Material.Ka", vec3(0.2f, 0.5f, 0.9f));
    prog.setUniform("Material.Kd", vec3(0.2f, 0.5f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("Material.Shininess", 100.0f);
    model = mat4(1.0f);
    setMatrices(prog);

    //vec3 cameraPos(0.0f, 0.0f, 1.5f);
    //view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    /*
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(50.0f), vec3(1.0f, 0.0f, 0.0f));



    setMatrices();
    plane.render();
    */
   // model = mat4(1.0f);

    glBindVertexArray(particleArray[drawBuf]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glDrawElementsInstanced(GL_TRIANGLES, torus.getNumVerts(), GL_UNSIGNED_INT, 0, nParticles);
    glBindVertexArray(0);

    flatProg.use();
    setMatrices(flatProg);
    grid.render();

    drawBuf = 1 - drawBuf;
}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;

    projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices(GLSLProgram &prog )
{
    mat4 mv = view * model; // Compute the Model-View matrix
   // prog.use();
    prog.setUniform("ModelViewMatrix", mv);
 //   prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection*mv);
    prog.setUniform("ProjectionMatrix", projection);
}

void SceneBasic_Uniform::initBuffers()
{
    glGenBuffers(2, posBuf);
    glGenBuffers(2, velBuf);
    glGenBuffers(2, age);
    glGenBuffers(2, rotation);



    int size = nParticles * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size , 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferData(GL_ARRAY_BUFFER,  size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glBufferData(GL_ARRAY_BUFFER,  size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, rotation[0]);
    glBufferData(GL_ARRAY_BUFFER, 2 * size * sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, rotation[1]);
    glBufferData(GL_ARRAY_BUFFER, 2 * size, 0, GL_DYNAMIC_COPY);


    std::vector<GLfloat> tempData(nParticles);
    float rate = particleLifetime / nParticles;
    for (int i = 0; i < nParticles; i++)
    {
        tempData[i] = rate * (i-nParticles);
    }

 
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles*sizeof(float), tempData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenVertexArrays(2, particleArray);

    //ParticleArray 0
    glBindVertexArray(particleArray[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, torus.getElementBuffer());
    glBindBuffer(GL_ARRAY_BUFFER, torus.getPositionBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, torus.getNormalBuffer());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, rotation[0]);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(6);

    //ParticleArray 1
    glBindVertexArray(particleArray[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, torus.getElementBuffer());
    glBindBuffer(GL_ARRAY_BUFFER, torus.getPositionBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, torus.getNormalBuffer());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, rotation[1]);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(6);

    glBindVertexArray(0);

    glGenTransformFeedbacks(2, feedback);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, rotation[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, rotation[1]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

