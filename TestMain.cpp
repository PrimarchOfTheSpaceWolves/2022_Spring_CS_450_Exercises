#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
using namespace std;

vector<GLfloat> vertPos = {
  -0.3f, -0.3f, 0.0f,
  0.3f, -0.3f, 0.0f,
  -0.3f, 0.3f, 0.0f,
  -0.3f, 0.3f, 0.0f,
  0.3f, -0.3f, 0.0f,
  0.3f, 0.3f, 0.0f,
};

vector<GLfloat> vertOnly = {
    -0.4f, -0.3f, 0.0f,
    0.3f, -0.3f, 0.0f,
    -0.3f, 0.3f, 0.0f,
    0.3f, 0.3f, 0.0f
};

vector<GLuint> elements = { 0, 1, 2, 1, 3, 2 };

string vertCode = R"(
#version 430 core

layout(location=0) in vec3 position;

void main() {
    gl_Position = vec4(position, 1.0);
}
)";

string fragCode = R"(
#version 430 core

layout(location=0) out vec4 out_color;

void main() {
    out_color = vec4(0.0, 1.0, 1.0, 1.0);
}
)";

static void error_callback(int e, const char* d) {
    cerr << "ERROR " << e << ": " << d << endl;
}

int main(int argc, char **argv) {
    cout << "BEGIN GRAPHICS!!!!" << endl;

    glfwSetErrorCallback(error_callback);

    if(!glfwInit()) {
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int windowWidth = 640;
    int windowHeight = 480;
    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, 
                                        "Doesn't Matter", NULL, NULL);
    
    if(!window) {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    int frameWidth, frameHeight;

    glClearColor(1.0, 0.0, 1.0, 1.0);

    glewExperimental = true;
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        cout << "GLEW ERROR: ";
        cout << glewGetErrorString(err) << endl;
        glfwTerminate();
        exit(1);
    }

    GLuint vertID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);

    char const * vertPtr = vertCode.c_str();
    char const * fragPtr = fragCode.c_str();
    glShaderSource(vertID, 1, &vertPtr, NULL);
    glShaderSource(fragID, 1, &fragPtr, NULL);

    glCompileShader(vertID);
    glCompileShader(fragID);

    GLuint progID = glCreateProgram();
    glAttachShader(progID, vertID);
    glAttachShader(progID, fragID);

    glLinkProgram(progID);

    glDeleteShader(vertID);
    glDeleteShader(fragID);

    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertOnly.size()*sizeof(GLfloat),
                    vertOnly.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
   

    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

    GLuint EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(GLuint),
                         elements.data(), GL_STATIC_DRAW);  

    glBindVertexArray(0);



    // DRAWING / MAIN RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        
        glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
        glViewport(0,0,frameWidth,frameHeight);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(progID);

        // Draw stuff
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        this_thread::sleep_for(chrono::milliseconds(15));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &EBO);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);

    glUseProgram(0);
    glDeleteProgram(progID);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}