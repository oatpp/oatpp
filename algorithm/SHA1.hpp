// [intaken src] https://github.com/vog/sha1
/*
     sha1.hpp - header of
 
     ============
     SHA-1 in C++
     ============
 
     100% Public Domain.
 
     Original C Code
     -- Steve Reid <steve@edmweb.com>
     Small changes to fit into bglibs
     -- Bruce Guenter <bruce@untroubled.org>
     Translation to simpler C++ Code
     -- Volker Diels-Grabsch <v@njh.eu>
     Safety fixes
     -- Eugene Hopkinson <slowriot at voxelstorm dot com>
 */

#ifndef oatpp_algorithm_SHA1_HPP
#define oatpp_algorithm_SHA1_HPP

#include <cstdint>
#include <iostream>
#include <string>

namespace oatpp { namespace algorithm {

class SHA1
{
public:
  SHA1();
  void update(const std::string &s);
  void update(std::istream &is);
  std::string final();
  static std::string from_file(const std::string &filename);
  
private:
  uint32_t digest[5];
  std::string buffer;
  uint64_t transforms;
};

}}
  
#endif /* oatpp_algorithm_SHA1_HPP */
