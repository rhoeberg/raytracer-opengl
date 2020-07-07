#define GLEW_STATIC
#include "GL/glew.h"
#include <GLFW/glfw3.h>

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

#define MOUSE_SENSITIVITY 0.1
#define EPSILON 0.0001

// GLM Mathematics
// #include "glm/glm.hpp"
// #include "glm/gtc/matrix_transform.hpp"
// #include "glm/gtc/type_ptr.hpp"

#include <iostream>

#include "math.h"
#include "camera.h"
// #include "geometry.h"
// #include "raytracer.h"

#include "camera.cpp"
#include "math.cpp"
#include "geometry.cpp"
#include "raytracer.cpp"

const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
bool keys[1024];
    
static bool firstMouse = false;
static float lastX;
static float lastY;
static float mouseX;
static float mouseY;

static float camYaw;
static float camPitch;
static Camera cam;

static bool moving = false;
static bool movedLastFrame = true;
float moveStartTimer = 0.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    	glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS) {
        keys[key] = true;
    }
    else if(action == GLFW_RELEASE) {
        keys[key] = false;
    }
}  

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    moving = true;
    moveStartTimer = glfwGetTime();
    // mouseX = xpos;
    // mouseY = ypos;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;

    camYaw -= xoffset;
    camPitch += yoffset;
	
    if (camPitch > 89.0f)
	camPitch = 89.0f;
    if (camPitch < -89.0f)
	camPitch = -89.0f;
	
    Vec3 dir;
    dir.x = cos(RADIANS(camYaw)) * cos(RADIANS(camPitch));
    dir.y = sin(RADIANS(camPitch));
    dir.z = sin(RADIANS(camYaw)) * cos(RADIANS(camPitch));

    cam.g = Norm(dir);
    cam.w = cam.g * -1;
    cam.u = Norm(Cross(cam.t, cam.w));
    cam.v = Cross(cam.w, cam.u);
}
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

char* readFile(const char* fileName)
{
    char* result = 0;
    FILE* file = fopen(fileName, "r");
    if(file) {
        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        result = (char*)malloc(fileSize + 1);
        fread(result, fileSize, 1, file);
        result[fileSize] = 0;

        fclose(file);
    }
    else {
        printf("cannot find file %s\n", fileName);
    }

    return result;
}

GLuint create_shader(const char *vPath, const char *fPath)
{
    GLuint shaderProgram;
    const char* vertexCode = readFile(vPath);
    const char* fragmentCode = readFile(fPath);

    GLint success;
    GLchar infoLog[512];
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
        printf("%s\n", infoLog);
    }
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
        printf("%s\n", infoLog);
    }
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
	glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void init_glfw(GLFWwindow **win)
{
    glfwSetErrorCallback(error_callback);
    if(!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    *win = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "great concept", NULL, NULL);
    if(!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(*win);
    
    glfwSetCursorPosCallback(*win, mouse_callback);
    glfwSetKeyCallback(*win, key_callback);

    // Set this to true so glew knows to use modern opengl
    glewExperimental = GL_TRUE;
    glewInit();
    
    int vWidth, vHeight;
    glfwGetFramebufferSize(*win, &vWidth, &vHeight);
    glViewport(0, 0, vWidth, vHeight);
    
    // enable depths testing to remove pixels which is behind other pixels
    glEnable(GL_DEPTH_TEST);  
}

void create_quad(GLuint *VAO)
{
    GLfloat vertices[] = {
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    GLuint indices[] = {
        0, 1, 2,
        0, 3, 1
    };
    
    // GLuint VBO, VAO, EBO;
    GLuint VBO, EBO;
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
}

int main(int argc, char *argv[])
{
    GLFWwindow *win;
    init_glfw(&win);
    
    GLuint shaderProgram = create_shader("vertexshader.vs", "fragmentshader.frag");
    
    GLuint quad;
    create_quad(&quad);

    Vec3 data[480000];
    for(int i = 0; i < 480000; i++) {
	data[i] = VEC3(0,0,0);
    }
    
    // int i = 0;
    // for (int y = 0; y < SCREEN_HEIGHT; y++) {
    // 	for(int x = 0; x < SCREEN_WIDTH; x++) {
    // 	    printf("(%.1f %.1f %.1f) ", data[i].x, data[i].y, data[i].z);
    // 	    i++;
    // 	}
    // 	printf("\n");
    // }

    GLuint texture;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, &data);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    
    cam = CAMERA(VEC3(0,0,0), VEC3(0,0,-1), VEC3(0,1,0), 0.785);
    camPitch = asin(cam.g.y);
    camYaw = atan2(cam.g.x, cam.g.z);
    
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    

    float moveDuration = 0.5f;
    float moveStartTime = 0.0f;
    while(!glfwWindowShouldClose(win)) {

	if(glfwGetTime() > moveStartTimer + moveDuration) {
	    moving = false;
	}

        glfwPollEvents();
	
	
	if(keys[GLFW_KEY_A]) {
	    cam.e += cam.u * 0.1f;
	    moving = true;
	    moveStartTimer = glfwGetTime();
	}
	else if(keys[GLFW_KEY_D]) {
	    cam.e -= cam.u * 0.1f;
	    moving = true;
	    moveStartTimer = glfwGetTime();
	}
	else if(keys[GLFW_KEY_W]) {
	    cam.e += cam.g * 0.1f;
	    moving = true;
	    moveStartTimer = glfwGetTime();
	}
	else if(keys[GLFW_KEY_S]) {
	    cam.e -= cam.g * 0.1f;
	    moving = true;
	    moveStartTimer = glfwGetTime();
	}
        
	int i = 0;
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
	    for(int x = 0; x < SCREEN_WIDTH; x++) {
		if(moving) {
		    if(x % 4 == 0 && y % 4 == 0) {
			int yFlipped = SCREEN_HEIGHT - 1 - y;
			Ray ray = RayFor(cam, SCREEN_WIDTH, SCREEN_HEIGHT, x, yFlipped);
			data[i] = WorldHit(ray, true);
		    }
		    else {
			data[i] = VEC3(0,0,0);
		    }
		}
		else {
		    if(movedLastFrame) {
			int yFlipped = SCREEN_HEIGHT - 1 - y;
			Ray ray = RayFor(cam, SCREEN_WIDTH, SCREEN_HEIGHT, x, yFlipped);
			data[i] = WorldHit(ray);
		    }
		}
		i++;
	    }
	}
	movedLastFrame = moving;

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_FLOAT, &data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
        
        glUseProgram(shaderProgram);
	
	glBindTexture(GL_TEXTURE_2D, texture);


        glBindVertexArray(quad);
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glfwSwapBuffers(win);
    }

    glfwTerminate();

    return 0;
}
