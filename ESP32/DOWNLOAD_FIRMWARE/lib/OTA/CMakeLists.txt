set(app_src ota.c)
set(pri_req Download
            Bootloader_App
            Flash_App)
idf_component_register(
    SRCS ${app_src}
    INCLUDE_DIRS "."
    REQUIRES ${pri_req}
)
