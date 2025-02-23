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
    tPrev(0)
{

    // torus(1.75f*0.75f, 0.75f * 0.75f,50, 50)

    //mesh = ObjMesh::load("media/pig_triangulated.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Initialise model matrix
    model = mat4(1.0f);
    view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    angle = 0.0f;


    // Set shader uniforms varuiaubles

    prog.setUniform("Light.L", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.5f));

    GLuint brick = Texture::loadTexture("media/texture/brick1.jpg");
    GLuint moss = Texture::loadTexture("media/texture/moss.png");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brick);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, moss);

}


void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        prog.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += 0.1f * deltaT;
    if (angle > glm::two_pi<float>())angle -= glm::two_pi<float>();
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color & depth buffers

    vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
    prog.setUniform("Light.Position", vec4(view * lightPos));

    prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    prog.setUniform("Material.Shininess", 100.0f);                 // Higher = sharper highlight

   
        model = mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
        setMatrices(); // Apply model-view-projection transformations
        cube.render(); // Render the torus object

}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model; // Compute the Model-View matrix

    // Send Model-View matrix to the shader
    prog.setUniform("ModelViewMatrix", mv);

    // Compute and send Normal Matrix (used for transforming normals correctly)
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    // Send Model-View-Projection matrix to the shader
    prog.setUniform("MVP", projection * mv);
}