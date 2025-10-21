#include<iostream>
#include<vector>
#include<thread>
#include<chrono>
#include<ctime>
#include<cstring>


#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include<fastdds/dds/publisher/qos/DataWriterQos.hpp>

#include"CSMXP_V3.h"
#include"CSMXP_V3PubSubTypes.h"

using namespace eprosima::fastdds::dds;
// 将十六进制字符串转换为字节数组
std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

int main(int argc, char** argv) {
    std::cout << "========================================" << std::endl;
    std::cout << "CSMXP测试发送程序启动" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 您的测试报文
    std::string hexData = "03014321ffffffff2c000000002c0000a8430017e064c8657dffff0101ff001c22194044000001fa28531500060724200028531506072420";
    std::vector<uint8_t> rawData = hexStringToBytes(hexData);
    
    std::cout << "准备发送的报文大小: " << rawData.size() << " 字节" << std::endl;
    
    // 打印报文内容
    std::cout << "报文内容(HEX): ";
    for (size_t i = 0; i < rawData.size() && i < 32; i++) {
        printf("%02X ", rawData[i]);
        if ((i + 1) % 16 == 0) std::cout << std::endl << "                ";
    }
    std::cout << std::endl;
    
    //创建参与者
    try {
        DomainParticipant* participant=DomainParticipantFactory::get_instance()->create_participant(0,PARTICIPANT_QOS_DEFAULT);
        if (participant== nullptr)
            {
	       std::cerr<<"创建DDS参与者失败";
               return -1;
            }
	std::cout<<"创建DDS参与者成功";
        //注册类型
        TypeSupport type(new CSMXP_V3PubSubType());
        type.register_type(participant);
        //创建主题
        Topic* topic=participant->create_topic("CSMXP_TimeSync",type.get_type_name(),TOPIC_QOS_DEFAULT);
        //创建发布者
        Publisher* publisher=participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        //创建DataWriter
	DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
        wqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
        wqos.durability().kind=TRANSIENT_LOCAL_DURABILITY_QOS;
        wqos.history().kind=KEEP_LAST_HISTORY_QOS;
        wqos.history().depth=10;
        DataWriter* writer=publisher->create_datawriter(topic,wqos);
	if(!writer){
		std::cerr<<"创建DataWriter失败"<<std::endl;
		return -1;
	}
	std::cout<<"创建DataWriter成功"<<std::endl;
        std::cout<<"等待订阅者"<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        //发送数据
        CSMXP_V3 message;
        message.MSG(rawData);
        std::cout<<"开始发送"<<std::endl;
            if(writer->write(&message)){
                std::cout<<"发送成功"<<std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        publisher->delete_datawriter(writer);
        participant->delete_publisher(publisher);
        participant->delete_topic(topic);
        DomainParticipantFactory::get_instance()->delete_participant(participant);
        std::cout<<"完成"<<std::endl;
    } catch (const std::exception& e) {
	    std::cerr<<"DDS初始化异常"<<e.what();
        return -1;

    }
    return 0;
}

