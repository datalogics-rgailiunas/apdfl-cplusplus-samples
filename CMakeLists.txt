
################################################################################
# Install resources into "dist" for APDFL 15
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
        )
