#pragma once
#include "tgpub.hpp"
#include "tgsub.hpp"
#include "tgrpc.hpp"
#include <fastdds/dds/topic/Topic.hpp>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef ALIGN_WITH_ROS2
#include "ParticipantEntitiesInfoPubSubTypes.h"
#include "ParticipantEntitiesInfo.h"

#define ROS_DISCOVERY_TOPIC "ros_discovery_info"

using namespace rmw_dds_common::msg;

enum EntityType {
  Puber = 0,
  Suber = 1,
};
#endif

using RegObjFunc = std::function<void(void)>;

class NodeImpl
{

public:
    NodeImpl(std::string name) : m_name(name)
    {
        eprosima::fastdds::dds::DomainParticipantQos participantQos;
        participantQos.wire_protocol().builtin.typelookup_config.use_server = true;

        participantQos.name(m_name);

        m_participant = eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(0, participantQos);
        if (m_participant == nullptr)
        {
            std::runtime_error exception("create_participant failed!");
            throw exception;
        }

#ifdef ALIGN_WITH_ROS2
        m_discovery_sender = CreateWriterImpl<ParticipantEntitiesInfoPubSubType>(ROS_DISCOVERY_TOPIC, QOS_PROFILE_DISCOVERY);
#endif
    };

    ~NodeImpl(void)
    {
        if (m_participant)
        {
            for (auto &v : m_topic)
            {
                m_participant->delete_topic(v.second);
            }
            eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(m_participant);
        }
    };

    void Getguid(std::array<uint8_t, 24>& guid_byte_array)
    {
        constexpr auto prefix_size = sizeof(m_participant->guid().guidPrefix.value);
        memcpy(&guid_byte_array, &m_participant->guid().guidPrefix.value, prefix_size);
        memcpy(&guid_byte_array[prefix_size], &m_participant->guid().entityId.value, m_participant->guid().entityId.size);
    }

#ifdef ALIGN_WITH_ROS2
    void SendDiscoveryInfo(eprosima::fastrtps::rtps::GUID_t guid, EntityType type)
    {
        std::array<uint8_t, 24> id = {0};
        Getguid(id);
        ParticipantEntitiesInfo tmp;

        Gid ggid;
        ggid.data(id);
        tmp.ggid(ggid);

        NodeEntitiesInfo info;
        info.node_name(m_name);
        info.node_namespace("/");

        constexpr auto prefix_size = sizeof(guid.guidPrefix.value);

        std::array<uint8_t, 24> pid = {0};
        memcpy(&pid, &guid.guidPrefix.value, prefix_size);
        memcpy(&pid[prefix_size], &guid.entityId.value, guid.entityId.size);

        Gid reader_gid;
        reader_gid.data(pid);
        std::vector<Gid> gid_vet;
        gid_vet.push_back(reader_gid);

        switch(type)
        {
            case Puber:
                info.writer_gid_seq(gid_vet);
                break;
            case Suber:
                info.reader_gid_seq(gid_vet);
                break;
            default:
                return;
        }

        std::vector<rmw_dds_common::msg::NodeEntitiesInfo> send_msg;
        send_msg.push_back(info);
        tmp.node_entities_info_seq(send_msg);
        m_discovery_sender->Write(tmp);
    }
#endif

    void RegisterTopic(std::string topic, std::shared_ptr<eprosima::fastdds::dds::TypeSupport> type, RegObjFunc reg = nullptr)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        if (m_topic.find(topic) == m_topic.end())
        {
            if(reg != nullptr)
            {
                reg();
            }
            type->get()->auto_fill_type_information(true);
            type->get()->auto_fill_type_object(false);
            type->register_type(m_participant);
            m_topic.insert(std::make_pair(topic, m_participant->create_topic(topic, type->get_type_name(), eprosima::fastdds::dds::TOPIC_QOS_DEFAULT)));
            // std::cout << type->get_type_name() << std::endl;
        }
    }

    template <typename MsgPubSubType>
    auto CreateReaderImpl(std::string topic, const QosProfile &qos, RegObjFunc reg = nullptr)
        -> std::shared_ptr<TGSub<typename MsgPubSubType::type>>
    {
        std::shared_ptr<eprosima::fastdds::dds::TypeSupport> type = std::make_shared<eprosima::fastdds::dds::TypeSupport>(new MsgPubSubType());
        RegisterTopic(topic, type, reg);
        std::shared_ptr<TGSub<typename MsgPubSubType::type>> suber(new TGSub<typename MsgPubSubType::type>(m_participant, m_topic[topic], qos));
        return suber;
    }

    template <typename MsgPubSubType>
    auto CreateReader(std::string topic, const QosProfile &qos, RegObjFunc reg = nullptr)
        -> std::shared_ptr<TGSub<typename MsgPubSubType::type>>
    {
        QosProfile qos_ = qos;
#ifdef ALIGN_WITH_ROS2
        topic = std::string("rt/") + topic;
        qos_ = QOS_PROFILE_ROSMESSAGE;
#endif
        auto ret = CreateReaderImpl<MsgPubSubType>(topic, qos_, reg);
#ifdef ALIGN_WITH_ROS2
        SendDiscoveryInfo(ret->GetReaderGuid(), EntityType::Suber);
#endif
        return ret;
    }

    template <typename MsgPubSubType>
    auto CreateWriterImpl(std::string topic, const QosProfile &qos, RegObjFunc reg = nullptr)
        -> std::shared_ptr<TGPub<typename MsgPubSubType::type>>
    {
        std::shared_ptr<eprosima::fastdds::dds::TypeSupport> type = std::make_shared<eprosima::fastdds::dds::TypeSupport>(new MsgPubSubType());
        RegisterTopic(topic, type, reg);
        std::shared_ptr<TGPub<typename MsgPubSubType::type>> puber(new TGPub<typename MsgPubSubType::type>(m_participant, m_topic[topic], qos));
        return puber;
    }

    template <typename MsgPubSubType>
    auto CreateWriter(std::string topic, const QosProfile &qos, RegObjFunc reg = nullptr) 
        -> std::shared_ptr<TGPub<typename MsgPubSubType::type>>
    {
        QosProfile qos_ = qos;
#ifdef ALIGN_WITH_ROS2
        topic = std::string("rt/") + topic;
        qos_ = QOS_PROFILE_ROSMESSAGE;
#endif
        auto ret = CreateWriterImpl<MsgPubSubType>(topic, qos_, reg);
#ifdef ALIGN_WITH_ROS2
        SendDiscoveryInfo(ret->GetWriterGuid(), EntityType::Puber);
#endif
        return ret;
    }

    template <typename RequestPubSubType,
              typename ResponsePubSubType>
    auto CreateService(const std::string &serviceName, const typename TGService<typename RequestPubSubType::type, typename ResponsePubSubType::type>::TGServiceCallback &func,
                        RegObjFunc reg = nullptr)
        -> std::shared_ptr<TGService<typename RequestPubSubType::type,
                                     typename ResponsePubSubType::type>>
    {
        using Request = typename RequestPubSubType::type;
        using Response = typename ResponsePubSubType::type;

        std::shared_ptr<eprosima::fastdds::dds::TypeSupport> requestType = std::make_shared<eprosima::fastdds::dds::TypeSupport>(new RequestPubSubType());
        std::string requestTopic = serviceName + std::string("_REQUEST_SERVICE_");
        RegisterTopic(requestTopic, requestType, reg);

        std::shared_ptr<eprosima::fastdds::dds::TypeSupport> responseType = std::make_shared<eprosima::fastdds::dds::TypeSupport>(new ResponsePubSubType());
        std::string responseTopic = serviceName + std::string("_RESPONSE_SERVICE_");
        RegisterTopic(responseTopic, responseType, reg);

        std::shared_ptr<TGService<Request, Response>> service(new TGService<Request, Response>(m_participant, m_topic[requestTopic], m_topic[responseTopic], func));
        return service;
    }

    template <typename RequestPubSubType, typename ResponsePubSubType>
    auto CreateClient(const std::string &serviceName, RegObjFunc reg = nullptr)
        -> std::shared_ptr<TGClient<typename RequestPubSubType::type,
                                    typename ResponsePubSubType::type>>
    {
        using Request = typename RequestPubSubType::type;
        using Response = typename ResponsePubSubType::type;

        std::shared_ptr<eprosima::fastdds::dds::TypeSupport> requestType = std::make_shared<eprosima::fastdds::dds::TypeSupport>(new RequestPubSubType());
        std::string requestTopic = serviceName + std::string("_REQUEST_SERVICE_");
        RegisterTopic(requestTopic, requestType, reg);

        std::shared_ptr<eprosima::fastdds::dds::TypeSupport> responseType = std::make_shared<eprosima::fastdds::dds::TypeSupport>(new ResponsePubSubType());
        std::string responseTopic = serviceName + std::string("_RESPONSE_SERVICE_");
        RegisterTopic(responseTopic, responseType, reg);

        std::shared_ptr<TGClient<Request, Response>> client(new TGClient<Request, Response>(m_participant, m_topic[requestTopic], m_topic[responseTopic]));
        return client;
    }

private:
    std::string m_name;
    std::mutex m_mutex;
    eprosima::fastdds::dds::DomainParticipant *m_participant = nullptr;
    std::map<std::string, eprosima::fastdds::dds::Topic *> m_topic;

#ifdef ALIGN_WITH_ROS2
    std::shared_ptr<TGPub<rmw_dds_common::msg::ParticipantEntitiesInfo>> m_discovery_sender;
#endif
};
