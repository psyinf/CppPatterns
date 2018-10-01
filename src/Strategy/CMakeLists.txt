file(GLOB HEADER_FILES *.h)
file(GLOB CPP_FILES *.cpp)
set(APP_NAME Strategy)

add_executable(${APP_NAME}
	${HEADER_FILES}
	${CPP_FILES}
)

target_link_libraries(${APP_NAME} 
	PRIVATE
)

set_target_properties(${APP_NAME} PROPERTIES
	FOLDER 			"Patterns/${APP_NAME}"
	DEBUG_POSTFIX 	_d
)

install(TARGETS ${APP_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)