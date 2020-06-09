time docker build . -t pmthreads
docker run -ti --shm-size=10g pmthreads:latest 
