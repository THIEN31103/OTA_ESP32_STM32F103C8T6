set(app_src bootloader_app.c)

set (pri_req Min
             Uart
             Download)

idf_component_register(
    SRCS ${app_src}
    INCLUDE_DIRS "."
    REQUIRES ${pri_req}
)
