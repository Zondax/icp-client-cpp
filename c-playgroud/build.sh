cp ../ic-agent-ffi/bindings.h test.h

gcc -o test \
    -L ../ic-agent-ffi/target/release/ -lic_agent_ffi\
    -I - \
    test.c 
