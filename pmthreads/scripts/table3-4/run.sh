TEST_HOME=/root/pmthreads/benchmarks/recovery-test/
SCRIPT_HOME=/root/pmthreads/scripts/table3-4/

cd $TEST_HOME
./table-3.sh > /tmp/table3 2>&1 
./table-4.sh > /tmp/table4 2>&1

cd $SCRIPT_HOME 
echo "TABLE-3"
./draw.sh /tmp/table3
echo " "
echo "TABLE-4"
./draw.sh /tmp/table4

#real	1m14.222s 

