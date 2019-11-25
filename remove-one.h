// remove-one.h -- Remove a single list element
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-25
//

#ifndef __REMOVE_ONE_H__
#define __REMOVE_ONE_H__

#include <list>


// Remove the first occurrence of EL in LIST.
// This differs from std::list<T>::remove as the latter removes all
// matching entries.
//
template<typename T>
void
remove_one (const T &el, std::list<T> &list)
{
  for (auto iter = list.begin (); iter != list.end (); ++iter)
    if (*iter == el)
      {
	list.erase (iter);
	break;
      }
}


#endif // __REMOVE_ONE_H__
