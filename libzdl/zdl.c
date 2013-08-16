#include <string.h>
#include <stdint.h>

#define PACKAGE_NAME 1
#define PACKAGE_VERSION 1
#include <bfd.h>

// TODO: do smth with it
#define __USE_GNU
#include <dlfcn.h>

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
/// {symname: symvalue} hash
static struct bfd_hash_table hash;
/// reference counting
static int refCount = 0;
/// pointer to abfd file, NULL if not opened
/// should close when refCount == 0
bfd* file = NULL;
/// last error 
static char error[256] = "";
/// TODO: change to /proc/self
static const char imageFile[] = "/dev/self";
/// target architecture, hard-code to elf64-nacl
static const char target[] = "elf64-nacl";
static const intptr_t ZeroVMBase = 0x440a00000000LL;


struct bfd_hash_r_entry {
  struct bfd_hash_entry root;
  bfd_vma value;	/* Symbol value.  */
};


struct bfd_hash_entry *
_bfd_r_hash_newfunc (struct bfd_hash_entry *entry,
      struct bfd_hash_table *table,
      const char *string)
{
  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (entry == NULL)
    {
      entry = (struct bfd_hash_entry *)
          bfd_hash_allocate (table, sizeof (struct bfd_hash_r_entry));
      if (entry == NULL)
  return entry;
    }

  /* Call the allocation method of the superclass.  */
  entry = bfd_hash_newfunc (entry, table, string);
  if (entry)
    {
      struct bfd_hash_r_entry *h = (struct bfd_hash_r_entry *) entry;

      /* Initialize the local fields.  */
      h->value = 0;
    }

  return entry;
}


void *dlopen(const char *filename, int flag)
{
  // trying to dlopen main program, ok
  if (filename == NULL)
  {
    // open file if this first dlopen
    if (!refCount) {
      // try to load ELF image
      bfd_init();
      file = bfd_openr(imageFile, target);
      char** matching;
      bfd_check_format (file, bfd_archive);
      bfd_check_format_matches (file, bfd_object, &matching);

      long symcount;
      void *minisyms;
      unsigned int size;
      // reading symbols
      symcount = bfd_read_minisymbols (file, 0, &minisyms, &size);
      asymbol *sym1 = bfd_make_empty_symbol(file);
      // init hash table
      bfd_hash_table_init(&hash, _bfd_r_hash_newfunc, 0);

      // iterate over symbols and insert them into hash
      int i=0;
      bfd_byte* from = (bfd_byte*) minisyms;
      symbol_info syminfo;
      for (;i<symcount;++i) {
        // convert read-only symbol to full symbol
        asymbol *sym2 = bfd_minisymbol_to_symbol(file, 0, from, sym1);
        // get it data
        bfd_get_symbol_info (file, sym2, &syminfo);
        if (bfd_asymbol_value(sym2)) {
          // create hash entry if not exists
          struct bfd_hash_r_entry* entry = (struct bfd_hash_r_entry*)
              bfd_hash_lookup(&hash, bfd_asymbol_name(sym2), 1, 1);
          // fill in symvalue
          if (entry)
            entry->value = bfd_asymbol_value(sym2);
        }

        from += size;
      }
    }

    // increase reference count
    refCount += 1;
    // reset error
    strncpy(error, "", sizeof(error));
    return (void*)&hash;
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
  // reject if dlopen had not been called
  // reject if handle is different from "self-resolution" hash
  if (!refCount || handle != (void*)&hash)
  {
    strncpy(error, "Current handle is not opened.", sizeof(error));
    return NULL;
  }

  // perform search in hash
  struct bfd_hash_r_entry *entry = (struct bfd_hash_r_entry*)bfd_hash_lookup(&hash, symbol, 0, 0);
  if (entry)
    // success, fixup symvalue address with zerovm base offset
    return (void*) (intptr_t) (entry->value + ZeroVMBase);

  strncpy(error, "Symbol was not found.", sizeof(error));
  return NULL;
}

int dlclose(void *handle)
{
  // close only if there are open handles (refcount > 0)
  // close only if handle is "self-resloution" hash
  if (refCount && handle == (void*)&hash)
  {
    // success
    refCount -= 1;

    // if refCount is 0 then close file and free hash table
    if (!refCount) {
      bfd_hash_table_free(&hash);
      bfd_close(file);
      file = 0;
    }
    strncpy(error, "", sizeof(error));
		return 0;
  }

	// else failure
  strncpy(error, "Current handle is not opened.", sizeof(error));
	return -1; 
}

// glibc extension
// have to define these to avoid undefined references to them
int dladdr(const void *addr, Dl_info *info)
{
  info->dli_saddr = NULL;
  info->dli_sname = NULL;
  return 0;
}
void *dlvsym(void *handle, const char *symbol, const char *version)
{
  // no dynamic linking - no versioning
  return dlsym(handle, symbol);
}
