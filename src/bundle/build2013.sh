#!/bin/sh
# BUILD FOR SYNCHRONICITY V6R2013HF2
if [ ! -e configure.ac ]; then
  echo "Script must be run in the root directory"
  exit 1
fi
if [ ! "$USER" = dtenv ]; then
  echo "Please run script as dtenv"
  exit 1
fi
#Example of install_prefix is /p/dt/cad/dmserver/<VERSION>
install_prefix=`pwd`/out2013/inst
echo "Build binaries will be installed to $install_prefix, use relink.csh to fix library search path after installing to final location"
rm -rf out2013
mkdir -p out2013
link1=/p/dt/cad/x86-64_linux26/synchronicity/V6R2013HF2/syncinc/linux_a64/lib.Linux
link2=/p/dt/cad/x86-64_linux26/synchronicity/V6R2013HF2/syncinc/linux_a64/code/bin
pushd out2013
../configure CC=/usr/intel/pkgs/gcc/4.7.2/bin/gcc CXX=/usr/intel/pkgs/gcc/4.7.2/bin/g++ --with-synchronicity_header=/p/dt/cad/x86-64_linux26/synchronicity/V6R2013HF2/syncinc/linux_a64/scapi --with-synchronicity_ldflags="-L${link1} -L${link2} -Wl,-rpath-link,${link1}:${link2}" --with-synchronicity_libs="-lscapi -lrce -lrde -lfl" --prefix="$install_prefix" && make || popd && exit 1
make install
popd

echo "=================================================="
echo "Build binaries will be installed to out2013/inst, use relink.csh to fix library search path after installing to final location"
echo "eg bundle/relink.csh -bin_dir $install_prefix -sync_dir /p/dt/cad/x86-64_linux26/synchronicity/V6R2013HF2/syncinc"
echo "=================================================="
