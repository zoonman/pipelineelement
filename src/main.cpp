#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include "SimpleSubscriber.h"

using namespace AmqpClient;

int main(int, char**) {
    mongocxx::instance inst{};

    if (const char* mongo_url = std::getenv("MONGO_URL")) {
        std::cout << "Your MONGO_URL is: " << mongo_url << '\n';

        mongocxx::client conn{mongocxx::uri{}};

        bsoncxx::builder::stream::document document{};

        auto collection = conn["testdb"]["testcollection"];
        document << "hello" << "world";

        collection.insert_one(document.view());
        auto cursor = collection.find({});

        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }

        if (const char *broker_url = std::getenv("RABBITMQ_URL")) {
            Channel::ptr_t channel;
            channel = Channel::Create(broker_url);

            boost::shared_ptr<SimpleSubscriber> sub = SimpleSubscriber::Create(channel, "wt");
        };
    }
}
