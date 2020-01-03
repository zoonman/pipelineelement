#ifndef PIPELINEELEMENT_PIPELINESUBSCRIBER_H
#define PIPELINEELEMENT_PIPELINESUBSCRIBER_H

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Channel.h"
#include "SimpleAmqpClient/Util.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>


namespace AmqpClient {

    class PipelineSubscriber {
    public:
        typedef boost::shared_ptr<PipelineSubscriber> ptr_t;

        static ptr_t Create(Channel::ptr_t channel,
                const std::string &iExchangeName,
                            const std::string &iQueue,
                            const std::string &oExchangeName,
                            const std::string &oQueue)
        {
            return boost::make_shared<PipelineSubscriber>(channel, iExchangeName, iQueue, oExchangeName, oQueue);
        }

        explicit PipelineSubscriber(Channel::ptr_t channel,
                const std::string &iExchangeName,
                const std::string &iQueue,
                const std::string &oExchangeName,
                const std::string &oQueue);

        virtual ~PipelineSubscriber();

        std::string WaitForMessageString(int timeout = -1);
        BasicMessage::ptr_t WaitForMessage(int timeout = -1);

        void Publish(const BasicMessage::ptr_t message);

    private:
        Channel::ptr_t m_channel;
        std::string m_iExchangeName;
        std::string m_oExchangeName;
        std::string m_iQueue;
        std::string m_oQueue;
    };
}


#endif //PIPELINEELEMENT_PIPELINESUBSCRIBER_H
