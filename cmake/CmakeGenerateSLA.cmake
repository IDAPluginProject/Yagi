macro(SLA_TARGET TARGET_NAME SLASPEC_INPUT_FILE SLA_OUTPUT_FILE)

get_filename_component(SLA_TARGET_INPUT_DIR ${SLASPEC_INPUT_FILE} DIRECTORY)

add_custom_command(OUTPUT "${SLA_OUTPUT_FILE}"
	COMMAND sleighc "${SLASPEC_INPUT_FILE}" "${SLA_OUTPUT_FILE}"
	MAIN_DEPENDENCY "${SLASPEC_INPUT_FILE}"
	WORKING_DIRECTORY "${SLA_TARGET_INPUT_DIR}")
	
add_custom_target(${TARGET_NAME} ALL DEPENDS ${SLA_OUTPUT_FILE})

endmacro(SLA_TARGET)