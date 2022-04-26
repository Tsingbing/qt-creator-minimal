#ifndef __TGSUB_H__
#define __TGSUB_H__
#include <exception>
#include <functional>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include "qos/qos_profile_conf.hpp"

template <typename SubMsgType>
class TGReaderListener : public eprosima::fastdds::dds::DataReaderListener
{
public:
    using CallBackFunc = std::function<void(const SubMsgType &)>;
    using RpcCallBackFunc = std::function<void(const SubMsgType &, const eprosima::fastrtps::rtps::SampleIdentity &)>;

    TGReaderListener(/* args */)
    {
    }
    ~TGReaderListener()
    {
    }

    void SetCallBack(const CallBackFunc &func)
    {
        func_ = func;
    }
    void SetRPCCallBack(const RpcCallBackFunc &func)
    {
        rpcCallBack = func;
    }

private:
    void on_data_available(eprosima::fastdds::dds::DataReader *reader) override
    {
        FASTDDS_SEQUENCE(DataSeq, SubMsgType);
        DataSeq data;
        eprosima::fastdds::dds::SampleInfoSeq infos;

        while (reader->take(data, infos, 5) == ReturnCode_t::RETCODE_OK)
        {
            for (int i = 0; i < infos.length(); i++)
            {
                if (infos[i].valid_data && (rpcCallBack != nullptr))
                {
                    const SubMsgType &tmp = data[i];
                    const eprosima::fastrtps::rtps::SampleIdentity &requestId = infos[i].related_sample_identity;
                    rpcCallBack(tmp, requestId);
                }
                else if (infos[i].valid_data && (func_ != nullptr))
                {
                    const SubMsgType &tmp = data[i];
                    func_(tmp);
                }
            }
        }
        reader->return_loan(data, infos);
    }

private:
    CallBackFunc func_ = nullptr;
    RpcCallBackFunc rpcCallBack = nullptr;
};

template <typename SubMsgType>
class TGSub
{
public:
    TGSub(eprosima::fastdds::dds::DomainParticipant *participant, eprosima::fastdds::dds::Topic *topic,
        const QosProfile &qos = QOS_PROFILE_DEFAULT, eprosima::fastdds::dds::Subscriber *subscriber = nullptr)
        : participant_(participant), subscriber_(subscriber), subTopic_(topic)
    {
        if (subscriber_ == nullptr)
        {
            subscriber_ = participant_->create_subscriber(eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT);
            subscriberIsSelf = true;
        }
        listener_ = new TGReaderListener<SubMsgType>();

        eprosima::fastdds::dds::DataReaderQos rqos;
        rqos.endpoint().history_memory_policy = eprosima::fastrtps::rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;
        rqos.reliability().kind = qos.reliability;
        rqos.history().kind = qos.history;
        rqos.history().depth = qos.history_depth;
        rqos.durability().kind = qos.durability;
        reader_ = subscriber_->create_datareader(subTopic_, rqos, listener_);
        if (reader_ == nullptr)
        {
            throw std::runtime_error("create_datareader error");
        }
    }
    ~TGSub()
    {
        if (reader_ != nullptr)
        {
            subscriber_->delete_datareader(reader_);
        }
        if (subscriberIsSelf && subscriber_ != nullptr)
        {
            participant_->delete_subscriber(subscriber_);
        }
        if (listener_ != nullptr)
        {
            delete listener_;
        }
    }

    void SetCallBack(const typename TGReaderListener<SubMsgType>::CallBackFunc &func)
    {
        dynamic_cast<TGReaderListener<SubMsgType> *>(listener_)->SetCallBack(func);
    }

    void SetRPCCallBack(const typename TGReaderListener<SubMsgType>::RpcCallBackFunc &func)
    {
        dynamic_cast<TGReaderListener<SubMsgType> *>(listener_)->SetRPCCallBack(func);
    }

    const eprosima::fastrtps::rtps::GUID_t &GetReaderGuid() const
    {
        return reader_->guid();
    }

private:
    eprosima::fastdds::dds::DomainParticipant *participant_;
    eprosima::fastdds::dds::Subscriber *subscriber_;
    eprosima::fastdds::dds::Topic *subTopic_;
    eprosima::fastdds::dds::DataReader *reader_;
    bool subscriberIsSelf = false;
    eprosima::fastdds::dds::DataReaderListener *listener_ = nullptr;
};

#endif