set(SRC
	dbusextendedabstractinterface.cpp
	dbusextendedpendingcallwatcher.cpp
	dbusextended.h
	dbusextendedabstractinterface.h)
set(HEADERS
	dbusextended.h
	dbusextendedabstractinterface.h
	DBusExtended
	DBusExtendedAbstractInterface)

add_library(dbusextended SHARED ${SRC})

target_link_libraries(dbusextended
	Qt5::Core
	Qt5::DBus)
target_include_directories(dbusextended PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include/libdbusextended>)

set_target_properties(dbusextended PROPERTIES
	SOVERSION ${PROJECT_VERSION_MAJOR}
	VERSION ${PROJECT_VERSION})

# Create pkgconfig file
configure_file(dbusextended.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/dbusextended-qt5.pc
	@ONLY)

configure_file(dbusextended-qt5.prf.in
	${CMAKE_CURRENT_BINARY_DIR}/dbusextended-qt5.prf
	@ONLY)

install(TARGETS dbusextended
	EXPORT dbusextendedTargets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${HEADERS} DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}/dbusextended)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dbusextended-qt5.pc
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dbusextended-qt5.prf
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/mkspecs/features/)

# Configure and install the CMake Config File
include(CMakePackageConfigHelpers)

configure_package_config_file(
	"dbusextendedConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/dbusextendedConfig.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dbusextended/)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/dbusextendedConfigVersion.cmake
	VERSION ${dbusextended_VERSION}
	COMPATIBILITY AnyNewerVersion)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/dbusextendedConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/dbusextendedConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dbusextended/
	COMPONENT Devel)
export(TARGETS dbusextended FILE dbusextendedTargets.cmake)
install(EXPORT dbusextendedTargets
	FILE dbusextendedTargets.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dbusextended/
	COMPONENT Devel)

