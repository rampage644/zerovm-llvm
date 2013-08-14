#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <elfio/elfio.hpp>

/// \b dl lib implementation for ZeroVM
///
/// At the moment ZeroVM doesn't support dynamic linking
/// and loading. But we still need dlsym functionality.
///
/// Current realization only works on nexe binary itself. Thus,
/// we could only use 'dlopen' with NULL-terminated string and
/// use dlsym to resolve symbols already linked-in. By now it's some
/// kind of symbol lookup, no dynamic loading and linking

/// we need some kind of "handle". in our case it'll be a pointer to
/// some static data
static int handleSelf = 0;

/// last error 
static char error[256] = "";

/// ELF-file reader object
static ELFIO::elfio reader;

extern "C" {

void *dlopen(const char *filename, int flag)
{
	// trying to dlopen main program, ok
	if (filename == NULL)
  {
    // try to load ELF image
    if (!reader.load("/dev/self"))
    {
      strncpy(error, "Can't read ELF image from /dev/self", sizeof(error));
      return NULL;
    }
    else
      return (void*)&handleSelf;
  }

  // tring to open something different, reject
  strncpy(error, "Dynamic loading is not supported yet!", sizeof(error));
  return NULL;
}

char *dlerror(void)
{
  return error;
}

void *dlsym(void *handle, const char *symbol)
{
  if (handle != (void*)&handleSelf)
  {
    strncpy(error, "Handle is not opened.", sizeof(error));
    return NULL;
  }

  std::string         name;
  ELFIO::Elf64_Addr   value;
  ELFIO::Elf_Xword    size;
  unsigned char       bind;
  unsigned char       type;
  ELFIO::Elf_Half     section_index;
  unsigned char       other;

  // iterate over sections
  // to find symtab section
  ELFIO::Elf_Half sec_num = reader.sections.size();
  for ( int i = 0; i < sec_num; ++i ) {
    ELFIO::section* psec = reader.sections[i];
    // check section flags
    if ( psec->get_type() == SHT_SYMTAB ) {
      // symbol iterator
      ELFIO::symbol_section_accessor symbols( reader, psec );
      // symbol lookup by name won't work cause we don't have .hash
      // section in NaCl ELF
      // TODO: optimize symbol lookup
      for (int i=0, count=symbols.get_symbols_num();i<count;++i) {
        symbols.get_symbol(i, name, value, size, bind,
                           type, section_index, other);

        if (name == symbol)
          return (void*)(uintptr_t)(0x440a00000000LL + value);
      }
    }
  }

  strncpy(error, "Symbol wasn't found.", sizeof(error));
  return NULL;
}

int dlclose(void *handle)
{
  if (handle == (void*)&handleSelf)
  {
    // success
    // TODO: clean or remove reader
		return 0;
  }

	// else failure
  strncpy(error, "Current handle isn't opened.", sizeof(error));
	return -1; 
}

// glibc extension
// have to define these to avoid undefined references to them
int dladdr(const void *addr, Dl_info *info)
{
  info->dli_sname = NULL;
  info->dli_saddr = NULL;
  return 0;
}
void *dlvsym(void *handle, const char *symbol, const char *version)
{
  // no dynamic linking - no versioning
  return dlsym(handle, symbol);
}

} // extern "C
