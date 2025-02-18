#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;


SceneBasic_Uniform::SceneBasic_Uniform() : torus(0.7f, 0.3f, 30, 30) {}

void SceneBasic_Uniform::initScene()
{
    compile(); // Keep the shader compilation

    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Initialise model matrix
    model = glm::mat4(1.0f);
    //rotate torus towards camera
    model = glm::rotate(model, glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));

    // Initialise view matrix
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f),  // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Look-at point
        glm::vec3(0.0f, 1.0f, 0.0f)); // Up direction

    // Initialise projection matrix
    projection = glm::mat4(1.0f); // Placeholder, should be replaced with an actual perspective matrix

    // Set shader uniforms
    prog.setUniform("LightPosition", view * glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));

    prog.setUniform("Ks", glm::vec3(1.0f, 1.0f, 1.0f));  // White specular highlight
    prog.setUniform("Shininess", 100.0f);                 // Higher = sharper highlight
    prog.setUniform("Kd", glm::vec3(1.0f, 1.0f, 1.0f));  // Set diffuse reflectivity (White)
    prog.setUniform("Ld", glm::vec3(1.0f, 1.0f, 1.0f));  // Set light intensity (White)

}

void SceneBasic_Uniform::setMatrices()
{
    glm::mat4 mv = view * model; // Compute the Model-View matrix

    // Send Model-View matrix to the shader
    prog.setUniform("ModelViewMatrix", mv);

    // Compute and send Normal Matrix (used for transforming normals correctly)
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    // Send Model-View-Projection matrix to the shader
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color & depth buffers

    setMatrices(); // Apply model-view-projection transformations

    torus.render(); // Render the torus object
}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}
