if [ ! -f ./dataset/parsec-2.1-native.tar.gz ]
then 
	wget https://parsec.cs.princeton.edu/download/2.1/parsec-2.1-native.tar.gz -P ./dataset
fi 
cd dataset 
tar -xvf parsec-2.1-native.tar.gz
find ./parsec-2.1 -name '*.tar' -execdir tar -xvf '{}' \;
