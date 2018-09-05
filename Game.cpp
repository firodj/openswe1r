// Game.cpp

#include "Application.hpp"
#include "Game.hpp"
#include <iostream>
#include <glad/glad.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 view;\n"
    "uniform mat4 model;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
    "}";

static const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}";

Game::Game(): shader_program_(-1), fb_(-1), rb_(-1), tex_flip_flop_(0),
  request_stop_(false), render_full_(false)
{
  tex_[0] = -1;
  tex_[1] = -1;
  std::cout << "Game constructed" << std::endl;
}

Game::~Game()
{
  std::cout << "Game destroyed" << std::endl;
}

void Game::Init(int w, int h)
{
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.3+ only
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
  window_width_ = w;
  window_height_ = h;
  window_ = glfwCreateWindow(window_width_, window_height_, "OpenSWE1R", NULL, Application::Get()->window());
  glfwGetFramebufferSize(window_, &screen_width_, &screen_height_);
  
  hidpi_x_ = (float)screen_width_ / w;
  hidpi_y_ = (float)screen_height_ / h;
  
  CreateFramebuffer();
}

void Game::Finish()
{
  glfwDestroyWindow(window_);
}

void Game::CreateFramebuffer()
{
  // framebuffer configuration
  glGenFramebuffers(1, &fb_);
  glBindFramebuffer(GL_FRAMEBUFFER, fb_);
  
  // create a color attachment texture
  glGenTextures(2, tex_);
  
  glBindTexture(GL_TEXTURE_2D, tex_[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width_, screen_height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  glBindTexture(GL_TEXTURE_2D, tex_[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width_, screen_height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  int tex_offscreen = (tex_flip_flop_ + 1) % 2;
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_[tex_offscreen], 0);
  
  // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  glGenRenderbuffers(1, &rb_);
  glBindRenderbuffer(GL_RENDERBUFFER, rb_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width_, screen_height_);

  // use a single renderbuffer object for both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb_);
  // now actually attach it

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::CompileShader()
{
  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  
  // link shaders
  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  
  shader_program_ = shaderProgram;
}

void Game::ConfigureVertex()
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
       0.5f,  0.5f, -0.0f,  // top right
       0.5f, -0.5f, -0.0f,  // bottom right
      -0.5f, -0.5f, -0.0f,  // bottom left
      -0.5f,  0.5f, -0.0f   // top left
  };
  unsigned int indices[] = {  // note that we start from 0!
      0, 1, 3,  // first Triangle
      1, 2, 3   // second Triangle
  };
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);
  vao_ = VAO;

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  //glBindBuffer(GL_ARRAY_BUFFER, 0);

  // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  
  glBindVertexArray(0);
}

void Game::Render()
{
  glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)1024 / (float)768, 0.1f, 100.0f);
  glm::mat4 view(1.0);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  glm::mat4 model(1.0);
  model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shader_program_);
  glUniformMatrix4fv(glGetUniformLocation(shader_program_, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
  glUniformMatrix4fv(glGetUniformLocation(shader_program_, "view"), 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(glGetUniformLocation(shader_program_, "model"), 1, GL_FALSE, glm::value_ptr(model));
  Application::CheckGLError(__FILE__, __LINE__);
  
  glBindVertexArray(vao_); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  //glDrawArrays(GL_TRIANGLES, 0, 6);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  // glBindVertexArray(0); // no need to unbind it every time
}

GLuint Game::tex() {
  std::unique_lock<std::mutex> lck(mtx_render_full_);
  return tex_[ tex_flip_flop_ ];
}

void Game::SwapBuffer()
{
  std::unique_lock<std::mutex> lck(mtx_render_full_);
  /*{
   
      while (render_full_ && !request_stop_) {
        if (cv_render_full_.wait_for(lck, std::chrono::milliseconds(16)) == std::cv_status::timeout) ;
      }
      if (request_stop_) break;
    }
  */
    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
  
  tex_flip_flop_ = (tex_flip_flop_ + 1) % 2;
  int tex_offscreen = (tex_flip_flop_ + 1) % 2;
  
  glBindFramebuffer(GL_FRAMEBUFFER, fb_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_[tex_offscreen], 0);
  
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void Game::ClearRenderFull()
{
  if (render_full_) {
    render_full_ = false;
    cv_render_full_.notify_one();
  }
}

int Game::Run()
{
  std::cout << "Game thread start" << std::endl;
  glfwMakeContextCurrent(window_);
  
  CompileShader();
  ConfigureVertex();
  
  glBindFramebuffer(GL_FRAMEBUFFER, fb_);
  glViewport(0, 0, screen_width_, screen_height_);
  
  while (!request_stop_) {
    
    Render();
    SwapBuffer();
    
    render_full_ = true;
  }
  return 0;
}
