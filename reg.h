// reg.h -- IR registers
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-01
//

#ifndef __REG_H__
#define __REG_H__

#include <string>


class Fun;


// IR register, representing a value/storage-location.
//
class Reg
{
public:

  Reg (const std::string &name, Fun *fun = 0);
  ~Reg ();


  // Return the name of this register.
  //
  const std::string &name () const { return _name; }


  // Set the function this register is associated with to FUN.  This
  // will also remove the register from any previously associated
  // function, and add it to FUN.  FUN may be NULL.
  //
  void set_fun (Fun *fun);


private:

  // Name of this register.
  //
  std::string _name;

  // Function where this register is used.
  //
  Fun *_fun = 0;
};


#endif // __REG_H__


