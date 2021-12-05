kpackage_install_package(package minimizeall scripts kwin)

# Copy the script to the build directory so one can run tests without prior
# make install. FIXME: use add_custom_command.
file(COPY package DESTINATION ${CMAKE_BINARY_DIR}/bin/kwin/scripts/)
