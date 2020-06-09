echo "import numpy as np" > output.py 
echo "data={}" >> output.py
apps=$(cat /tmp/parsec-output | grep EVALUATE |  awk '{printf "\"%s\", ", $2}' )
echo "apps = [$apps]" >> output.py
echo "figname='/tmp/parsec.pdf'" >> output.py
for config in pthread pmthreads-i pmthreads-p atlas nvthread
do
	timings=$(cat /tmp/parsec-output | grep "$config" |  awk '{printf "%s, ", $2}')
	config=$(echo $config | tr "-" "_")
	echo "data[\"$config\"] = np.array([$timings])" >> output.py
done

python draw.py
