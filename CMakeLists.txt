
################################################################################
# Install resources into "dist" for APDFL 18
################################################################################

set(DL_SAMPLES Sample_Source)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
        USE_SOURCE_PERMISSIONS
        PATTERN ".git" EXCLUDE
        PATTERN "_Input" EXCLUDE
        PATTERN "CMakeLists.txt" EXCLUDE
        )

install(DIRECTORY    ${CMAKE_CURRENT_SOURCE_DIR}/_Input/
        DESTINATION  "${INSTALL_RESOURCEDIR}/Sample_Input"
        USE_SOURCE_PERMISSIONS
        PATTERN "CMakeLists.txt" EXCLUDE
        )

if(UTIL_PLAT MATCHES "x64")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/All/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}/All
            USE_SOURCE_PERMISSIONS
            PATTERN "All_Datalogics_32Bit.sln" EXCLUDE
            )
else()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/All/
        DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}/All
        USE_SOURCE_PERMISSIONS
        PATTERN "All_Datalogics_64Bit.sln" EXCLUDE
        )
endif()