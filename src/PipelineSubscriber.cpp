//
// Created by Philipp Tkachev on 2020-01-02.
//

#include "./SimpleSubscriber.h"

#include <boost/lexical_cast.hpp>
#include <utility>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace AmqpClient {

    SimpleSubscriber::SimpleSubscriber(Channel::ptr_t channel, const std::string &exchange_name, const std::string &queue_name) : m_channel(std::move(channel))
    {
        m_consumerQueue = queue_name;

        m_channel->DeclareExchange(exchange_name, Channel::EXCHANGE_TYPE_FANOUT);
        m_channel->DeclareQueue(m_consumerQueue);
        m_channel->BindQueue(m_consumerQueue, exchange_name, "");

        m_channel->DeclareExchange("out", Channel::EXCHANGE_TYPE_FANOUT);
        m_channel->DeclareQueue("outQueue");
        m_channel->BindQueue("outQueue", "out", "");

        m_channel->BasicConsume(m_consumerQueue, m_consumerQueue);
    }

    SimpleSubscriber::~SimpleSubscriber()
    {
    }

    std::string SimpleSubscriber::WaitForMessageString(int timeout)
    {
        BasicMessage::ptr_t incoming = WaitForMessage(timeout);
        return incoming->Body();
    }

    void SimpleSubscriber::Publish(const BasicMessage::ptr_t message) {

        m_channel->BasicPublish("out", "", message);
    }

    BasicMessage::ptr_t SimpleSubscriber::WaitForMessage(int timeout)
    {
        Envelope::ptr_t envelope;
        m_channel->BasicConsumeMessage(m_consumerQueue, envelope, timeout);
        return envelope->Message();
    }
}