set(app_src download.c)

set (pri_req Http_Client_App
            json
            File_hex
            Bootloader_App)

idf_component_register(
    SRCS ${app_src}
    INCLUDE_DIRS "."
    REQUIRES ${pri_req}
)
