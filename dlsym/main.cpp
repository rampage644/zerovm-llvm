#include <stdio.h>
#include <stdlib.h>

#include <iostream>

//// bfd.h include workaround
//#define PACKAGE 1
//#define PACKAGE_VERSION 1

//#include <bfd.h>
//#include <bfdlink.h>

#include <elfio/elfio.hpp>

extern "C"
void printMe() {
  std::cout << "success!" << std::endl;
}

using namespace ELFIO;
int main(int argc, char** argv) {

  // Create elfio reader
  elfio reader;
  // Load ELF data
  if ( !reader.load("a.out") ) {
    return -1;
  }
  // Print ELF file sections info
  Elf_Half sec_num = reader.sections.size();
  for ( int i = 0; i < sec_num; ++i ) {
    section* psec = reader.sections[i];
    std::cout << "hit " << i << " type " << psec->get_type() <<   std::endl;
    if ( psec->get_type() == SHT_SYMTAB ) {
      symbol_section_accessor symbols( reader, psec );
      std::string   name;
      Elf64_Addr    value;
      Elf_Xword     size;
      unsigned char bind;
      unsigned char type;
      Elf_Half      section_index;
      unsigned char other;
      bool succes = symbols.get_symbol( name, value, size, bind,
      type, section_index, other );
      if (name == "printMe") {
        u_int64_t ptr = (u_int64_t)0x440a00000000UL + value;
        u_int64_t meptr = (u_int64_t)&printMe;
        std::cout << name << " " << std::hex << value << " " << ptr << " " << meptr << std::endl;
        typedef void (*printMePtr)() ;
        printMePtr ptr_ = (printMePtr)ptr;
        ptr_();
      }
    }
  }


  return 0;
}
