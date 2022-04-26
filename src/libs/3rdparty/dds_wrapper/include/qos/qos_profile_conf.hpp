#pragma once

#include <fastdds/dds/core/policy/QosPolicies.hpp>
#include <fastdds/rtps/resources/ResourceManagement.h>

struct QosProfile
{
    // LivelinessQosPolicyKind liveliness = AUTOMATIC_LIVELINESS_QOS;
    eprosima::fastdds::dds::ReliabilityQosPolicyKind reliability = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
    eprosima::fastdds::dds::HistoryQosPolicyKind history = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    int history_depth = 1;
    eprosima::fastdds::dds::DurabilityQosPolicyKind durability = eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS;
};

class QosProfileConf
{
public:
    QosProfileConf();
    virtual ~QosProfileConf();

    static QosProfile CreateQosProfile(const eprosima::fastdds::dds::ReliabilityQosPolicyKind &reliability,
                                       const eprosima::fastdds::dds::HistoryQosPolicyKind &history,
                                       const int history_depth,
                                       const eprosima::fastdds::dds::DurabilityQosPolicyKind &durability)
    {
        QosProfile qos_profile;
        qos_profile.reliability = reliability;
        qos_profile.history = history;
        qos_profile.history_depth = history_depth;
        qos_profile.durability = durability;
        return qos_profile;
    }
};

#define QOS_PROFILE_DEFAULT     QosProfileConf::CreateQosProfile(eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS, eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS, 5,    eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS)
#define QOS_PROFILE_EVENT       QosProfileConf::CreateQosProfile(eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS,    eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS, 5,    eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS)
#define QOS_PROFILE_EVENT_LOCAL QosProfileConf::CreateQosProfile(eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS,    eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS, 5,    eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS)
#define QOS_PROFILE_PERIOD      QosProfileConf::CreateQosProfile(eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS, eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS, 50,   eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS)
#define QOS_PROFILE_PERIOD_BIG  QosProfileConf::CreateQosProfile(eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS, eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS, 1000, eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS)

// align ros2
#define QOS_PROFILE_ROSMESSAGE  QosProfileConf::CreateQosProfile(eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS,    eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS, 1,    eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS)
#define QOS_PROFILE_DISCOVERY   QosProfileConf::CreateQosProfile(eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS,    eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS, 1,    eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS)