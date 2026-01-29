set(app_src app_uart.c)
set(pri_req driver)
idf_component_register(
    SRCS ${app_src}
    INCLUDE_DIRS "."
    REQUIRES ${pri_req}
)
