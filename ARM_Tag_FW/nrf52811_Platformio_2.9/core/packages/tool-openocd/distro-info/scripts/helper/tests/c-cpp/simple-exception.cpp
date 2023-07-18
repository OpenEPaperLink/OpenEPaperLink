/* ----------------------------------------------------------------------------
 * This file is part of the xPack distribution.
 *   (https://xpack.github.io)
 * Copyright (c) 2020 Liviu Ionescu.
 *
 * Permission to use, copy, modify, and/or distribute this software 
 * for any purpose is hereby granted, under the terms of the MIT license.
 * -------------------------------------------------------------------------- */

#include <iostream>
#include <exception>

struct MyException : public std::exception {
   const char* what() const throw () {
      return "MyException";
   }
};
 
void
func(void)
{
  // Do not use new, so the exception will be a local object, not a pointer.
  throw MyException();
}

int
main(int argc, char* argv[])
{
  try {
    func();
  } catch(MyException& e) {
    std::cout << e.what() << std::endl;
  } catch(std::exception& e) {
    std::cout << "Other" << std::endl;
  }  

  return 0;
}
