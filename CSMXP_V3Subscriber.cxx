// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file CSMXP_V3Subscriber.cpp
 * This file contains the implementation of the subscriber functions.
 */

#include "CSMXP_V3Subscriber.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>

// Include CSMXP_V3 type support
#include "CSMXP_V3.h"
#include "CSMXP_V3PubSubTypes.h"

#include <thread>
#include <iostream>
#include <chrono>

using namespace eprosima::fastdds::dds;

CSMXP_V3Subscriber::CSMXP_V3Subscriber()
    : participant_(nullptr)
    , subscriber_(nullptr)
    , topic_(nullptr)
    , reader_(nullptr)
    , type_(new CSMXP_V3PubSubType())
    , debugMode_(false)
    , isRunning_(false)
{
}

CSMXP_V3Subscriber::~CSMXP_V3Subscriber()
{
    if (isRunning_)
    {
        stop();
    }
    uninit();
}

bool CSMXP_V3Subscriber::init(int domainId, std::string topicName, std::string multicastIp, int port, int bDebug)
{
    debugMode_ = (bDebug != 0);
    listener_.debugMode = debugMode_;

    try
    {
        // Create DomainParticipant
        DomainParticipantQos participantQos = PARTICIPANT_QOS_DEFAULT;
        participantQos.name("CSMXP_V3_Subscriber");

        // Configure multicast if specified
        if (!multicastIp.empty() && port > 0)
        {
            // Note: FastDDS multicast configuration is complex and requires transport configuration
            // This is a placeholder for multicast configuration
            if (debugMode_)
            {
                std::cout << "Multicast configuration: " << multicastIp << ":" << port << std::endl;
            }
        }

        participant_ = DomainParticipantFactory::get_instance()->create_participant(
            domainId, participantQos);

        if (participant_ == nullptr)
        {
            std::cerr << "Error creating DomainParticipant" << std::endl;
            return false;
        }

        // Register type
        type_.register_type(participant_);

        // Create Topic
        topic_ = participant_->create_topic(
            topicName,
            type_->getName(),
            TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            std::cerr << "Error creating Topic" << std::endl;
            return false;
        }

        // Create Subscriber
        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

        if (subscriber_ == nullptr)
        {
            std::cerr << "Error creating Subscriber" << std::endl;
            return false;
        }

        // Configure DataReader QoS
        DataReaderQos readerQos = DATAREADER_QOS_DEFAULT;
        readerQos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
        readerQos.durability().kind = TRANSIENT_LOCAL_DURABILITY_QOS;
        readerQos.history().kind = KEEP_LAST_HISTORY_QOS;
        readerQos.history().depth = 10;

        // Create DataReader with listener
        reader_ = subscriber_->create_datareader(topic_, readerQos, &listener_);

        if (reader_ == nullptr)
        {
            std::cerr << "Error creating DataReader" << std::endl;
            return false;
        }

        if (debugMode_)
        {
            std::cout << "CSMXP_V3Subscriber initialized successfully" << std::endl;
            std::cout << "Domain ID: " << domainId << std::endl;
            std::cout << "Topic: " << topicName << std::endl;
        }

        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception in CSMXP_V3Subscriber::init: " << e.what() << std::endl;
        return false;
    }
}

void CSMXP_V3Subscriber::uninit()
{
    if (reader_ != nullptr)
    {
        subscriber_->delete_datareader(reader_);
        reader_ = nullptr;
    }

    if (topic_ != nullptr)
    {
        participant_->delete_topic(topic_);
        topic_ = nullptr;
    }

    if (subscriber_ != nullptr)
    {
        participant_->delete_subscriber(subscriber_);
        subscriber_ = nullptr;
    }

    if (participant_ != nullptr)
    {
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
        participant_ = nullptr;
    }
}

void CSMXP_V3Subscriber::run()
{
    isRunning_ = true;

    if (debugMode_)
    {
        std::cout << "CSMXP_V3Subscriber is running. Press Ctrl+C to stop..." << std::endl;
    }

    while (isRunning_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void CSMXP_V3Subscriber::stop()
{
    isRunning_ = false;
}

void CSMXP_V3Subscriber::setDataReceiverFunction(DataReceiver* recFun, void* userObject)
{
    listener_.dataReceiverFun = recFun;
    listener_.receiver = userObject;
}

int CSMXP_V3Subscriber::getPublisherCount() const
{
    return listener_.getMatchedCount();
}

bool CSMXP_V3Subscriber::isConnected() const
{
    return (participant_ != nullptr && reader_ != nullptr && listener_.getMatchedCount() > 0);
}

// SubListener implementation
void CSMXP_V3Subscriber::SubListener::on_data_available(DataReader* reader)
{
    CSMXP_V3 message;
    SampleInfo info;

    while (reader->take_next_sample(&message, &info) == ReturnCode_t::RETCODE_OK)
    {
        if (info.valid_data)
        {
            samples++;

            if (debugMode)
            {
                std::cout << "Message received. "
                         << "SRC: " << message.SRC()
                         << ", DST: " << message.DST()
                         << ", MSG size: " << message.MSG().size()
                         << ", Total samples: " << samples << std::endl;
            }

            // Call user callback function
            if (dataReceiverFun != nullptr)
            {
                dataReceiverFun(message.MSG(), receiver);
            }
        }
    }
}

void CSMXP_V3Subscriber::SubListener::on_subscription_matched(
    DataReader* reader,
    const SubscriptionMatchedStatus& info)
{
    matched = info.current_count;

    if (info.current_count_change == 1)
    {
        std::cout << "Subscriber matched. Current publishers: " << info.current_count << std::endl;
    }
    else if (info.current_count_change == -1)
    {
        std::cout << "Subscriber unmatched. Current publishers: " << info.current_count << std::endl;
        if (info.current_count == 0)
        {
            std::cout << "No publishers available" << std::endl;
        }
    }
}
