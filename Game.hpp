// Game.hpp

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <mutex>
#include <condition_variable>

class Game
{
public:
  Game();
  virtual ~Game();
  
  virtual int Run();
  void Init();
  void Finish();
  
  void CompileShader();
  void ConfigureVertex();
  void Render();
  
  void CreateFramebuffer();
  
  void set_request_stop(bool value) { request_stop_ = value; }
  GLuint tex() { return tex_[ tex_flip_flop_ ]; }
  bool render_full() { return render_full_; }
  void ClearRenderFull();
  void SwapBuffer();
  
protected:
  GLuint shader_program_, vao_;
  GLuint fb_, rb_, tex_[2];
  int tex_flip_flop_;
  bool request_stop_;
  int screen_width_, screen_height_;
  
  GLFWwindow *window_;
  
  std::mutex mtx_render_full_;
  bool render_full_;
  std::condition_variable cv_render_full_;
};
