find . -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.cc" -o -name "*.cpp" -o -name "Makefile" -o -name "*.inc" > cscope.files
cscope -Rbkq -i cscope.files
ctags -R .
