// StarWars.cpp

#include "StarWars.hpp"
#include <unicorn/unicorn.h>

#include "common.h"
#include "emulation.h"
#include "shader.h"
#include "shaders.h"
#include "exe.h"
#include <iostream>

Exe* exe; //FIXME: This is hack. I feel this shouldn't be exposed aside from the loader
const char* exeName = "swep1rcr.exe";
Address clearEax = 0;

StarWars::StarWars()
{

}

StarWars::~StarWars()
{

}

void StarWars::CompileShader()
{
  //FIXME: This is ugly but gets the job done.. for now
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glDisable(GL_CULL_FACE);
  //  glDepthFunc(GL_GEQUAL);
  glCullFace(GL_FRONT);

  std::cout << "-- Compiling shaders" << std::endl;
  
  GLuint shader1Texture = 0;
  {
    GLuint vertexShader = CreateShader(VertexShader1Texture, GL_VERTEX_SHADER);
    GLuint fragmentShader = CreateShader(FragmentShader1Texture, GL_FRAGMENT_SHADER);
    shader1Texture = CreateShaderProgram(vertexShader, fragmentShader);
  }
  
  bool linked = LinkShaderProgram(shader1Texture);
  PrintShaderProgramLog(shader1Texture);
  assert(linked);
  
  shader_program_ = shader1Texture;
  glUseProgram(shader_program_); //FIXME: Hack..
}

int StarWars::Run()
{
  std::cout << "StarWars thread start" << std::endl;
  glfwMakeContextCurrent(window_);
  
  std::cout << "-- Initializing" << std::endl;

  InitializeEmulation(reinterpret_cast<void*>(this));

  CompileShader();
  
  // Initialize
  
  std::cout << "-- Loading exe" << std::endl;
  
  Exe *exe = LoadExe(exeName);
  if (exe == NULL) {
    std::cout << "Couldn't load " << exeName << std::endl;
    exit(EXIT_FAILURE);
  }
  RelocateExe(exe);

  // Attempt to identify the game version using the COFF timestamp
  if (exe->coffHeader.timeDateStamp == 0x3727ce0e) {
    std::cout << "Game version: Retail, English" << std::endl;
  } else if (exe->coffHeader.timeDateStamp == 0x3738c552) {
    std::cout << "Game version: Retail, German" << std::endl; // International?
  } else if (exe->coffHeader.timeDateStamp == 0x37582659) {
    std::cout << "Game version: Webdemo, English" << std::endl;
  } else if (exe->coffHeader.timeDateStamp == 0x3c60692c) {
    std::cout << "Game version: Patched, English" << std::endl;
  } else if (exe->coffHeader.timeDateStamp == 0x3c6321d1) {
    std::cout << "Game version: Patched, International" << std::endl;
  } else {
    std::cout << "Game version: Unknown (COFF timestamp: " << exe->coffHeader.timeDateStamp << ")" << std::endl;
    assert(false);
  }
  
  clearEax = Allocate(3);
  uint8_t* p = (uint8_t*)Memory(clearEax);
  *p++ = 0x31; *p++ = 0xC0; // xor eax, eax
  *p++ = 0xC3;              // ret
  
  // Map the important exe parts into emu memory
  for(unsigned int sectionIndex = 0; sectionIndex < exe->coffHeader.numberOfSections; sectionIndex++) {
    PeSection* section = &exe->sections[sectionIndex];
    void* mappedSection = (void*)exe->mappedSections[sectionIndex];
    if (mappedSection != NULL) {
      uint32_t base = exe->peHeader.imageBase + section->virtualAddress;
      std::cout << "Mapping " << base << " - " << (base + section->virtualSize - 1) << std::endl;
      MapMemory(mappedSection, base, AlignUp(section->virtualSize, exe->peHeader.sectionAlignment), true, true, true);
    }
  }

  //FIXME: Schedule a virtual main-thread
  std::cout << "Emulation starting" << std::endl;
  
  CreateEmulatedThread(exe->peHeader.imageBase + exe->peHeader.addressOfEntryPoint, false);

  glBindFramebuffer(GL_FRAMEBUFFER, fb_);
  glViewport(0, 0, screen_width_, screen_height_);

  while (!request_stop_) {
    // Processing ...
    bool ret = StepEmulation(reinterpret_cast<void*>(this));
    if (!ret) break;
  
    //render_full_ = true;
  }
  
  CleanupEmulation();
  
  UnloadExe(exe);
  
  return 0;
}

// 0x90 = nop (used to disable code)
// 0xC3 = ret (used to skip function)
// 0x84 = je (probably used to be `jne`, used to invert condition)
// 0x75 = jne (probably used to be `je`, used to invert condition)

// These functions access internal FILE* data I belive; crashes our emu
/**
*(uint8_t*)Memory(0x4A1670) = 0xC3; // _lock
*(uint8_t*)Memory(0x4A16F0) = 0xC3; // _unlock
*(uint8_t*)Memory(0x4A1710) = 0xC3; // _lock_file
*(uint8_t*)Memory(0x4A1780) = 0xC3; // _unlock_file
*/

/**
//FIXME FIXME FIXME FIXME FIXME
  // These do something bad internally
  CreateBreakpoint(0x49f270, UcMallocHook, "<malloc>");
  CreateBreakpoint(0x49f200, UcFreeHook, "<free>");

  // This function used to crash with SIGSEGV, so I wanted to peek at the parameters.
  CreateBreakpoint(0x48A230, UcTGAHook, "<TGAHook>");
*/

/**
*(uint8_t*)Memory(0x487d71) = 0x75; // Invert the check for eax after "DirectDrawEnumerate" (ours will always fail)
*(uint8_t*)Memory(0x488ce2) = 0x75; // Invert the check for eax after "EnumDisplayModes" (ours will always fail)
*(uint8_t*)Memory(0x489e20) = 0x75; // Invert the check for eax after "EnumDevices" [graphics] (ours will always fail)
*(uint8_t*)Memory(0x48a013) = 0x84; // Invert the check for eax after "EnumTextureFormats" (ours will always fail)
*(uint8_t*)Memory(0x485433) = 0x75; // Invert the check for eax after "EnumDevices" [input] (ours will always fail)
**/

//memset(Memory(0x423cd9), 0x90, 5); // Disable command line arg scanning

