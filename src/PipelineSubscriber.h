#ifndef PIPELINEELEMENT_SIMPLESUBSCRIBER_H
#define PIPELINEELEMENT_SIMPLESUBSCRIBER_H

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Channel.h"
#include "SimpleAmqpClient/Util.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>


namespace AmqpClient {

    class SimpleSubscriber {
    public:
        typedef boost::shared_ptr<SimpleSubscriber> ptr_t;

        static ptr_t Create(Channel::ptr_t channel, const std::string &exchange_name, const std::string &queue_name)
        {
            return boost::make_shared<SimpleSubscriber>(channel, exchange_name, queue_name);
        }

        explicit SimpleSubscriber(Channel::ptr_t channel, const std::string &exchange_name, const std::string &queue_name);

        virtual ~SimpleSubscriber();

        std::string WaitForMessageString(int timeout = -1);
        BasicMessage::ptr_t WaitForMessage(int timeout = -1);

        void Publish(const BasicMessage::ptr_t message);

    private:
        Channel::ptr_t m_channel;
        std::string m_consumerQueue;
    };
}


#endif //PIPELINEELEMENT_SIMPLESUBSCRIBER_H
