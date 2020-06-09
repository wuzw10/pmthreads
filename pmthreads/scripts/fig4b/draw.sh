echo "import numpy as np" > output.py 
echo "data={}" >> output.py
apps=$(cat /tmp/phoenix-output | grep EVALUATE |  awk '{printf "\"%s\", ", $2}' )
echo "apps = [$apps]" >> output.py
echo "figname='/tmp/phoenix.pdf'" >> output.py
for config in pthread pmthreads-i pmthreads-p atlas nvthread
do
	timings=$(cat /tmp/phoenix-output | grep "$config" |  awk '{printf "%s, ", $2}')
	config=$(echo $config | tr "-" "_")
	echo "data[\"$config\"] = np.array([$timings])" >> output.py
done
python draw.py
