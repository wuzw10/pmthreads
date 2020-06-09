cd /tmp
wget http://releases.llvm.org/7.0.1/llvm-7.0.1.src.tar.xz
tar -xvf llvm-7.0.1.src.tar.xz
cd llvm-7.0.1.src 
mkdir build 
cd build 
cmake .. 
make && make install 

cd /tmp
wget http://releases.llvm.org/7.0.1/cfe-7.0.1.src.tar.xz 
tar -xvf cfe-7.0.1.src.tar.xz
cd cfe-7.0.1.src
mkdir build
cd build
cmake ..
make && make install