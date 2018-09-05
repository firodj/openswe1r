// File: Application.hpp

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class Game;

class Application {
public:
  void Init();
  void Finish();
  void Run(Game* game);
  
  void MakeContextCurrent();
  static void CheckGLError(const char *file, int line);
  
  static void OnErrorCallback(int error, const char* description);
  static void OnKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void OnFramebufferSizeCallback(GLFWwindow* window, int width, int height);
  
  static Application* Get();

  GLFWwindow* window() { return window_; }
  
  bool LockGL(int ms);
  void UnlockGL();
  
  Game* game() { return game_; }
  
private:
  GLFWwindow *window_;
  GLuint quad_shader_program_, quad_vao_, quad_vbo_;
  Game *game_;
  
  int window_width_, window_height_;
  int screen_width_, screen_height_;
};
