#ifndef mandel_julia
#define mandel_julia

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sys/stat.h>
#include <map>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include <complex>
//#define M_PI           3.14159265358979323846

static void error_callback(int error, const char *description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

double cx = -2.5, cy = 1.5, zoom = 0.156729;
double C_re, C_im; //Julia parameters
int counter;
int itr = 256;
int fps = 0;
uint64_t index_counter = 0;

GLFWwindow *window = nullptr;

int w = 640;
int h = 480;

GLuint program;
GLuint shader;

double last_time = 0, current_time = 0;
unsigned int ticks = 0;

bool keys[1024] = { 0 };

std::map<int, int> frequency_buckets;

static void cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    double xr = 2.0 * (xpos / (double)w - 0.5);
    double yr = 2.0 * (ypos / (double)h - 0.5);

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        cx += (xr - cx) / zoom / 2.0;
        cy -= (yr - cy) / zoom / 2.0;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    zoom += yoffset * 0.1 * zoom;
    if(zoom < 0.1) {
        zoom = 0.1;
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const double d = 0.1 / zoom;

    if(action == GLFW_PRESS) {
        keys[key] = true;
    } else if(action == GLFW_RELEASE) {
        keys[key] = false;
    }

    if(keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, 1);
    } else if(keys[GLFW_KEY_A]) {
        cx -= d;
    } else if(keys[GLFW_KEY_D]) {
        cx += d;
    } else if(keys[GLFW_KEY_W]) {
        cy += d;
    } else if(keys[GLFW_KEY_S]) {
        cy -= d;
    } else if(keys[GLFW_KEY_MINUS]) {
        itr += 10;
    } else if(keys[GLFW_KEY_EQUAL]) {
        itr -= 10;
        if(itr <= 0) {
            itr = 0;
        }
    }

}

const char* vertex_shader =
        "#version 410\n"
                "in vec3 vp;"
                "void main () {"
                "  gl_Position = vec4 (vp, 1.0);"
                "}";

static void update_window_title()
{
    std::ostringstream ss;
    ss << "Alex Saad-Falcon ";
    ss << ", FPS: " << fps;
    ss << ", Iterations: " << itr;
    ss << ", Zoom: " << zoom;
    ss << ", At: (" << std::setprecision(8) << C_re << " + " << C_im << " + " << counter;
    glfwSetWindowTitle(window, ss.str().c_str());
}

static void compile_shader(GLuint &prog)
{
    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);

    std::ifstream t("../complex_shader.glsl");
    if(!t.is_open()) {
        std::cerr << "Cannot open complex_shader.glsl!" << std::endl;
        return;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    const char *src  = str.c_str();

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &src, NULL);
    glCompileShader (fs);

    int success;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success) {
        int s;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &s);

        char *buf = new char[s];
        glGetShaderInfoLog(fs, s, &s, buf);

        std::cerr << buf << std::endl;
        delete [] buf;
        return;
    }

    prog = glCreateProgram ();
    glAttachShader (prog, fs);
    glAttachShader (prog, vs);
    glLinkProgram (prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if(!success) {
        int s;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &s);

        char *buf = new char[s];
        glGetProgramInfoLog(prog, s, &s, buf);

        std::cerr << buf << std::endl;
        delete [] buf;
        return;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

static time_t last_mtime;

static time_t get_mtime(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        perror(path);
        exit(1);
    }
    return statbuf.st_mtime;
}

int render(float coeff_float_arr[], float real_arr[], float imag_arr[],
           float  * coeff_float_max, int num_coeff) {
//int mandel_julia_renderer(short * samples, u_int start, u_int num_samples) {
    if(!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return 1;
    }

    atexit(glfwTerminate);

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(w, h, "Mandelbrot", NULL, NULL);
    if(!window) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_FALSE);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    //std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    //std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    GLuint prog;
    compile_shader(prog);

    last_mtime = get_mtime("../complex_shader.glsl");

    float points[] = {
            -1.0f,  1.0f,  0.0f,
            -1.0f,  -1.0f,  0.0f,
            1.0f,  -1.0f,  0.0f,

            -1.0f,  1.0f,  0.0f,
            1.0f,  -1.0f,  0.0f,
            1.0f,  1.0f,  0.0f,
    };

    GLuint vbo = 0;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 2 * 9 * sizeof (float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("../texture_vs.glsl",
                     "../texture_fs.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // positions          // colors           // texture coords
            -0.5f, 0.80f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // bottom right
            -0.50f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // top right
            -1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // top left
            -1.0f,  0.80f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // bottom left
    };
    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // load and create a texture
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //int width, height, nrChannels;
    //unsigned char *data = stbi_load("../name.png", &width, &height, &nrChannels, 0);
    //if (data)
    //{
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //}
    //else
    //{
    //    std::cout << "Failed to load texture" << std::endl;
    //}
    //stbi_image_free(data);

    glUseProgram (prog);

    last_time = glfwGetTime();

    //glBindVertexArray (vao);
    //glBindVertexArray(VAO);

    while(!glfwWindowShouldClose(window)) {
        time_t new_time = get_mtime("../complex_shader.glsl");
        if(new_time != last_mtime) {
            glDeleteProgram(prog);
            compile_shader(prog);
            glUseProgram(prog);
            last_mtime = new_time;

            std::cout << "Reloaded shader: " << last_mtime << std::endl;
        }

        glfwGetWindowSize(window, &w, &h);
        glUniform2d(glGetUniformLocation(prog, "screen_size"), (double)w, (double)h);
        glUniform1d(glGetUniformLocation(prog, "screen_ratio"), (double)w / (double)h);
        glUniform1d(glGetUniformLocation(prog, "C_re"), C_re);
        glUniform1d(glGetUniformLocation(prog, "C_im"), C_re);
        glUniform2d(glGetUniformLocation(prog, "center"), cx, cy);
        glUniform1d(glGetUniformLocation(prog, "zoom"), zoom);
        glUniform1i(glGetUniformLocation(prog, "itr"), itr);
        glUniform1i(glGetUniformLocation(prog, "num_coeff"), num_coeff);
        glUniform1fv(glGetUniformLocation(prog, "coeff_float_arr"), num_coeff, coeff_float_arr);
        //glUniform1fv(glGetUniformLocation(prog, "real_arr"), num_coeff / 2, real_arr);
        //glUniform1fv(glGetUniformLocation(prog, "imag_arr"), num_coeff / 2, imag_arr);
        glUniform1d(glGetUniformLocation(prog, "coeff_float_max"), *coeff_float_max);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glUseProgram(prog);
        glBindVertexArray(vao);
        glDrawArrays (GL_TRIANGLES, 0, 6);

        ourShader.use();
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glUseProgram(prog);

        glfwSwapBuffers(window);
        glfwPollEvents();

        int period = 500;
        double theta;
        ticks++;
        current_time = glfwGetTime();
        C_re = -.6;
        C_im = -.6;
        double C_re_old = -.6, C_im_old = -.6;

        uint64_t avg_size = 8000;
        float moving_avg_real[avg_size];
        float moving_avg_imag[avg_size];
        for (int i = 0; i < avg_size; ++i) {
            moving_avg_real[i] = 0;
            moving_avg_imag[i] = 0;
        }
        if(current_time - last_time > .002) {
            //std::cout << *coeff_float_max << std::endl;
            fps = ticks;
            update_window_title();
            last_time = glfwGetTime();
            ticks = 0;
            //C_re = std::cos(((double)counter*2*M_PI)/(double)period);
            //C_im = std::sin(((double)counter*2*M_PI)/(double)period);
            float mag = 0, old_mag = 0, alpha = .4, beta = 1-alpha;
            for (int i = 0; i < num_coeff; ++i) {
                mag += coeff_float_arr[i] * coeff_float_arr[i]; // sum magnitude squared
                //mag += coeff_float_arr[i] * coeff_float_arr[i];
            }
            mag = alpha * mag * .7885 / 400000000.0 / float(num_coeff) + beta * old_mag;
            //std::cout << "Magnitude : " << mag << std::endl;
            old_mag = mag;
            itr = mag;
            std::cout << "MAG SQUARED " << mag << std::endl;
            theta = std::rand() * 2 * M_PI;

            float real_sum = 0, imag_sum = 0;
            for (int i = 0; i < 5000; i++) {
                real_sum += real_arr[i];
                imag_sum += imag_arr[i];
            }
            real_sum /= 5000;
            imag_sum /= 5000;
            //real_sum = 1 - real_sum;
            //imag_sum = 1 - imag_sum;
            std::cout << "real_sum" << real_sum << std::endl;
            std::cout << "imag_sum" << imag_sum << std::endl;
            moving_avg_real[index_counter] = real_sum;
            moving_avg_imag[index_counter] = imag_sum;
            index_counter = (index_counter + 1) % avg_size;
            std::cout << index_counter << std::endl;
            float avg_real = 0, avg_imag = 0;
            for (int i = 0; i < avg_size; ++i) {
                avg_real += moving_avg_real[i];
                avg_imag += moving_avg_imag[i];
                //std::cout << "mvg avg real : " << moving_avg_real[i] << std::endl;
                //std::cout << "mvg avg imag : " << moving_avg_imag[i] << std::endl;
            }
            avg_real /= avg_size;
            avg_imag /= avg_size;

            C_re = avg_real;
            C_im = avg_imag;

            //C_re = (1 - real_sum) / 2.5;
            //C_im = (1 - imag_sum) / 2.5;
            std::cout << "C_re : " << C_re << std::endl;
            std::cout << "C_im : " << C_im << std::endl;
            //C_re = 0.4;
            counter = (counter + 1) % period;
        }
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);


    glfwDestroyWindow(window);
}

#endif