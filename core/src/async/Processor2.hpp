//
//  Processor2.hpp
//  crud
//
//  Created by Leonid on 3/20/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_async_Processor2_hpp
#define oatpp_async_Processor2_hpp

#include "../collection/FastQueue.hpp"
#include "../base/Environment.hpp"

namespace oatpp { namespace async {
  
class Processor2 {
public:
  
  struct Routine {
  public:
    typedef oatpp::collection::FastQueue<Routine> FastQueue;
  public:
    Routine* _ref;
  };
  
public:
  
  struct Entry {
  public:
    typedef oatpp::collection::FastQueue<Entry> FastQueue;
  public:
    Routine* routine;
    Entry* _ref;
  };
  
private:
  Entry::FastQueue m_activeQueue;
  Entry::FastQueue m_waitingQueue;
public:
  
  
  
};
  
}}

#endif /* oatpp_async_Processor2_hpp */
