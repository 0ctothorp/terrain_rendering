#include <iostream>
#include <vector>
#include <memory>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void GetFirstNMessages(GLuint numMsgs) {
    GLint maxMsgLen = 0;
    glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

    std::vector<GLchar> msgData(numMsgs * maxMsgLen);
    std::vector<GLenum> sources(numMsgs);
    std::vector<GLenum> types(numMsgs);
    std::vector<GLenum> severities(numMsgs);
    std::vector<GLuint> ids(numMsgs);
    std::vector<GLsizei> lengths(numMsgs);

    GLuint numFound = glGetDebugMessageLog(numMsgs, 1000, &sources[0], &types[0], &ids[0], 
                                           &severities[0], &lengths[0], &msgData[0]);

    sources.resize(numFound);
    types.resize(numFound);
    severities.resize(numFound);
    ids.resize(numFound);
    lengths.resize(numFound);

    std::vector<std::string> messages;
    messages.reserve(numFound);

    std::vector<GLchar>::iterator currPos = msgData.begin();
    for(size_t msg = 0; msg < lengths.size(); ++msg) {
        messages.push_back(std::string(currPos, currPos + lengths[msg] - 1));
        currPos = currPos + lengths[msg];
        std::cout << "gl debug: " << messages[msg] << '\n';
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if(key == -1) {
        std::cerr << "Pressed unknown key\n";
        return;
    }

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW ERROR: " << description << '\n';
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

}

GLFWwindow* GetGLFWwindow(int &w, int &h, const char *name){
    GLFWwindow* window;
    // Initialise GLFW
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        exit(-1);
    }

    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (w == 0 || h == 0) {
        // if 0 than WINDOWLESS FULL SCREEN :
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(mode->width, mode->height, name, monitor, nullptr);
        w=mode->width; h=mode->height;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        // Open a window and create its OpenGL context
        window = glfwCreateWindow(w, h, name, nullptr, nullptr);
    }    

    if(window == nullptr) {
        std::cerr << "Failed to open GLFW window. .... \n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);    

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {        
        std::cerr << "Failed to initialize GLEW\n";
        exit(-1);
    }    

    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    return window;
}

class Plane {
private:
    int width;
    int height;
    std::vector<glm::vec3> vertices;
    std::vector<int> indices;

    void calc_vertices_positions() {
        int pos = 0;
        for(int i = 0; i <= width; i++) {
            for(int j = 0; j <= height; j++) {
                vertices[pos].x = j;
                vertices[pos].y = i;
                vertices[pos].z = 0;
                pos++;
            }
        }

        for(uint i = 0; i < vertices.size(); i++)
            std::cout << i << ": " << vertices[i].x << ", " << vertices[i].y << ", " 
                      << vertices[i].z << "\n";

        std::cout << std::endl;
    }

    void calc_indices() {
        indices.push_back(0);
        for(uint i = 0; i < vertices.size() - width - 1; i++) {
            if(i % (width + 1) == (uint)width) {
                indices.push_back(i + width + 1);
                indices.push_back(i + width + 1);
                if(i < vertices.size() - width - 2) {
                    indices.push_back(i + 1);
                    indices.push_back(i + 1);
                }
            } else {
                indices.push_back(i + width + 1);
                indices.push_back(i + 1);
            }
        }

        for(int i: indices) {
            std::cout << i << ", ";
        }
        std::cout << std::endl;
    }

public:
    Plane(int _width = 5, int _height = 5) : width(_width), height(_height) {
        vertices.resize((width + 1) * (height + 1));
        calc_vertices_positions();
        calc_indices();
    }
};

int main() {
    int w = 640, h = 480;
    auto window = GetGLFWwindow(w, h, "OpenGL terrain rendering");

    glEnable(GL_DEBUG_OUTPUT);
    GetFirstNMessages(10);

    Plane plane(3, 3);

    while(glfwWindowShouldClose(window) == 0) {     

        glfwPollEvents();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);   
    }

    glfwTerminate();
}
