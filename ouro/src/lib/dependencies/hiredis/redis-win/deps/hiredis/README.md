hiredis-win
===========

The version of the hiredis library for windows can also be compiled under Linux, cross-platform

Based on the official last release (0.11.0) to be modified to implement all the functions of hiredis on Windows (all test cases are passed based on the official version of test.c file)

Compilation steps:

  1 Use vs2010 to open./vsproject/hiredis-win.sln

  2 Compile hiredis-win-lib, the project default configuration for the MD version, if there are other needs can modify the project configuration.

  3 The compiled library file is located in the debug or release folder under this folder.
