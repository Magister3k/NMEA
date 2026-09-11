# CMake generated Testfile for 
# Source directory: D:/Projects/C++/NMEA
# Build directory: D:/Projects/C++/NMEA/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(GeodeticMathVerification "D:/Projects/C++/NMEA/build/Debug/test_math.exe")
  set_tests_properties(GeodeticMathVerification PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/C++/NMEA/CMakeLists.txt;66;add_test;D:/Projects/C++/NMEA/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(GeodeticMathVerification "D:/Projects/C++/NMEA/build/Release/test_math.exe")
  set_tests_properties(GeodeticMathVerification PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/C++/NMEA/CMakeLists.txt;66;add_test;D:/Projects/C++/NMEA/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(GeodeticMathVerification "D:/Projects/C++/NMEA/build/MinSizeRel/test_math.exe")
  set_tests_properties(GeodeticMathVerification PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/C++/NMEA/CMakeLists.txt;66;add_test;D:/Projects/C++/NMEA/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(GeodeticMathVerification "D:/Projects/C++/NMEA/build/RelWithDebInfo/test_math.exe")
  set_tests_properties(GeodeticMathVerification PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/C++/NMEA/CMakeLists.txt;66;add_test;D:/Projects/C++/NMEA/CMakeLists.txt;0;")
else()
  add_test(GeodeticMathVerification NOT_AVAILABLE)
endif()
