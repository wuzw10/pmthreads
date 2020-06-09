./eval.sh mpt.out 10  > /dev/null  2>&1 &  
pid=$!
#`sudo lsof -i -P -n | grep LISTEN | grep 11215 | awk '{print '}`
#sleep 3 
#kill -s SIGINT $pid 
echo $pid
