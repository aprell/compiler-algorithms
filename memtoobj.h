#ifndef __MEMTOOBJ_H__
#define __MEMTOOBJ_H__


#include <cstdint>
    
template <class T, typename M>
static inline constexpr T
*member_to_enclosing_object (M *member, const M T::*member_ptr)
{
  return reinterpret_cast<T *>
    (reinterpret_cast<intptr_t> (member)
     - reinterpret_cast <ptrdiff_t> (&(reinterpret_cast<T *> (0)->*member_ptr)));
}


#endif // __MEMTOOBJ_H__
