# C++ Pipeline Element

A simplest implementation of Cradle Platform
Pipeline Element in C++.

## Getting Started

### Toolchain

- clang
- gcc
- make
- cmake
- Docker

### Dependencies

- boost
- [rabbitmq-c](https://github.com/alanxz/rabbitmq-c)
- [libmongoc](https://github.com/mongodb/mongo-c-driver)
- [mongo-cxx-driver](http://mongocxx.org/mongocxx-v3/installation/)
- [SimpleAmqpClient](https://github.com/alanxz/SimpleAmqpClient)
    
### Local Building

Building to run project locally from CLI
```shell script
make
```

### Docker Image Building

We use Docker images as part of the platform therefore your
workflow will heavily depend upon using this tool.

 
```shell script
docker build . -t analogdevicesinc/cxx-pipeline-processor:latest
```

#### Start MongoDB locally
```shell script
docker run -d --hostname mongohost  --name ld-mongo -p 27017:27017 -v ~/data:/data/db mongo
```
GUI client for MongoDB https://www.robomongo.org/

#### Start RabbitMQ locally
```shell script
docker run -d --hostname rabbithost --name ld-rabbit -p 15672:15672 -p 5672:5672 rabbitmq:3-management
```
Access RabbitMQ UI http://localhost:15672/

Login: `guest`, password: `guest`.

#### Run built container
```shell script
docker run --env-file=./.env  analogdevicesinc/cxx-pipeline-processor:latest 
```
Always tag your builds properly and make sure that those align with your code tags.
Use [semantic versioning](https://semver.org/) for your project.
 
#### View all cmake variables
```shell script
cmake -LAH
```
