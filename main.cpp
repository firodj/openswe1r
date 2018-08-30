// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "main.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <ctype.h>

#include "common.h"
#include "descriptor.h"
#include "emulation.h"
#include "exe.h"

//FIXME: Alternative for non-posix OS!
#include <time.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "com/d3d.h"
#include "com/ddraw.h"
#include "com/dinput.h"

uint32_t callId = 0;

unsigned int exportCount = 0;
Export* exports = NULL;
GLFWwindow* glfwWindow = NULL;

void AddExport(const char* name, ExportCallback callback, Address address) {
  exports = (Export*)realloc(exports, (exportCount + 1) * sizeof(Export));
  Export* export_sym = &exports[exportCount];
  export_sym->name = (const char*)malloc(strlen(name) + 1);
  strcpy((char*)export_sym->name, name);
  export_sym->callback = callback;
  export_sym->address = address;
  export_sym->thunkAddress = 0;
  export_sym->hook = 1;
  exportCount++;
}

void AddExport2(const char* name, ExportCallback callback) {
  exports = (Export*)realloc(exports, (exportCount + 1) * sizeof(Export));
  Export* export_sym = &exports[exportCount];
  export_sym->name = (const char*)malloc(strlen(name) + 1);
  strcpy((char*)export_sym->name, name);
  export_sym->callback = callback;
  export_sym->address = 0;
  export_sym->thunkAddress = 0;
  export_sym->hook = 2;
  exportCount++;
}

Export* LookupExportByName(const char* name) {
  for(unsigned int i = 0; i < exportCount; i++) {
    Export* export_sym = &exports[i];
    if (!strcmp(export_sym->name, name)) {
      return export_sym;
    }
  }
  return NULL;
}


// HACK BLOCK!

const char** dirlisting = NULL;

Address clearEax = 0;

uint32_t tls[1000] = {0};

//FIXME: To be moved elsewhere
#include "shaders.h"
#include "shader.h"


#include "windows.h" // Hack while exports are not ready
// HACK:
#include <unicorn/unicorn.h>

void UnknownImport(uc_engine* uc, Address address, void* user_data);

Address CreateInterface(const char* name, unsigned int slotCount, uint32_t objectSize) {
  //FIXME: Unsure about most terminology / inner workings here
  Address interfaceAddress = Allocate(objectSize);
  Address vtableAddress = Allocate(4 * slotCount);
  uint32_t* vtable = (uint32_t*)Memory(vtableAddress);
  for(unsigned int i = 0; i < slotCount; i++) {
    // Point addresses to themself
    char* slotName = (char*)malloc(128);
    sprintf(slotName, "%s__%d", name, i);
    Export* export_sym = LookupExportByName(slotName);

    Address hltAddress;
    if (export_sym != NULL) {
      if (export_sym->address == 0) {
        hltAddress = CreateHlt();
        AddHltHandler(hltAddress, export_sym->callback, (void*)slotName);
        export_sym->address = hltAddress;
      } else {
        hltAddress = export_sym->address;
      }
    } else {
      hltAddress = CreateHlt();
      AddHltHandler(hltAddress, UnknownImport, (void*)slotName);
      AddExport((const char*)slotName, UnknownImport, hltAddress);
    }
    vtable[i] = hltAddress;
  }
  // First element in object is pointer to vtable
  *(uint32_t*)Memory(interfaceAddress) = vtableAddress;

  return interfaceAddress;
}

Exe* exe; //FIXME: This is hack. I feel this shouldn't be exposed aside from the loader
const char* exeName = "swep1rcr.exe";

char* TranslatePath(const char* path) {
  size_t length = strlen(path) + 1;
  char* newPath = (char*)malloc(length);
  char* cursor = strcpy(newPath, path);
  while(*cursor != '\0') {
    if (*cursor == '\\') {
      *cursor = '/';
    } else {
      *cursor = tolower(*cursor);
    }
    cursor++;
  }

  // This is a patch for the original CD release.
  // We simply map the "/gnome/data/" folder from the CD back to our current
  // folders "/data/" folder.
  // This allows easier installation (by simply copying all files from disc)
  if ((length >= 14) && !memcmp(newPath, "d:/gnome/data/", 14)) {
    memcpy(newPath, "./data/", 7);
    memmove(&newPath[7], &newPath[14], length - 14);
    return newPath;
  }

  // This is another CD patch for some patched games.
  // They search CD contents directly in the CDs "/data/" path (instead of
  // "/gnome/data/"). This allows them to just set the "CD Path" to the
  // installation folder of the game.
  // The most prominent version doing this is probably the GOG.com re-release
  // from 2018.
  if ((length >= 8) && !memcmp(newPath, "d:/data/", 8)) {
    memcpy(newPath, "./data/", 7);
    memmove(&newPath[7], &newPath[8], length - 8);
    return newPath;
  }

  if ((length >= 10) && !memcmp(newPath, "d:/./data/", 10)) {
    memcpy(newPath, "./data/", 7);
    memmove(&newPath[7], &newPath[10], length - 10);
    return newPath;
  }

  return newPath;
}

void StackTrace(uint32_t base, unsigned int frames, unsigned int arguments) {
  uint32_t stackAddress = base;
  for(unsigned int i = 0; i < frames; i++) {
    info_printf("Base: 0x%" PRIX32 "\n", stackAddress);
    if (stackAddress == 0) {
      // End of stack trace!
      return;
    }
    uint32_t* stack = (uint32_t*)Memory(stackAddress);
    // stack[0] = EBP of calling function
    // stack[1] = Return address
    // stack[2..] = Arguments
    if (stack == NULL) {
      info_printf("Corrupt base in trace!\n");
      return;
    }
    info_printf("#%2d Returns to 0x%" PRIX32 " (", i, stack[1]);
    for(unsigned int j = 0; j < arguments; j++) {
      info_printf("@%d=0x%08" PRIX32 ", ", j, stack[j + 2]);
    }
    info_printf("...)\n");
    // Get the previous ebp
    stackAddress = stack[0];
  }
}



static void UcTimerHook(void* uc, uint64_t address, uint32_t size, void* user_data) {
  info_printf("Time is %" PRIu64 "\n", GetTicks());
}

// This is strictly for debug purposes, it attempts to dump fscanf (internally used by sscanf too)
static void UcFscanfHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  info_printf("\nfscanf\n\n");

  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);


  // This is the FILE struct used by microsoft CRT
  typedef struct {
    Address _ptr;
    int32_t _cnt;
    Address _base;
    int32_t _flag;
    int32_t _file;
    int32_t _charbuf;
    int32_t _bufsiz;
    Address _tmpfname;
  } _iobuf;

  // Pop the return address
  Address returnAddress = stack[0];
  info_printf("Return at 0x%" PRIX32 "\n", returnAddress);
  _iobuf* iob = (_iobuf*)Memory(stack[1]); // Get FILE object
  char* buf = (char*)Memory(iob->_ptr);
  info_printf("stream: 0x%" PRIX32 " ('%.100s...')\n", stack[1], buf);
  char* fmt = (char*)Memory(stack[2]);
  info_printf("fmt: 0x%" PRIX32 " ('%s')\n", stack[2], fmt);

  // We'll let MS code handle buffer loads
  if (buf == NULL) {
    return;
  }

#if 0
  if (!strcmp(fmt, "%s")) {
    SetTracing(true);
  }
#endif

#if 0
  //FIXME: Hack.. for these to work, they'd have to consume the buffer
  if (!strcmp(fmt, "%s")) {
    eax = sscanf(buf, fmt, Memory(stack[3]));
    uc_reg_write(uc, UC_X86_REG_EAX, &eax);
    eip = returnAddress;
    uc_reg_write(uc, UC_X86_REG_EIP, &eip);
    esp += 4;
    uc_reg_write(uc, UC_X86_REG_ESP, &esp);
    return;
  }
  if (!strcmp(fmt, "%f %f %f")) {
    eax = 3; //sscanf(buf, fmt);
    uc_reg_write(uc, UC_X86_REG_EAX, &eax);
    eip = returnAddress;
    uc_reg_write(uc, UC_X86_REG_EIP, &eip);
    esp += 4;
    uc_reg_write(uc, UC_X86_REG_ESP, &esp);
    return;
  }
#endif


}





// This is strictly for debug purposes, it attempts to add messages to the log in case of the weird-crash
static void UcCrashHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  int ecx;
  uc_reg_read(uc, UC_X86_REG_ECX, (void*)&ecx);
  int edi;
  uc_reg_read(uc, UC_X86_REG_EDI, (void*)&edi);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

  char buf[1024];
  sprintf(buf, "Bug: ecx=0x%08" PRIX32 ", at eip=0x%08" PRIX32 ". 0x%08" PRIX32 " 0x%08" PRIX32 " 0x%08" PRIX32 " 0x%08" PRIX32,
          ecx, eip,
          stack[0], stack[1], stack[2], stack[3]);
  glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, buf);
}


//Winmm.lib
HACKY_IMPORT_BEGIN(timeGetTime)
  //FIXME: Avoid overflow?
  eax = GetTicks();
HACKY_IMPORT_END()


// WTF.. why?! COMCTL ordinal import which does nothing
//Comctl32.lib
HACKY_IMPORT_BEGIN(InitCommonControls)
HACKY_IMPORT_END()


// CD check
//Mincore.lib
HACKY_IMPORT_BEGIN(GetFileVersionInfoSizeA)
  hacky_printf("lptstrFilename 0x%" PRIX32 " (%s)\n", stack[1], Memory(stack[1]));
  hacky_printf("lpdwHandle 0x%" PRIX32 "\n", stack[2]);

  if (stack[2]) {
    *(uint32_t*)Memory(stack[2]) = 0;
  }

  eax = 0; // Size of file version info [0 means error]
  esp += 2 * 4;
HACKY_IMPORT_END()



static void UcMallocHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

  // Pop the return address
  Address returnAddress = stack[0];
  info_printf("Return at 0x%" PRIX32 "\n", returnAddress);

  eax = Allocate(stack[1]);
  info_printf("malloc(%d) -> 0x%08X\n", stack[1], eax);

  uc_reg_write(uc, UC_X86_REG_EAX, (void*)&eax);
  eip = returnAddress;
  uc_reg_write(uc, UC_X86_REG_EIP, (void*)&eip);
  esp += 4;
  uc_reg_write(uc, UC_X86_REG_ESP, (void*)&esp);
}

static void UcFreeHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

  // Pop the return address
  Address returnAddress = stack[0];
  info_printf("Return at 0x%" PRIX32 "\n", returnAddress);

  info_printf("free(0x%08X)\n", stack[1]);
  Free(stack[1]);

  eax = 0;
  uc_reg_write(uc, UC_X86_REG_EAX, (void*)&eax);
  eip = returnAddress;
  uc_reg_write(uc, UC_X86_REG_EIP, (void*)&eip);
  esp += 4;
  uc_reg_write(uc, UC_X86_REG_ESP, (void*)&esp);
}


// Some TGA loading function

static void UcTGAHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

  // Pop the return address
  Address returnAddress = stack[0];
  info_printf("Return at 0x%" PRIX32 "\n", returnAddress);

  //int __cdecl sub_48A230(int a1, char *a2, _DWORD *a3, _DWORD *a4)
  info_printf("\n\n\n[ 48A230 ] TGAHook(0x%08X, 0x%08X, 0x%08X, 0x%08X)\n\n\n\n", stack[1], stack[2], stack[3], stack[4]);

  // Emulate instruction we overwrote
  eax = stack[1];
  uc_reg_write(uc, UC_X86_REG_EAX, (void*)&eax);
  eip = 0x48a234;
  uc_reg_write(uc, UC_X86_REG_EIP, (void*)&eip);
}



// Generic hook

// Callback for tracing instructions

void UnknownImport(uc_engine* uc, Address address, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

#if 0
  // This lists a stack trace.
  // It's a failed attempt because most functions omit the frame pointer
  int ebp;
  uc_reg_read(uc, UC_X86_REG_EBP, &ebp);
  StackTrace(ebp, 10, 4);
#endif

  // Pop the return address
  Address returnAddress = stack[0];
  eip = returnAddress;
  esp += 4;
  
  sys_printf("\nUnknown function!\n\n");

  sys_printf("Stack at 0x%" PRIX32 "; returning EAX: 0x%08" PRIX32 "\n", stackAddress, eax);
  sys_printf("%7" PRIu32 " Emulation at %X ('%s') from %X\n\n", callId, eip, (char*)user_data, returnAddress);

  callId++;

  assert(false);

  uc_reg_write(uc, UC_X86_REG_ESP, (void*)&esp);
  uc_reg_write(uc, UC_X86_REG_EIP, (void*)&eip);
  uc_reg_write(uc, UC_X86_REG_EAX, (void*)&eax);
}



//FIXME: Abstract exe mapping and context creation from emu kickoff
void RunX86(Exe* exe) {

  // Map the important exe parts into emu memory
  for(unsigned int sectionIndex = 0; sectionIndex < exe->coffHeader.numberOfSections; sectionIndex++) {
    PeSection* section = &exe->sections[sectionIndex];
    void* mappedSection = (void*)exe->mappedSections[sectionIndex];
    if (mappedSection != NULL) {
      uint32_t base = exe->peHeader.imageBase + section->virtualAddress;
      sys_printf("Mapping 0x%" PRIX32 " - 0x%" PRIX32 "\n", base, base + section->virtualSize - 1);
      MapMemory(mappedSection, base, AlignUp(section->virtualSize, exe->peHeader.sectionAlignment), true, true, true);
    }
  }

  //FIXME: Schedule a virtual main-thread
  sys_printf("Emulation starting\n");
  CreateEmulatedThread(exe->peHeader.imageBase + exe->peHeader.addressOfEntryPoint, false);
  RunEmulation();

  CleanupEmulation();
}

static void glfw_error_callback(int error, const char* description)
{
    sys_printf("Glfw Error %d: %s\n", error, description);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(int argc, char* argv[]) {
  sys_printf("-- Initializing\n");
  InitializeEmulation();
  
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    sys_printf("Failed to initialize GLFW!\n");
    return 1;
  }
 
  {
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    sys_printf("Running against GLFW %i.%i.%i\n", major, minor, revision);
  }
    
  sys_printf("-- Creating window\n");
  {
    bool fullscreen = false;
    int w = 640;
    int h = 480;

  	//Uint32 style = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN;
    //if (fullscreen) {
    //  style |= SDL_WINDOW_FULLSCREEN;
    //}

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
      
    //sdlWindow = SDL_CreateWindow("OpenSWE1R", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, style);
    //assert(sdlWindow != NULL);
      
    glfwWindow = glfwCreateWindow(w, h, "OpenSWE1R", NULL, NULL);
    assert(glfwWindow != NULL);
      
    glfwSetKeyCallback(glfwWindow, glfw_key_callback);
      
    glfwMakeContextCurrent(glfwWindow);

    //SDL_GLContext glcontext = SDL_GL_CreateContext(sdlWindow);
    //assert(glcontext != NULL);

    //gladLoadGLLoader(SDL_GL_GetProcAddress);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    sys_printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    sys_printf("Renderer: %s\n", glGetString(GL_RENDERER));
    sys_printf("Version:  %s\n", glGetString(GL_VERSION));

    //FIXME: This is ugly but gets the job done.. for now
    static GLuint vao = 0;
    if (vao == 0) {
      glGenVertexArrays(1, &vao);
    }
    glBindVertexArray(vao);


    glDisable(GL_CULL_FACE);
//    glDepthFunc(GL_GEQUAL);
    glCullFace(GL_FRONT);

  	//SDL_ShowWindow(sdlWindow);
  }
  sys_printf("-- Compiling shaders\n");
  GLuint shader1Texture = 0;
  {
    GLuint vertexShader = CreateShader(VertexShader1Texture, GL_VERTEX_SHADER);
    GLuint fragmentShader = CreateShader(FragmentShader1Texture, GL_FRAGMENT_SHADER);
    shader1Texture = CreateShaderProgram(vertexShader, fragmentShader);
  }
  bool linked = LinkShaderProgram(shader1Texture);
  PrintShaderProgramLog(shader1Texture);
  assert(linked);
  glUseProgram(shader1Texture); //FIXME: Hack..
  sys_printf("-- Loading exe\n");
  
  Exe *exe = LoadExe(exeName);
  if (exe == NULL) {
    sys_printf("Couldn't load '%s'\n", exeName);
    exit(EXIT_FAILURE);
  }
  RelocateExe(exe);

  // Attempt to identify the game version using the COFF timestamp
  if (exe->coffHeader.timeDateStamp == 0x3727ce0e) {
    sys_printf("Game version: Retail, English\n");
  } else if (exe->coffHeader.timeDateStamp == 0x3738c552) {
    sys_printf("Game version: Retail, German\n"); // International?
  } else if (exe->coffHeader.timeDateStamp == 0x37582659) {
    sys_printf("Game version: Webdemo, English\n");
  } else if (exe->coffHeader.timeDateStamp == 0x3c60692c) {
    sys_printf("Game version: Patched, English\n");
  } else if (exe->coffHeader.timeDateStamp == 0x3c6321d1) {
    sys_printf("Game version: Patched, International\n");
  } else {
    sys_printf("Game version: Unknown (COFF timestamp: 0x%08X)\n", exe->coffHeader.timeDateStamp);
    assert(false);
  }

  clearEax = Allocate(3);
  uint8_t* p = (uint8_t*)Memory(clearEax);
  *p++ = 0x31; *p++ = 0xC0; // xor eax, eax
  *p++ = 0xC3;              // ret

// 0x90 = nop (used to disable code)
// 0xC3 = ret (used to skip function)
// 0x84 = je (probably used to be `jne`, used to invert condition)
// 0x75 = jne (probably used to be `je`, used to invert condition)

// These functions access internal FILE* data I belive; crashes our emu
#if 0
*(uint8_t*)Memory(0x4A1670) = 0xC3; // _lock
*(uint8_t*)Memory(0x4A16F0) = 0xC3; // _unlock
*(uint8_t*)Memory(0x4A1710) = 0xC3; // _lock_file
*(uint8_t*)Memory(0x4A1780) = 0xC3; // _unlock_file
#endif

#if 0 //FIXME FIXME FIXME FIXME FIXME
  // These do something bad internally
  CreateBreakpoint(0x49f270, UcMallocHook, "<malloc>");
  CreateBreakpoint(0x49f200, UcFreeHook, "<free>");

  // This function used to crash with SIGSEGV, so I wanted to peek at the parameters.
  CreateBreakpoint(0x48A230, UcTGAHook, "<TGAHook>");
#endif

#if 0
*(uint8_t*)Memory(0x487d71) = 0x75; // Invert the check for eax after "DirectDrawEnumerate" (ours will always fail)
*(uint8_t*)Memory(0x488ce2) = 0x75; // Invert the check for eax after "EnumDisplayModes" (ours will always fail)
*(uint8_t*)Memory(0x489e20) = 0x75; // Invert the check for eax after "EnumDevices" [graphics] (ours will always fail)
*(uint8_t*)Memory(0x48a013) = 0x84; // Invert the check for eax after "EnumTextureFormats" (ours will always fail)
*(uint8_t*)Memory(0x485433) = 0x75; // Invert the check for eax after "EnumDevices" [input] (ours will always fail)
#endif

//memset(Memory(0x423cd9), 0x90, 5); // Disable command line arg scanning

  sys_printf("-- Switching mode\n");
  RunX86(exe);
  sys_printf("-- Exiting\n");
  UnloadExe(exe);
  
  glfwDestroyWindow(glfwWindow);
  glfwTerminate();
    
  return EXIT_SUCCESS;
}
