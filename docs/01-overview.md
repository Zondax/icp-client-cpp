# Overview

IC is one of the world’s fastest general-purpose blockchain web3 is being built on, with a big and active community.
Among its advantages the most important one definitely, is its capability of, literally, allowing the developers to deploy anything on chain in a form of a smart contract.
This project creates a C++ wrapper for the IC rust Agent, which we believe can help to bring more developers into IC.

### Why C++ ?

In a simple way, in IC developers can deploy their app either on their local machine or live on chain, and the agent is the way the developer and users have to interact with the app. This means the Agent should follow the app interface specifications, making it convenient to have many languages available to create the agent.
To facilitate future maintenance, we decided to build a wrapper for the IC Rust Agent. We wrote a middle software layer that takes the rust functions from the rust agent and exports into C functions that are later used to build the C++ interface.

We have chosen C++, because is a fast and efficient language that has many applications, and the C base layer we also have written can even be used later as a starting point for other different languages like Python.

### Who benefits with this project ?

Every C++ developer can benefit from this library, gaining the opportunity to deploy their C++ projects on IC.

With this library the developer does not need to have all the knowledge on how to deal with candid files and candid types. ICP-Client-C++ offers a Generator tool capable of translating a candid file into a C++ header file with services in a form of C++ functions with C++ native types.

One of the main communities that can benefit from our project is the gaming developers community. All their C++ native projects can have a IC interface running on chain and they can use our C++ wrapper to interact with it.
