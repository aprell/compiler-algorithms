// memtoobj.h -- Calculation of object address given a pointer to a member
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-28
//

#ifndef __MEMTOOBJ_H__
#define __MEMTOOBJ_H__


#include <cstdint>
    

// Given a pointer, MEMBER, to a member inside an object, and a
// corresponding C++ pointer-to-member, MEMBER_PTR, for that member
// inside a specific class, return a pointer to the enclosing object
// of the same class.
//
// Note that this will not do anything sensible on a NULL pointer.
//
template <class T, typename M>
static inline constexpr T
*member_to_enclosing_object (M *member, const M T::*member_ptr)
{
  return reinterpret_cast<T *>
    (reinterpret_cast<intptr_t> (member)
     - reinterpret_cast <ptrdiff_t> (&(reinterpret_cast<T *> (0)->*member_ptr)));
}


#endif // __MEMTOOBJ_H__

