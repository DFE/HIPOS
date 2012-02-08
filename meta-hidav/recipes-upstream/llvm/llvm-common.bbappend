FILESEXTRAPATHS_prepend := "${THISDIR}/llvm-common:"

PR_append = "+r1"

# provide native perl binaries via PATH
inherit perlnative
DEPENDS += "perl-native"

SRC_URI = "file://llvm-config"

PRINC = 1

do_compile() {
  cd ..
  # This adds an explicit call to the interpreter in the wrapper script
  #  so we don't run into shebang max size issues in the llvm-configX.X perl script
  #  if the path to perl-native > 127 chars.
  sed -i -e "s|@PATH_TO_PERL@|`which perl`|" llvm-config
}

