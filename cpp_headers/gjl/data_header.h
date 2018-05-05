// -*-c++-*-
#ifndef DATA_HEADER_H
#define DATA_HEADER_H

#include <string>

/*
  This is not yet working.
 */

namespace gjl {
  namespace header {

    typedef std::string string_t;

    template <typename object_t>
    class ObjectEntry {
    private:
      object_t value_;
      object_t (*getter_)() = nullptr;
      string_t text_ = "";
    };


    template <typename object_t>
    class Object {
      
    private: object_t instance_;
  
    };

    
    class Data {
    public:
      template <typename obj_t>
      void add_entry(obj_t x, string_t text) {
        
      }
    private:
      //      std::vector<
      
    };

  } /*** END namespace header */
} /*** END namespace gjl */

#endif
