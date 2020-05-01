cmake_minimum_required(VERSION 3.1.0)

project(dbusextended
	VERSION 0.0.3
	DESCRIPTION "Provides additional features to the QDBusAbstractInterface class")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FeatureSummary)
include(GNUInstallDirs)

set(QT_MIN_VERSION "5.6.0")
find_package(Qt5 COMPONENTS Core DBus REQUIRED)

add_subdirectory(src)

feature_summary(WHAT PACKAGES_FOUND PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
