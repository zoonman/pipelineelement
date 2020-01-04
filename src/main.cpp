/***
 * WARNING! THIS IS PROOF OF CONCEPT IMPLEMENTATION AND REQUIRES A PROPER REWRITE!
 *
 * (с) 2020, Analog Devices Inc.
 * Author: Philipp Tkachev <philipp.tkachev@analog.com>
 */
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "PipelineSubscriber.h"

/**
 * Name for the collection
 */
auto const DATA_COLLECTION_NAME = "testcollection";

using namespace AmqpClient;
using namespace std;

/**
 * Dump data from MongoDB's collection
 * @param collection
 */
void dumpCollection(mongocxx::collection collection) {
    auto cursor = collection.find({});
    for (auto&& doc : cursor) {
        spdlog::get("console")->info(bsoncxx::to_json(doc));
    }
}

/**
 * Check when env is not set and flip the flag!
 */
auto unmetRequirements = false;
/**
 * Get an Environment variable
 * @param name
 * @return
 */
const char * getEnvVar(const char * name) {
    const char *value = std::getenv(name);
    if (!value) {
        spdlog::get("stderr")->error("Define %v environment variable!", name);
        unmetRequirements = true;
    } else {
        spdlog::get("console")->trace("Environment {} = {}", name, value);
    }
    return value;
}


int main(int, char**) {
    auto console = spdlog::stdout_color_st("console", spdlog::color_mode::always);
    auto err_logger = spdlog::stderr_color_mt("stderr");
    // Let's read our enviroment variables
    const char *debug = std::getenv("DEBUG");
    if (debug && strncmp(debug, "false", 5) != 0 && strncmp(debug, "0", 1) != 0) {
        // ideally we want to see here a proper log levels
        // but this is ok to enable full logs output when we set debug flag
        console->set_level(spdlog::level::trace);
        err_logger->set_level(spdlog::level::trace);
    }
    // Initialize the rest of parameters
    auto mongoUrl = getEnvVar("MONGODB_URL");
    auto brokerUrl = getEnvVar("RABBITMQ_URL");
    auto inputExchangeName = getEnvVar("INPUT_EXCHANGE_NAME");
    auto inputQueueName = getEnvVar("INPUT_QUEUE_NAME");
    auto outputExchangeName = getEnvVar("OUTPUT_EXCHANGE_NAME");
    auto outputQueueName = getEnvVar("OUTPUT_QUEUE_NAME");

    if (unmetRequirements) {
        return 1;
    }

    mongocxx::instance inst{};
    spdlog::get("console")->info("Your MONGODB_URL is: {}", mongoUrl);

    auto mongo_uri = mongocxx::uri{mongoUrl};
    mongocxx::client conn{mongo_uri};

    // reference to DB
    auto database = conn[mongo_uri.database()];

    // your data collection
    auto collection = database[DATA_COLLECTION_NAME];

    // Let's have a look what's inside
    // Normally we don't need it.
    dumpCollection(collection);

    Channel::ptr_t channel;
    channel = Channel::CreateFromUri(brokerUrl);

    // Initialize our rabbitmq connector
    boost::shared_ptr<PipelineSubscriber> sub = PipelineSubscriber::Create(channel,
            inputExchangeName,
            inputQueueName,
            outputExchangeName,
            outputQueueName
    );

    // our dead loop for message handling
    while (true) {
        Envelope::ptr_t envelope;
        // Here we are waiting for new message to come in from RabbitMQ.
        // It is automatically ack'ing the message and it is better to do it manually
        // to ensure that there is no lost messages.
        // Ideally it should be split to a proper BasicConsume implementation allowing multithreading etc.
        bool flag = channel->BasicConsumeMessage(inputQueueName, envelope, -1);
        if (!flag) {
            spdlog::get("console")->warn("Message consumption timed out");
            break;
        }
        // Hooray! We got the message and now we can parse it and move on.
        auto incomingMessage = envelope->Message();
        spdlog::get("console")->info("We got the message {}", incomingMessage->Body());

        if (incomingMessage->ContentTypeIsSet()) {
            if ("application/json" == incomingMessage->ContentType()) {
                // Do your work here
                // for example add message validation, decompression, deserialization etc

                // Some protobuf here feels natural
                // see https://github.com/kingback1/SimpleAmqpClient-examples/tree/master/examples/consume

                // Let's drop a whole document into db
                auto document = bsoncxx::from_json(incomingMessage->Body());
                auto insertionResult = collection.insert_one(document.view());
                // OK, what we got in here?
                dumpCollection(collection);
                // Getting an inserted id is not easy but doable if you have acknowledged writes
                auto _id = insertionResult->inserted_id().get_oid().value.to_string();

                spdlog::get("console")->info("Inserted document with ID: {}", _id);

                // Now we will construct a new message for the next consumer in the chain
                auto outgoingMessage = BasicMessage::Create();
                // Technically you don't need to define an content type but it can drastically simplify your application
                outgoingMessage->ContentType("application/json");
                // RabbitMQ expects the binary data serialized to a string implementation
                // Ideally you want to use JSON for non-binary data or ProtoBuf for binary-friendly data
                outgoingMessage->Body(R"({"id":")" + _id + "\"}");
                sub->Publish(outgoingMessage);
            }
        }
    }
}
