#!/bin/sh
echo "Creating ${top_builddir}/src/version.cpp"
${MKDIR_P} ${top_builddir}/src
${SED} -e s/@PACKAGE_CHANGESET@/\"$(hg -R "${top_srcdir}" id -i)\"/ ${top_srcdir}/src/version.cpp.in > ${top_builddir}/src/version.cpp
