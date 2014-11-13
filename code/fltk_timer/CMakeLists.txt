
PROJECT(HELLOW_FLTK)


SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_C_COMPILER /usr/local/arm/4.2.2-eabi/usr/bin/arm-linux-gcc)
SET(CMAKE_CXX_COMPILER /usr/local/arm/4.2.2-eabi/usr/bin/arm-linux-g++)
ADD_DEFINITIONS(-Wall  -Wno-trigraphs  -g    -fno-strict-aliasing -fno-common -fno-unsigned-char  -fstack-protector-all -pipe  -march=armv5te -mtune=arm9tdmi)

SET(SRC_LIST
hellow_fltk.cxx
)

SET(FLTK_LIB_DIR /root/fltk-1.1.0b13_ut_arm/lib)
SET(TINY_X_DIR /root/workspace/Tiny-4.02/xc/exports)

INCLUDE_DIRECTORIES(/root/fltk-1.1.0b13_ut_arm ${TINY_X_DIR}/include)

LINK_DIRECTORIES(${TINY_X_DIR}/lib ${FLTK_LIB_DIR})



ADD_EXECUTABLE(hellow_fltk ${SRC_LIST})

TARGET_LINK_LIBRARIES(hellow_fltk X11 Xext fltk )

