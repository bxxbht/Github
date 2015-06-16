project(browser)
add_definitions()
set(CMAKE_CXX_COMPILER arm-linux-g++)
set(CMAKE_C_FLAGS -Wall  -Wno-trigraphs  -g    -fno-strict-aliasing -fno-common -fno-unsigned-char  -pipe  -march=armv5te -mtune=arm9tdmi)

include_directories(/root/fltk-1.1.0b13_ut_arm_release /root/libiconv-1.14/out/include)

#link_directories(/root/fltk-1.1.0b13_ut_arm_release/lib /root/workspace/Tiny-4.02/xc/exports/lib
link_directories(/root/fltk-1.1.0b13_ut_arm_release/lib /root/workspace/Tiny-4.02/xc/exports/lib
)

add_executable (browser_arm browser.cxx)

#target_link_libraries(browser_arm fltk X11 Xext libiconv.a)
target_link_libraries(browser_arm fltk X11 Xext)


