use candid::Encode;
use ic_agent::{agent::{http_transport::ReqwestHttpReplicaV2Transport}, Agent, identity::AnonymousIdentity};
use ic_utils::Canister;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
struct GreetingResponse {
    greeting: String,
}

#[tokio::main]
async fn main() {
    let name = "Zondax";
    let canister_id = "rrkah-fqaaa-aaaaa-aaaaq-cai";
    let url = "http://127.0.0.1:4943";


    let transport = ReqwestHttpReplicaV2Transport::create(url)
        .expect("Failed to create Reqwest transport");
    
    let agent = Agent::builder()
        .with_transport(transport)
        .with_identity(AnonymousIdentity)
        .build()
        .expect("Failed to build agent");
    
    agent.fetch_root_key()            
             .await
             .expect("Couldn't fetch root key");
    
    let canister = Canister::builder()
         .with_agent(&agent)
         .with_canister_id(canister_id)
         .build()
         .unwrap();

    let response= canister
         .update_("greet")
         .with_arg_raw(Encode!(&name).unwrap())
         .build::<(String,)>()
         .call_and_wait()
         .await;

   println!("{:?}", response);
}
