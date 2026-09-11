cmake_minimum_required(VERSION 3.15)
project(HighSpeedNmeaSystem C CXX)

# Все артефакты находятся внутри выбранного build tree:
# build/mingw или build/msvc.
set(NMEA_RELEASE_DIR "${CMAKE_BINARY_DIR}/release")
set(NMEA_ARCHIVE_DIR "${CMAKE_BINARY_DIR}/archive")
set(NMEA_LIB_DIR "${CMAKE_BINARY_DIR}/lib")

foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${CONFIG}" CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NMEA_RELEASE_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NMEA_RELEASE_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${NMEA_ARCHIVE_DIR}")
endforeach()

function(nmea_set_target_output_directories TARGET_NAME)
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${NMEA_RELEASE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${NMEA_RELEASE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${NMEA_ARCHIVE_DIR}"
    )

    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
    if(TARGET_TYPE STREQUAL "STATIC_LIBRARY")
        set_target_properties(${TARGET_NAME} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${NMEA_LIB_DIR}"
        )
    endif()
endfunction()

# ====================================================================
# 1. ���������� ��������� ����������
# ====================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ����������� �������� ������ �������
set(PROJECT_SOURCES
    main.cpp
    nmea_processor.cpp
    nmea_service.cpp
    nmea450_decoder.cpp
    nmea_decoder.cpp
    ais_decoder.cpp
    high_speed_geo_parser.cpp
)

# �������� ������������ �����
add_executable(nmea_high_speed ${PROJECT_SOURCES})
nmea_set_target_output_directories(nmea_high_speed)

# ====================================================================
# 2. ��������� ����������� ���������� � FAST-MATH
# ====================================================================
if(MSVC)
    target_compile_options(nmea_high_speed PRIVATE 
        /O2          # ������������ ����������� �� ��������
        /Oi          # ��������� ���������� (intrinsic) �������
        /Ot          # ������������ ����������� �������� ����� ��������
        /fp:fast     # ���������� -ffast-math ��� ����������� MSVC
        /arch:AVX2   # ��������� ���������� AVX2 (������� ���������� FMA)
        /nologo      # ���������� ������ ��������� �����������
        /W3          # ������ ������� ��������������
    )
    message(STATUS "[MSVC OPTIMIZATION] ������� ���������������� �������������� ����� /fp:fast � ���������� AVX2.")
else()
    # ��������� ��� ������������ GCC � Clang (Linux / MinGW)
    target_compile_options(nmea_high_speed PRIVATE 
        -O3                 # ������������ ������� ����������� �����������
        -ffast-math         # ��������� ���������������� ���������� (IEEE 754 bypass)
        -march=native       # ����������� ��� ������� ����������� ���������� �����
        -ftree-vectorize    # ����������� ����-������������ ������ �����������
        -Wall               # ��������� ���� ������� ��������������
        -Wextra             # �������������� �������������� ��� ������� ����
    )
    # �������� ��������� ���������� POSIX ������� ��� ������ �������� GC
    target_link_libraries(nmea_high_speed PRIVATE pthread)
    message(STATUS "[GCC/CLANG OPTIMIZATION] �������� ����� -O3, -ffast-math � ����-������������ ��� ����������� -march=native.")
endif()

# ���������� ���������� ������������ ����� ��� ����-����� ����������
add_executable(test_math test_vectorized_math.cpp)
nmea_set_target_output_directories(test_math)

if(MSVC)
    target_compile_options(test_math PRIVATE /O2 /fp:fast /arch:AVX2)
else()
    target_compile_options(test_math PRIVATE -O3 -ffast-math -march=native -ftree-vectorize)
endif()

# ����������� ��������������� ������� (�����������, ��� CTest)
enable_testing()
add_test(NAME GeodeticMathVerification COMMAND test_math)
