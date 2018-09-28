/*
 * We have non-static A::af that takes a pointer to plain C function.
 * We want to pass a pointer to a non-static member function of class B
 * to A::af. But this is illegal.
 *
 * This code gives an example of how to solve the problem.
 * We want to make sure it works with templates.
 *
 * John Lapeyre  24 Oct 2014
 *
 */

#include <iostream>
//#define data_t int

template <typename data_t>
class A {
public:
  void af(void (*g) (data_t, void *p), void *p);
};

/*
 * This takes pointer to plain C/C++ function g or static member
 * function. Also takes other data in pointer p. When calling af from
 * a static member function of class B, p points to an object of class
 * B. We call non-static member functions of B via this object.
*/
template <typename data_t>
void A<data_t>::af( void (*g)(data_t, void *p),  void *p) {
  data_t x = 1;
  (*g)(x,p);
}

/***************************************************/

template <typename data_t>
class B {
public:
  void b_callback (data_t i) {
    std::cout << "B::b_callback got argument " << i << "\n";
    std::cout << "B::b_callback B::bx_ " << bx_ << "\n";
  }
  void b_main_routine();
  static void b_wrapper_callback(data_t i, void *p);
private:
  data_t bx_ = 3;
};

template <typename data_t>
void B<data_t>::b_wrapper_callback(data_t i, void *p) {
  B<data_t> *bp = (B<data_t>*) p;
  bp->b_callback(i);
}

template <typename data_t>
void B<data_t>::b_main_routine () {
  A<data_t> aobj;
  aobj.af(B<data_t>::b_wrapper_callback,this);
  // Careful: following compiles and runs with gcc and clang,
  // but it is nonsense.
  // aobj.af(B<data_t>::b_wrapper_callback, &aobj);
}

/***************************************************/

int main () {
  B<int> bobj;
  bobj.b_main_routine();
  return 1;
}
