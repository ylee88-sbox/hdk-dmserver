#!/bin/sh
# BUILD FOR SYNCHRONICITY V6B2012x
if [ ! -e configure.ac ]; then
  echo "Script must be run in the root directory"
  exit 1
fi
if [ ! "$USER" = dtenv ]; then
  echo "Please run script as dtenv"
  exit 1
fi
#Example of install_prefix is /p/dt/cad/dmserver/<VERSION>
install_prefix=`pwd`/out2012/inst
echo "Build binaries will be installed to out2012/inst, use relink.csh to fix library search path after installing to final location"
rm -rf out2012
mkdir -p out2012
pushd out2013
../configure CC=/usr/intel/pkgs/gcc/4.7.2/bin/gcc CXX=/usr/intel/pkgs/gcc/4.7.2/bin/g++ --with-synchronicity_header=/p/dt/cad/x86-64_linux26/synchronicity/V6B2012x/syncinc/scapi --with-synchronicity_ldflags="-L/p/dt/cad/x86-64_linux26/synchronicity/V6B2012x/syncinc/lib.Linux -Wl,-rpath-link,/p/dt/cad/x86-64_linux26/synchronicity/V6B2012x/syncinc/lib.Linux" --with-synchronicity_libs="-lscapi -lfl" --prefix="$install_prefix" && make && make install
popd

echo "=================================================="
echo "Build binaries will be installed to out2013/inst, use relink.csh to fix library search path after installing to final location"
echo "eg bundle/relink.csh -bin_dir $install_prefix/bin -sync_dir /p/dt/cad/x86-64_linux26/synchronicity/V6B2012x/syncinc"
echo "=================================================="
