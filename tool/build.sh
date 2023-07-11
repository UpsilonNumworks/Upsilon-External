#!/usr/bin/sh
# Build the given application with the _(re)build suffix
# $1: application to rebuild

# Get the application name
application="$1"
# Remove the suffix
application_without_suffix="$(echo "${application}" | sed -e 's/_rebuild//')"
application_without_suffix="$(echo "${application_without_suffix}" | sed -e 's/_build//')"
echo "Building ${application_without_suffix}"
# Rebuild the application
make -C "apps/${application_without_suffix}"