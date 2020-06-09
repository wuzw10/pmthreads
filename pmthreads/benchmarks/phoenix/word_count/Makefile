include ../defines.mk

TEST_NAME = word_count
TEST_FILES = word_count sort
TEST_ARGS = ../dataset/word_count_datafiles/word_100MB.txt

DMP_O_GRAN = 6
DMP_O_QUANTUM = 100000

DMP_B_GRAN = 6
DMP_B_QUANTUM = 100000

include $(HOME)/pmthreads/benchmarks/phoenix/Default.mk
