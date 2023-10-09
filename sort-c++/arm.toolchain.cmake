# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 4.19.111)
if(NOT DEFINED CMAKE_SYSTEM_PROCESSOR)
  set(CMAKE_SYSTEM_PROCESSOR aarch64)
else()
  #message("CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
endif()

# which C and C++ compiler to use
# set(CMAKE_CXX_COMPILER /home/manu/softwares/host/bin/aarch64-buildroot-linux-gnu-g++)
# set(CMAKE_C_COMPILER   /home/manu/softwares/host/bin/aarch64-buildroot-linux-gnu-gcc)
# SET(CMAKE_LINKER /home/manu/softwares/host/bin/aarch64-buildroot-linux-gnu-ld)
# SET(CMAKE_AR /home/manu/softwares/host/bin/aarch64-buildroot-linux-gnu-ar CACHE FILEPATH "Archiver")

# set(CMAKE_CXX_COMPILER /home/manu/softwares/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++)
# set(CMAKE_C_COMPILER   /home/manu/softwares/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
# SET(CMAKE_LINKER /home/manu/softwares/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-ld)
# SET(CMAKE_AR /home/manu/softwares/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-ar CACHE FILEPATH "Archiver")

set(CMAKE_CXX_COMPILER /opt/hisi-linux/x86-arm/arm-himix200-linux/bin/arm-himix200-linux-g++)
set(CMAKE_C_COMPILER   /opt/hisi-linux/x86-arm/arm-himix200-linux/bin/arm-himix200-linux-gcc)
SET(CMAKE_LINKER /opt/hisi-linux/x86-arm/arm-himix200-linux/bin/arm-himix200-linux-ld)
SET(CMAKE_AR /opt/hisi-linux/x86-arm/arm-himix200-linux/bin/arm-himix200-linux-ar CACHE FILEPATH "Archiver")

# location of the target environment
# set(CMAKE_FIND_ROOT_PATH /home/manu/nfs/opencv_install_rv1126)
# set(CMAKE_FIND_ROOT_PATH /home/manu/nfs/opencv_install_rk3588)
set(CMAKE_FIND_ROOT_PATH /home/manu/nfs/opencv_install_hi3519a)

# adjust the default behavior of the FIND_XXX() commands:
# search for headers and libraries in the target environment,
# search for programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)