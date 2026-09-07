#include "oatpp/data/mapping/Tree.hpp"
#include <cstdio>
using oatpp::data::mapping::Tree;
int main() {
  {
    Tree t;
    t.setPrimitive<v_uint64>(0x4142434445464748ULL);
    printf("[*] before: type=%d\n", (int)t.getType());
    try { t.setVector(0xFFFFFFFFFFFFFFFFULL); }
    catch (const std::exception& e) { printf("[*] caught: %s\n", e.what()); }
    printf("[!] after : type=%d\n", (int)t.getType());
  } // ~Tree -> deleteValueObject() on confused node
  printf("[OK] destructed cleanly\n");
  return 0;
}
