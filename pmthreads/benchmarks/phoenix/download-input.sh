if [ ! -f dataset/histogram.tar.gz ]
then 
	wget http://csl.stanford.edu/~christos/data/histogram.tar.gz -P ./dataset 
fi 

if [ ! -f dataset/linear_regression.tar.gz ]
then 
	wget http://csl.stanford.edu/~christos/data/linear_regression.tar.gz -P ./dataset
fi 

if [ ! -f dataset/string_match.tar.gz ]
then 
	wget http://csl.stanford.edu/~christos/data/string_match.tar.gz -P ./dataset
fi 

if [ ! -f dataset/reverse_index.tar.gz ]
then 
	wget http://csl.stanford.edu/~christos/data/reverse_index.tar.gz -P ./dataset
fi 

if [ ! -f dataset/word_count.tar.gz  ]
then 
	wget http://csl.stanford.edu/~christos/data/word_count.tar.gz -P ./dataset
fi 

cd dataset 

for a in `ls -1 *.tar.gz`; do tar -zxvf $a; done
