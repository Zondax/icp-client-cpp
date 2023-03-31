cp ../ic-agent-wrapper/bindings.h test.h

gcc -o test \
    -L ../ic-agent-wrapper/target/release/ -lic_agent_wrapper\
    -I - \
    test.c 

./test
