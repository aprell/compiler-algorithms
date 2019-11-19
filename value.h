// value.h -- IR value
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-18
//

#ifndef __VALUE_H__
#define __VALUE_H__


class Fun;
class Value;


// IR value, representing a known value.
//
// For now, we just handle integers.
//
class Value
{
public:

  // Return a new value with integer value INT_VALUE.
  // If FUN is non-NULL, the new value is added to FUN.
  //
  Value (int int_value, Fun *fun = 0);

  ~Value ();


  // Return the function where this value is used.
  //
  Fun *fun () const { return _fun; }

  // Set the function this value is associated with to FUN.  This
  // will also remove the value from any previously associated
  // function, and add it to FUN.  FUN may be NULL.
  //
  void set_fun (Fun *fun);


  // Return the actual value.
  //
  int int_value () const { return _int_value; }


private:

  // The actual value.
  //
  int _int_value;

  // Function where this value is used.
  //
  Fun *_fun = 0;
};


#endif // __VALUE_H__
