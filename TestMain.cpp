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

    this_thread::sleep_for(chrono::milliseconds(15));


    return 0;
}