
################################################################################
# Install resources into "dist" for APDFL 18
################################################################################

set(DL_SAMPLES Sample_Source)

install(DIRECTORY    ${CMAKE_CURRENT_SOURCE_DIR}/_Input/
        DESTINATION  "${INSTALL_RESOURCEDIR}/Sample_Input"
        USE_SOURCE_PERMISSIONS
        PATTERN "CMakeLists.txt" EXCLUDE
        )
if(UTIL_PLAT MATCHES "x64")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*32Bit*.*" EXCLUDE PATTERN "*.sh" EXCLUDE PATTERN "*Makefile*" EXCLUDE
            PATTERN "*.xcodeproj" EXCLUDE PATTERN "*.rel*" EXCLUDE PATTERN "*Mac*" EXCLUDE
            PATTERN "*.plist" EXCLUDE PATTERN "*.strings*" EXCLUDE PATTERN "dlutils" EXCLUDE
            PATTERN "*.mak" EXCLUDE PATTERN
            )
elseif(UTIL_PLAT MATCHES "Win32")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*64Bit*.*" EXCLUDE PATTERN "*.sh" EXCLUDE PATTERN "*Makefile*" EXCLUDE
            PATTERN "*.xcodeproj" EXCLUDE PATTERN "*.rel*" EXCLUDE PATTERN "*Mac*" EXCLUDE
            PATTERN "*.plist" EXCLUDE PATTERN "*.strings*" EXCLUDE PATTERN "dlutils" EXCLUDE
            PATTERN "*.mak" EXCLUDE PATTERN
            )
elseif(UTIL_PLAT MATCHES "Mac")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*Bit*.*" EXCLUDE PATTERN "*Makefile*" EXCLUDE PATTERN "*.bat" EXCLUDE 
            PATTERN "*.rel*" EXCLUDE PATTERN "*.mak" EXCLUDE
            )
else()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*.xcodeproj" EXCLUDE PATTERN "*.bat*" EXCLUDE PATTERN "*Mac*" EXCLUDE
            PATTERN "*.plist" EXCLUDE PATTERN "*.strings*" EXCLUDE PATTERN "*.sh*" EXCLUDE
            )
endif()