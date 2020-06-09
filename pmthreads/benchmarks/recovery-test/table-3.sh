TYPE=1
for DELTA_T in 10 100 500 
do 
	./eval-pmthreads.sh $DELTA_T $TYPE 
	echo ""
done 

./eval-atlas.sh $TYPE
