#include <iostream>

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
    // parameters
    const char *debug = std::getenv("DEBUG");
    if (debug && strncmp(debug, "false", 5) != 0 && strncmp(debug, "0", 1) != 0) {
        console->set_level(spdlog::level::trace);
    }
    const char *mongoUrl = getEnvVar("MONGODB_URL");
    const char *brokerUrl = getEnvVar("RABBITMQ_URL");
    const char *inputExchangeName = getEnvVar("INPUT_EXCHANGE_NAME");
    const char *inputQueueName = getEnvVar("INPUT_QUEUE_NAME");
    const char *outputExchangeName = getEnvVar("OUTPUT_EXCHANGE_NAME");
    const char *outputQueueName = getEnvVar("OUTPUT_QUEUE_NAME");

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

    // let's have a look what's inside
    // delete
    dumpCollection(collection);

    Channel::ptr_t channel;
    channel = Channel::CreateFromUri(brokerUrl);

    // initialize our rabbitmq connector
    boost::shared_ptr<PipelineSubscriber> sub = PipelineSubscriber::Create(channel,
            inputExchangeName,
            inputQueueName,
            outputExchangeName,
            outputQueueName
    );

    while (true) {
        Envelope::ptr_t envelope;
        bool flag = channel->BasicConsumeMessage(inputQueueName, envelope, -1);
        if (!flag) {
            spdlog::get("console")->warn("Message consumption timed out");
            break;
        }
        auto incomingMessage = envelope->Message();

        spdlog::get("console")->info("We got the message {}", incomingMessage->Body());

        if (incomingMessage->ContentTypeIsSet()) {
            if("application/json" == incomingMessage->ContentType()) {
                // do your work here
                // for example add message validation, decompression etc

                // let's drop a whole document into db
                auto document = bsoncxx::from_json(incomingMessage->Body());
                auto insertionResult = collection.insert_one(document.view());
                dumpCollection(collection);
                auto _id = insertionResult->inserted_id().get_oid().value.to_string();

                spdlog::get("console")->info("Inserted document with ID: {}", _id);

                // let's forward message to the exchange
                auto outgoingMessage = BasicMessage::Create();
                outgoingMessage->ContentType("application/json");
                outgoingMessage->Body(R"({"id":")" + _id + "\"}");
                sub->Publish(outgoingMessage);
            }
        }
    }
}
