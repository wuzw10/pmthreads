CUR=$(pwd)
rm -rf ./build 
mkdir -p build 
cd build 
cmake .. -DALL_PERSISTENT=false -DUSE_MALLOC=true && sleep 1 
make -j 20

cd $CUR
rm -rf ./build2
mkdir -p build2
cd build2
cmake .. -DALL_PERSISTENT=true -DUSE_MALLOC=true && sleep 1  
make -j 20 
