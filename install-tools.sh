#!/bin/bash

function show_usage {
    echo "Usage: to install everything (1-2 hours on Devcloud):"
    echo "  ./install-tools.sh"
    echo "If you encounter errors and want to install a failed component individually:"
    echo "  ./install-tools.sh m4|gmp|mpfr|mpc|cmake|gcc|llvm-clang|python-packages"
}

# No matter the script is sourced or directly run, BASH_SOURCE is always this script, and $1 is the
# argument to the script
T2S_PATH="$( cd "$(dirname "$BASH_SOURCE" )" >/dev/null 2>&1 ; pwd -P )" # The path to this script
if [ "$1" != "" -a "$1" != "m4"  -a  "$1" != "gmp" -a  "$1" != "mpfr" -a  "$1" != "mpc" -a  "$1" != "cmake" -a  "$1" != "gcc" -a "$1" != "llvm-clang" -a "$1" != "python-packages" ]; then
    show_usage
    if [ $0 == $BASH_SOURCE ]; then
        # The script is directly run
        exit
    else 
        return 
    fi
else
    components="$1"
fi

function install_cmake {
    eval major="$1"
    eval minor="$2"
    echo Installing cmake ...
    mkdir -p cmake-$minor && cd cmake-$minor
    wget -c https://cmake.org/files/v$major/cmake-$minor.tar.gz
    tar -zxvf cmake-$minor.tar.gz > /dev/null
    cd cmake-$minor
    mkdir -p build && cd build
    ../configure --prefix=$T2S_PATH/install > /dev/null
    make -j > /dev/null
    make install > /dev/null
    cd ..
    cd ..
    cd ..
}

function install_m4 {
    eval version="$1"
    echo Installing m4 ...
    wget -c http://ftp.gnu.org/gnu/m4/m4-$version.tar.xz
    tar xvf m4-$version.tar.xz > /dev/null
    cd m4-$version
    ./configure --prefix=$T2S_PATH/install > /dev/null
    make -j > /dev/null
    make install > /dev/null
    cd ..
}

function install_gmp {
    eval version="$1"
    echo Installing gmp ...
    wget -c https://ftp.gnu.org/gnu/gmp/gmp-$version.tar.xz
    tar xvf gmp-$version.tar.xz > /dev/null
    cd gmp-$version
    ./configure --prefix=$T2S_PATH/install > /dev/null
    make -j > /dev/null
    make install > /dev/null
    cd ..
}

function install_mpfr {
    eval version="$1"
    echo Installing mpfr ...
    wget -c https://www.mpfr.org/mpfr-current/mpfr-$version.tar.gz
    tar xvzf mpfr-$version.tar.gz > /dev/null
    cd mpfr-$version
    ./configure --prefix=$T2S_PATH/install --with-gmp=$T2S_PATH/install  > /dev/null
    make -j > /dev/null
    make install > /dev/null
    cd ..
}

function install_mpc {
    eval version="$1"
    echo Installing mpc ...
    wget -c https://ftp.gnu.org/gnu/mpc/mpc-$version.tar.gz
    tar xvzf mpc-$version.tar.gz > /dev/null
    cd mpc-$version
    ./configure --prefix=$T2S_PATH/install --with-gmp=$T2S_PATH/install  --with-mpfr=$T2S_PATH/install > /dev/null
    make -j > /dev/null
    make install > /dev/null
    cd ..
}

function install_gcc {
    eval version="$1"
    echo Installing gcc ...
    wget -c http://www.netgull.com/gcc/releases/gcc-$version/gcc-$version.tar.gz
    tar xvzf gcc-$version.tar.gz > /dev/null
    mkdir -p gcc-$version-build && cd gcc-$version-build
    export LD_LIBRARY_PATH=$T2S_PATH/install/lib:$T2S_PATH/install/lib64:$LD_LIBRARY_PATH
    ../gcc-$version/configure --enable-languages=c,c++ --disable-multilib --prefix=$T2S_PATH/install/gcc-$version --with-gmp=$T2S_PATH/install --with-mpfr=$T2S_PATH/install --with-mpc=$T2S_PATH/install > /dev/null
    make -j > /dev/null
    make install > /dev/null
    cd ..
}

function install_llvm_clang {
    eval release="$1"
    eval version="$2"
    eval gcc_version="$3"
    echo Installing llvm and clang ...
    git clone -b release_$release https://github.com/llvm-mirror/llvm.git llvm$version
    git clone -b release_$release https://github.com/llvm-mirror/clang.git llvm$version/tools/clang
    cd llvm$version
    mkdir -p build && cd build
    export PATH=$T2S_PATH/install/bin:$PATH
    export LD_LIBRARY_PATH=$T2S_PATH/install/lib:$T2S_PATH/install/lib64:$LD_LIBRARY_PATH
    CXX=$T2S_PATH/install/gcc-$gcc_version/bin/g++ CC=$T2S_PATH/install/gcc-$gcc_version/bin/gcc cmake -DCMAKE_CXX_LINK_FLAGS="-Wl,-rpath,$T2S_PATH/install/gcc-$gcc_version/lib64 -L$T2S_PATH/install/gcc-$gcc_version/lib64" \
        -DLLVM_ENABLE_TERMINFO=OFF -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_ASSERTIONS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$T2S_PATH/install .. > /dev/null
    make -j > /dev/null
    make install > /dev/null
    cd ..
    cd ..
}

function install_python-packages {
    pip install numpy
    pip install matplotlib
}

# Below we install newer version of gcc and llvm-clang and their dependencies
mkdir -p $T2S_PATH/install $T2S_PATH/install/bin
export PATH=$T2S_PATH/install/bin:$PATH

cd $T2S_PATH
mkdir -p downloads
cd downloads

if [ "$components" == "" -o  "$components" == "m4" ]; then
    install_m4         "1.4.18"
fi
if [ "$components" == "" -o  "$components" == "gmp" ]; then
    install_gmp        "6.2.1"    
fi
if [ "$components" == "" -o  "$components" == "mpfr" ]; then
    install_mpfr       "4.1.0"
fi
if [ "$components" == "" -o  "$components" == "mpc" ]; then
    install_mpc        "1.2.1"
fi
if [ "$components" == "" -o  "$components" == "cmake" ]; then
    install_cmake      "3.11"  "3.11.1"
fi
if [ "$components" == "" -o  "$components" == "gcc" ]; then
    install_gcc        "7.5.0"
fi
if [ "$components" == "" -o  "$components" == "llvm-clang" ]; then
    install_llvm_clang "90"    "9.0"    "7.5.0"
fi
if [ "$components" == "" -o  "$components" == "python-packages" ]; then
    install_python-packages
fi
    
cd ..


