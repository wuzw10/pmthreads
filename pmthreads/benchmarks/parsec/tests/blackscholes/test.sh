


status=$(cat /tmp/output | grep "NORMAL EXIT")


if [  -z "$status" ]; then
	cat /tmp/output | grep "real"
else
	echo "WRONG"
fi 
