set(TS_FILES ../i1n/check_laads_order_zh_CN.ts)

set(PROJECT_SOURCES
    main.cpp
    check_laads_order.cpp
    check_laads_order.h
    fast_cksum.cpp
    fast_cksum.h
    check_laads_order.ui
    icons.qrc
    ${TS_FILES}
    )

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

add_executable(check_laads_order ${PROJECT_SOURCES})

target_link_libraries(check_laads_order PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# 最高警告等级，并将警告视为错误，强制MSVC视源文件为utf8编码
if (MSVC)
    target_compile_options(check_laads_order PRIVATE /W4 /WX /source-charset:utf-8)
else ()
    target_compile_options(check_laads_order PRIVATE -Wall -Wextra -Werror)
endif ()
