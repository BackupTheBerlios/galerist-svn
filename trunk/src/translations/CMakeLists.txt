#create translation files
add_custom_target(translations
                   COMMAND lrelease ${CMAKE_CURRENT_SOURCE_DIR}/english.ts -qm ${CMAKE_CURRENT_SOURCE_DIR}/english.qm)
