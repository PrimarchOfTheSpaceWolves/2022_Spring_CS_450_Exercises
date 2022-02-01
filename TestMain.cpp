#include <iostream>
#include <cstring>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;

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

    // DRAWING / MAIN RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        
        glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
        glViewport(0,0,frameWidth,frameHeight);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        glfwSwapBuffers(window);
        glfwPollEvents();
        this_thread::sleep_for(chrono::milliseconds(15));
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}