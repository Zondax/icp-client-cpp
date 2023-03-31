cp ../ic-agent-wrapper/bindings.h lib/bindings.h

gcc -o test_lib \
    -L ../ic-agent-wrapper/target/release/ -lic_agent_wrapper\
    -I  lib/ \
    test.c lib/principal.c

./test_lib
