#include "libguile.h"
extern "C" SCM  doubleValue(SCM x) {
  const double value=scm_to_double(x);
  return scm_from_double(value*2);
}

int main(){
  scm_init_guile();
  SCM r=scm_c_make_gsubr("test1",1,0,0,(void*)&doubleValue);

}
