
CMD_ARGS=$(cat Makefile | grep "TEST_ARGS" | cut -d" "  -f 3- )


echo "$CMD_ARGS"
