#ifndef __REG_H__
#define __REG_H__

#include <string>


// A "register" representing a value/storage-location.
//
class Reg
{
public:

  Reg (const std::string &name) : _name (name) { }
  

  // Return the name of this register.
  //
  const std::string &name () const { return _name; }


private:

  // Name of this register.
  //
  std::string _name;
};


#endif // __REG_H__


