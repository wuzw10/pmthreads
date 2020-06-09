scons --build-example=stress-test



#run 

#LIST 
./build/examples/stress-test/stress-test -n 80000 -p 40000 -t 4 -r 0.5 -e
#B+Tree
./build/examples/stress-test/stress-test -n 2000000 -p 1000000 -t 4 -r 0.5 
