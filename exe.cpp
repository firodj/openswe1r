#include "main.h"
#include "windows.h"
#include "export.h"
#include "emulation.h"
#include "common.h"
#include <string.h>
#include <assert.h>
#include "exe.h"

void UnknownImport(uc_engine* uc, Address address, void* user_data);

Export* LookupExportByOrdinal(const char* name, uint32_t ordinal) {
    if (!strcmp(name, "COMCTL32.dll")) {
        if (ordinal == 17) {
            return LookupExportByName("InitCommonControls");
        }
    }
    return NULL;
}


// FIXME: Move to exe functions
void RelocateSection(Exe* exe, unsigned int sectionIndex) {
    // Relocate
    //reloc 21589 offset    0 [301d3017] ABSOLUTE
    //reloc 21590 offset   11 [301d3028] HIGHLOW
#if 0
    switch(relocation->type)
case RelocationAbsolute:
    assert(0);
    break;
case RelocationHighLow:
    assert(0);
    break;
default:
    assert(0);
    break;
}
#endif
}

void LoadSection(Exe* exe, unsigned int sectionIndex) {
    PeSection* section = &exe->sections[sectionIndex];
    
    // Map memory for section
    uint8_t* mem = (uint8_t*)aligned_malloc(0x1000, section->virtualSize);
    
    // Read data from exe and fill rest of space with zero
    fseek(exe->f, section->rawAddress, SEEK_SET);
    uint32_t rawSize = section->rawSize;
    if (rawSize > section->virtualSize) {
        rawSize = section->virtualSize;
    }
    fread(mem, 1, rawSize, exe->f);
    if (rawSize < section->virtualSize) {
        memset(&mem[rawSize], 0x00, section->virtualSize - rawSize);
    }
    
    // Write back address to the exe object
    exe->mappedSections[sectionIndex] = mem;
}

void UnloadSection(Exe* exe, unsigned int sectionIndex) {
    aligned_free(exe->mappedSections[sectionIndex]);
    exe->mappedSections[sectionIndex] = NULL;
}

//FIXME: These are hacks (register when mapping instead!)!
extern Exe* exe;

// This loads the exe into memory, even on Linux
// NOTE: This purposely does not map the file into memory for portability
Exe* LoadExe(const char* path) {
  exe = (Exe*)malloc(sizeof(Exe)); //FIXME: Hack to make this global!
  memset(exe, 0x00, sizeof(Exe));

  // Load the exe file and skip the DOS header
  exe->f = fopen(path, "rb");
  if (exe->f == NULL) {
    perror(path);
    return NULL;
  }
  char dosMagic[2];
  fread(dosMagic, 1, 2, exe->f);
  assert(memcmp(dosMagic, "MZ", 2) == 0);
  fseek(exe->f, 60, SEEK_SET);
  uint32_t peAddress;
  fread(&peAddress, 1, 4, exe->f);

  // Read COFF header
  fseek(exe->f, peAddress, SEEK_SET);
  char peMagic[4];
  fread(peMagic, 1, 4, exe->f);
  assert(memcmp(peMagic, "PE\x00\x00", 4) == 0);
  fread(&exe->coffHeader, 1, sizeof(exe->coffHeader), exe->f);
  sys_printf("Machine type: 0x%" PRIX16 "\n", exe->coffHeader.machine);
  sys_printf("Number of sections: %" PRIu16 "\n", exe->coffHeader.numberOfSections);
  
  // Read optional PE header
  assert(exe->coffHeader.sizeOfOptionalHeader >= sizeof(exe->peHeader));
  fread(&exe->peHeader, 1, sizeof(exe->peHeader), exe->f);
  sys_printf("Entry point: 0x%" PRIX32 "\n", exe->peHeader.imageBase + exe->peHeader.addressOfEntryPoint);

  //FIXME: Parse data dictionaries
  exe->dataDirectories = (PeDataDirectory*) malloc(exe->peHeader.numberOfRvaAndSizes * sizeof(PeDataDirectory));
  fread(exe->dataDirectories, exe->peHeader.numberOfRvaAndSizes, sizeof(PeDataDirectory), exe->f);

  // Load sections
  exe->mappedSections = (uint8_t**)malloc(exe->coffHeader.numberOfSections * sizeof(uint8_t*));
  exe->sections = (PeSection*)malloc(exe->coffHeader.numberOfSections * sizeof(PeSection));
  for(unsigned int sectionIndex = 0; sectionIndex < exe->coffHeader.numberOfSections; sectionIndex++) {
    PeSection* section = &exe->sections[sectionIndex];

    // Load actual data
    //FIXME: Use/skip proper alignment etc. from header
    fread(section, 1, sizeof(PeSection), exe->f);

    //FIXME: Check if section must be loaded
    if (1) {
      size_t cur = ftell(exe->f);
      LoadSection(exe, sectionIndex);
      fseek(exe->f, cur, SEEK_SET);
    } else {
      exe->mappedSections[sectionIndex] = NULL;
    }

    // Debug printing
    sys_printf("Section %u: Virtual: 0x%" PRIX32 " - 0x%" PRIX32 " Initialized: 0x%" PRIX32 " - 0x%" PRIX32 " ('%.8s')\n", sectionIndex,
           exe->peHeader.imageBase + section->virtualAddress,
           exe->peHeader.imageBase + section->virtualAddress + section->virtualSize - 1,
           exe->peHeader.imageBase + section->virtualAddress,
           exe->peHeader.imageBase + section->virtualAddress + section->rawSize - 1,
           section->name);

  }

  // Do relocations (not necessary as we currently load to preferred address)
  if (false) {
    uint32_t relocationRva = exe->peHeader.imageBase + exe->dataDirectories[5].virtualAddress;
    uint32_t remainingSize = exe->dataDirectories[5].size;

    while(remainingSize >= sizeof(API(IMAGE_BASE_RELOCATION))) {
      API(IMAGE_BASE_RELOCATION)* baseRelocation = (API(IMAGE_BASE_RELOCATION)*) Memory(relocationRva);
      assert(baseRelocation->sizeOfBlock >= sizeof(API(IMAGE_BASE_RELOCATION)));

      unsigned int relocationCount = (baseRelocation->sizeOfBlock - sizeof(API(IMAGE_BASE_RELOCATION))) / 2;
      sys_printf("Base relocation: 0x%" PRIX32 " (%d relocations)\n", baseRelocation->virtualAddress, relocationCount);
      uint16_t* relocations = (uint16_t*) Memory(relocationRva);
      for(unsigned int i = 0; i < relocationCount; i++) {
        uint16_t relocation = relocations[i];
        unsigned int type = relocation >> 12;
        unsigned int offset = relocation & 0xFFF;
    
        sys_printf("  Relocation (type %d) at 0x%" PRIX32 "\n", type, exe->peHeader.imageBase + baseRelocation->virtualAddress + offset);
        switch(type) {
          case 0: // IMAGE_REL_BASED_ABSOLUTE
            // "This relocation is meaningless and is only used as a place holder to round relocation blocks up to a DWORD multiple size."
            break;
          case 3: // IMAGE_REL_BASED_HIGHLOW
            // "This relocation means add both the high and low 16 bits of the delta to the DWORD specified by the calculated RVA."
            break;
          default:
            assert(false);
            break;
        }
      }

      relocationRva += baseRelocation->sizeOfBlock;
      remainingSize -= baseRelocation->sizeOfBlock;
    }    
  }


  // Check imports
  // IMAGE_DIRECTORY_ENTRY_IMPORT = 1
  // IMAGE_DIRECTORY_ENTRY_IAT = 12
  {
    uint32_t importRva = exe->peHeader.imageBase + exe->dataDirectories[1].virtualAddress;
    uint32_t remainingSize = exe->dataDirectories[1].size;
    sys_printf("Import table located at 0x%" PRIX32 "\n", importRva);
    //FIXME: Should be done differently. Import table expects zero element at end which is not checked yet! (it's optional here)
    while(remainingSize >= sizeof(API(IMAGE_IMPORT_DESCRIPTOR))) {

      // Access import and check if it is valid
      API(IMAGE_IMPORT_DESCRIPTOR)* imports = (API(IMAGE_IMPORT_DESCRIPTOR)*) Memory(importRva);
      if (IsZero(imports, sizeof(API(IMAGE_IMPORT_DESCRIPTOR)))) {
        break;
      }

      // Dump imports
      const char* name = (const char*)Memory(exe->peHeader.imageBase + imports->name);
      //FIXME: Bound checking?
      uint32_t originalThunkAddress = exe->peHeader.imageBase + imports->originalFirstThunk;
      uint32_t thunkAddress = exe->peHeader.imageBase + imports->firstThunk;
      sys_printf("Imports for '%s' (0x%" PRIX32 " / 0x%" PRIX32 ")\n", name, originalThunkAddress, thunkAddress);
      while(1) {
        uint32_t importByNameAddress = *(uint32_t*)Memory(originalThunkAddress);
        uint32_t* symbolAddress = (uint32_t*)Memory(thunkAddress);
        if (importByNameAddress == 0) {
          break;
        }

        // Dump entry
        char* label;
        if (importByNameAddress & 0x80000000) {
          unsigned int ordinal = importByNameAddress & 0x7FFFFFFF;
          sys_printf("  0x%" PRIX32 ": @%" PRIu32 " ..", thunkAddress, ordinal);
          label = (char*) malloc(128);
          sprintf(label, "<%s@%d>", name, ordinal);
        } else {
          API(IMAGE_IMPORT_BY_NAME)* importByName = (API(IMAGE_IMPORT_BY_NAME)*) Memory(exe->peHeader.imageBase + importByNameAddress);
          sys_printf("  0x%" PRIX32 ": 0x%" PRIX16 " '%s' ..", thunkAddress, importByName->hint, importByName->name);
          label = importByName->name;
        }

        Export* export_sym = NULL;
        if (importByNameAddress & 0x80000000) {
          export_sym = LookupExportByOrdinal(name, importByNameAddress & 0x7FFFFFFF);
        } else {
          export_sym = LookupExportByName(label);
        }

        //FIXME: This is a hack.. these calls were WAY too slow because UC is really bad at switching contexts
#if 1
        if (!strcmp("EnterCriticalSection", label) || !strcmp("LeaveCriticalSection", label)) {
          Address codeAddress = Allocate(20);
          uint8_t* code = (uint8_t*)Memory(codeAddress);
          //FIXME:
          *code++ = 0x58; // pop eax // return address
          *code++ = 0x59; // pop ecx // argument
          // test count, 0
          // jne break
          // inc $count for entercrit // dec $count for leavecrit
          // ret
          // break: < Place a breakpoint here which switches threads
          *code++ = 0x50; // push eax
          *code++ = 0xC3; // retn
          *symbolAddress = codeAddress;
          sys_printf("EnterCriticalSection, LeaveCriticalSection patched\n");
        } else
        if (!strcmp("DispatchMessageA", label)) {
          Address codeAddress = CreateDispatchMessage();
          *symbolAddress = codeAddress;
          sys_printf("DispatchMessageA patched\n");
        } else
#endif
        {
          
          if (export_sym == NULL) {
            Address hltAddress = CreateHlt();
            AddHltHandler(hltAddress, UnknownImport, (void*)label);
            AddExport(label, UnknownImport, hltAddress);
            *symbolAddress = hltAddress;
            sys_printf("missing at 0x%08X\n", hltAddress);
            //FIXME: Report error and assert false
          } else {
            if (true) { //(export->isVariable == false) {
              Address symAddress = export_sym->hook == 2 ? CreateInt21() : CreateHlt();
              export_sym->thunkAddress = thunkAddress;
              AddHltHandler(symAddress, export_sym->callback, (void*)label);
              *symbolAddress = symAddress;
              sys_printf("found at 0x%08X\n", symAddress);
            } else {
              sys_printf("found.. is variable\n");
              assert(false);
            }
          }

        }
        
        // Jump to next imported symbol
        originalThunkAddress += 4;
        thunkAddress += 4;
      }

      // Jump to next entry
      importRva += sizeof(API(IMAGE_IMPORT_DESCRIPTOR));
      remainingSize -= sizeof(API(IMAGE_IMPORT_DESCRIPTOR));
    }
  }

  return exe;
}

void RelocateExe(Exe* exe) {
  //FIXME
}

void UnloadExe(Exe* exe) {
  if (exe->mappedSections != NULL) {
    for(unsigned int sectionIndex = 0; sectionIndex < exe->coffHeader.numberOfSections; sectionIndex++) {
      if (exe->mappedSections[sectionIndex] != NULL) {
        UnloadSection(exe, sectionIndex);
      }
    }
    free(exe->mappedSections);
    exe->mappedSections = NULL;
  }
  if (exe->sections != NULL) {
    free(exe->sections);
    exe->sections = NULL;
  }
  if (exe->dataDirectories != NULL) {
    free(exe->dataDirectories);
    exe->dataDirectories = NULL;
  }
  fclose(exe->f);
  free(exe);
}
