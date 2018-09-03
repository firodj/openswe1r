// StarWars.cpp

#include "StarWars.hpp"

#include <iostream>

StarWars::StarWars()
{

}

StarWars::~StarWars()
{

}
  
int StarWars::Run()
{
  std::cout << "Game thread start" << std::endl;
  glfwMakeContextCurrent(window_);
  
  // Initialize
  
  while (!request_stop_) {
    // Processing ...
    
    {
      std::unique_lock<std::mutex> lck(mtx_render_full_);
      while (render_full_ && !request_stop_) {
        if (cv_render_full_.wait_for(lck, std::chrono::milliseconds(16)) == std::cv_status::timeout) ;
      }
      if (request_stop_) break;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, fb_);
    glViewport(0, 0, screen_width_, screen_height_);
  
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glFlush();
  
    render_full_ = true;
  }
  
  return 0;
}

