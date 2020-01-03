//
// Created by Philipp Tkachev on 2020-01-02.
//

#include "PipelineSubscriber.h"

#include <boost/lexical_cast.hpp>
#include <utility>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace AmqpClient {

    PipelineSubscriber::PipelineSubscriber(Channel::ptr_t channel,
            const std::string &iExchangeName,
            const std::string &iQueue,
            const std::string &oExchangeName,
            const std::string &oQueue
            ) : m_channel(std::move(channel))
    {
        m_iExchangeName = iExchangeName;
        m_iQueue = iQueue;
        m_oExchangeName = oExchangeName;
        m_oQueue = oQueue;

        m_channel->DeclareExchange(m_iExchangeName, Channel::EXCHANGE_TYPE_FANOUT);
        m_channel->DeclareQueue(m_iQueue, false, true);
        m_channel->BindQueue(m_iQueue, m_iExchangeName, "");

        m_channel->DeclareExchange(m_oExchangeName, Channel::EXCHANGE_TYPE_FANOUT);
        m_channel->DeclareQueue(m_oQueue, false, true);
        m_channel->BindQueue(m_oQueue, m_oExchangeName, "");

        m_channel->BasicConsume(m_iQueue, m_iQueue);
    }

    PipelineSubscriber::~PipelineSubscriber()
    {
    }

    std::string PipelineSubscriber::WaitForMessageString(int timeout)
    {
        BasicMessage::ptr_t incoming = WaitForMessage(timeout);
        return incoming->Body();
    }

    void PipelineSubscriber::Publish(const BasicMessage::ptr_t message) {
        m_channel->BasicPublish(m_oExchangeName, "", message);
    }

    BasicMessage::ptr_t PipelineSubscriber::WaitForMessage(int timeout)
    {
        Envelope::ptr_t envelope;
        m_channel->BasicConsumeMessage(m_iQueue, envelope, timeout);
        return envelope->Message();
    }
}