#include "StdH.h"

//patcher.cpp
#include "patcher.h"

#define __DO_NOT_SHOW_PATCHER_WARNINGS__
#ifdef  __DO_NOT_SHOW_PATCHER_WARNINGS__
#pragma warning(disable:4309 4310 4311 4312)
#pragma comment(linker, "/IGNORE:4786")
#endif

// [Cecil] SE1 Debug output function
#define SE1_PATCHER_DEBUG_OUTPUT 0

#if SE1_PATCHER_DEBUG_OUTPUT
  #define PATCHER_OUT(Output) InfoMessage(Output) //CPrintF(Output)
#else
  #define PATCHER_OUT(Output)
#endif

// [Cecil] Allowed to rewrite anything of this length
static int _iRewriteLen = -1;

// [Cecil] Force instruction rewrite
void Patch_ForceRewrite(const int iLength) {
  _iRewriteLen = iLength;
};

HANDLE CPatch::s_hHeap = 0;
bool CPatch::okToRewriteTragetInstructionSet(long addr, int& rw_len)
{
  // [Cecil] Force rewrite
  if (_iRewriteLen != -1) {
    rw_len = _iRewriteLen;
    _iRewriteLen = -1;
    
    return true;
  }

  bool instruction_found;
  int read_len = 0;
  int instruction_len;

  do {
    instruction_len = 0;
    instruction_found = false;

    if (*reinterpret_cast<char*>(addr) == (char)0xE9) // jmp XX XX XX XX
    {
      PATCHER_OUT("jmp XX XX XX XX \n");
      instruction_len = 5;
      m_old_jmp = 5 + addr + *reinterpret_cast<long*>(addr + 1);

    } else if (*reinterpret_cast<char*>(addr) == (char)0x68 // push???
     ||        *reinterpret_cast<char*>(addr) == (char)0xB8 // mov eax, XX XX XX XX
     || !memcmp(reinterpret_cast<char*>(addr), "\xB8\x1E", 2))
    {
      PATCHER_OUT("2 \n");
      instruction_len = 5;
      instruction_found = true;
      
    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x8B\x55", 2)) // mov edx, [ebp + arg_0]
    {
      PATCHER_OUT("mov edx, [ebp+arg0]\n");
      instruction_len = 3;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x8B\xFF", 2) 
            || !memcmp(reinterpret_cast<char*>(addr), "\x8B\xEC", 2)
            || !memcmp(reinterpret_cast<char*>(addr), "\x8B\xF1", 2)
            || !memcmp(reinterpret_cast<char*>(addr), "\x8B\xF9", 2) // mov
            ||        *reinterpret_cast<char*>(addr) == (char)0x6A)  // push XX
    {
      PATCHER_OUT("3 \n");
      instruction_len = 2;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x8B\x46", 2))    
    {
      PATCHER_OUT("mov ecx, [ebp + arg_0] \n");
      instruction_len = 3;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x8B\x4D", 2)) // mov ecx, [ebp + arg_0]
    {
      PATCHER_OUT("mov ecx, [ebp + arg_0] \n");
      instruction_len = 3;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x8B\x75", 2))
    {
      PATCHER_OUT("mov esi, [ebp + arg_0] \n");
      instruction_len = 3;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x8B\x45", 2))
    {
      PATCHER_OUT("mov eax, [ebp + arg_0] \n");
      instruction_len = 3;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x8D\x45", 2)) // lea eax, [ebp+...]
    {
      PATCHER_OUT("lea eax, [ebp+...] \n");
      instruction_len = 3;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x64\xA1", 2)) // mov eax, large FS
    {
      PATCHER_OUT("mov eax, large FS \n");
      instruction_len = 6;
      instruction_found = true;

    } else if (*reinterpret_cast<char*>(addr) == (char)0xA1) // mov eax, DWORD
    {
      PATCHER_OUT("mov eax, XX XX XX XX \n");
      instruction_len = 5;
      instruction_found = true;

    } else if ((*reinterpret_cast<char*>(addr) >= (char)0x50) && (*reinterpret_cast<char*>(addr) < (char)0x58)) // push
    {
      PATCHER_OUT("push xxx\n");
      instruction_len = 1;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x81\xEC", 2)) // sub esp, DWORD
    {
      PATCHER_OUT("sub esp, DWORD \n");
      instruction_len = 6;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\x83\xEC", 2)) // sub esp, byte + N 
    {
      PATCHER_OUT("sub esp, byte + N \n");
      instruction_len = 3;
      instruction_found = true;

    } else if (*reinterpret_cast<char*>(addr) == (char)0x89) // mov
    {
      PATCHER_OUT("mov\n");
      instruction_len = 3;
      instruction_found = true;

    } else if (!memcmp(reinterpret_cast<char*>(addr), "\xF6\x46", 2)) // test byte ptr [esi+...]
    {
      PATCHER_OUT("test byte ptr [esi+...] \n");
      instruction_len = 4;
      instruction_found = true;

    } else if (*reinterpret_cast<char*>(addr) == (char)0xD9  // fld
            || *reinterpret_cast<char*>(addr) == (char)0xD8) // fmul
    {
      PATCHER_OUT("fld / fadd / fmul \n");
      instruction_len = 6;
      instruction_found = true;
    }

    read_len += instruction_len;
    addr     += instruction_len;

    if (read_len >= 5) {
      rw_len = read_len;
      PATCHER_OUT("Finished: read_len >= 5 \n\n");

      return true;
    }
  } while(instruction_found);
  
  #if SE1_PATCHER_DEBUG_OUTPUT
    CTString strNotFound(0, "Invalid instruction! (0x%X) \n\n", *reinterpret_cast<long *>(addr));
    PATCHER_OUT(strNotFound);
  #endif
  
  return false;
}

BOOL CPatch::HookFunction(long FuncToHook, long  MyHook, long* NewCallAddress, bool patch_now)
{
  if (!s_hHeap)
  {
    s_hHeap = HeapCreate(
      /*HEAP_CREATE_ENABLE_EXECUTE*/0x00040000 | HEAP_NO_SERIALIZE, 
      1048576, //1MB, should be quite enouth
      0);//not limited
  }
  BOOL retVal = FALSE;
  if(FuncToHook == MyHook) return FALSE;
  if(FuncToHook == 0 || MyHook == 0) return FALSE;


  DWORD OldProtect;
  if(VirtualProtect( reinterpret_cast<void*>(FuncToHook), 10, PAGE_READWRITE, &OldProtect ))
  {
    int rewrite_len = 0;
    m_old_jmp = 0;
    const int long_jmp_len = 5;
    int new_instruction_set_len = 0;


    if(okToRewriteTragetInstructionSet(FuncToHook, rewrite_len))
    {
      new_instruction_set_len = rewrite_len;
      if(m_old_jmp == 0) new_instruction_set_len += long_jmp_len;
      //m_PatchInstructionSet = new char[new_instruction_set_len]; //executable instructions
      m_PatchInstructionSet = reinterpret_cast<char*> (HeapAlloc(s_hHeap, HEAP_ZERO_MEMORY, new_instruction_set_len)); //executable instructions

      *NewCallAddress = reinterpret_cast<long>(m_PatchInstructionSet);
      m_RestorePatchSet = new char[rewrite_len]; //not executable memory backup
      
      // 5 bytes(jmp+address) = jmp XX XX XX XX
      char InstructionSet[long_jmp_len] = {(char)0xE9, (char)0x00, (char)0x00, (char)0x00, (char)0x00};
      //ZeroMemory(m_PatchInstructionSet, new_instruction_set_len);

      //generating code
      memcpy(m_PatchInstructionSet, reinterpret_cast<char*>(FuncToHook), rewrite_len); //copy old bytes
      if(m_old_jmp == 0) m_PatchInstructionSet [rewrite_len] = (char)0xE9;                   //long jmp
      long jmp_new = m_old_jmp ? m_old_jmp : FuncToHook + rewrite_len;

      *reinterpret_cast<int*>(m_PatchInstructionSet + (new_instruction_set_len - long_jmp_len) + 1) =
        (jmp_new)   -    ((reinterpret_cast<long>(m_PatchInstructionSet)) + new_instruction_set_len);
                          //calculate and set
                          //address to jmp
                          //to old function

      /////////////////////////////////
      // rewrite function
      // set a jump to my MyHook
      *reinterpret_cast<int*>(InstructionSet + 1) = MyHook - (FuncToHook + long_jmp_len);
      // rewrite original function address
      memcpy(m_RestorePatchSet, InstructionSet, rewrite_len);
      ////////////////////////////////


      m_FuncToHook = FuncToHook;
      m_restore_size = rewrite_len;
      m_size = new_instruction_set_len;
      m_valid = true;

      ::VirtualProtect( m_PatchInstructionSet, new_instruction_set_len, PAGE_EXECUTE_READWRITE, &m_protect);
      if(patch_now)set_patch();
      retVal = TRUE;

    }


    ::VirtualProtect( reinterpret_cast<void*>(FuncToHook), 5, OldProtect, &OldProtect);
  }
  return retVal;
}

CPatch::~CPatch()
{
  if(!m_set_forever)
  {
    remove_patch(true);
  }
}

bool CPatch::patched()
{
  return m_patched;
}
bool CPatch::ok(){return m_valid;}
bool CPatch::ok(bool _valid)
{
  m_valid = _valid;
  return m_valid;
}
void CPatch::remove_patch(bool forever)
{
  if(m_set_forever)return;
  if(m_patched)
  {
    if(!m_valid)return;
    m_valid = false;
    DWORD OldProtect;
    if(!::VirtualProtect(m_PatchInstructionSet, m_size, PAGE_READWRITE, &OldProtect))return;
    DWORD FuncOldProtect;
    if(::VirtualProtect(reinterpret_cast<void*>(m_FuncToHook), m_restore_size, PAGE_READWRITE, &FuncOldProtect))
    {
      ::memcpy(reinterpret_cast<void*>(m_FuncToHook), m_PatchInstructionSet, m_restore_size);
      if(m_old_jmp)
      {
        *reinterpret_cast<long*>(m_FuncToHook + m_restore_size - 5 + 1)
           = m_old_jmp - (m_FuncToHook + m_restore_size);
      }
      ::VirtualProtect(reinterpret_cast<void*>(m_FuncToHook), m_restore_size, FuncOldProtect, &FuncOldProtect);
    }
    ::VirtualProtect(m_PatchInstructionSet, m_size, m_protect, &OldProtect);
    m_patched = false;
    m_valid = true;
  }
  if(forever)
  {
    m_valid = false;
    delete[] m_RestorePatchSet;
    //delete[] m_PatchInstructionSet;
    HeapFree(s_hHeap, 0, m_PatchInstructionSet);
    m_RestorePatchSet = 0;
    m_PatchInstructionSet = 0;
  }
}
void CPatch::set_patch()
{
  if(!m_valid)return;
  if(m_patched)return;
  m_valid = false;
  DWORD OldProtect;
  if(::VirtualProtect(reinterpret_cast<void*>(m_FuncToHook), m_restore_size, PAGE_READWRITE, &OldProtect))
  {
    ::memcpy(reinterpret_cast<void*>(m_FuncToHook), m_RestorePatchSet, m_restore_size);
    ::VirtualProtect(reinterpret_cast<void*>(m_FuncToHook), m_restore_size, OldProtect, &OldProtect);
  }
  m_valid = true;
  m_patched = true;
}
