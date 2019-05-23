hiredis-win
===========

The windows version of the hiredis library can also be compiled under linux, cross-platform

Based on the official release version (0.11.0), all the functions of hiredis are implemented on windows (all test cases based on the official version of test.c file are passed)

Compile steps:

    1 Open vs. /vsproject/hiredis-win.sln using vs2010
  
    2 Compile hiridis-win-lib, the project default configuration is MD version, if you have other requirements, you can modify the project configuration.
  
    3 Compile the generated library file in the debug or release folder under this folder.
