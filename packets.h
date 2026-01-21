#if !defined PACKETS_H
#define PACKETS_H

#include <inttypes.h>

#include "axis.h"


struct ControllerPacket
{
	uint64_t sendTime_ms;
	uint64_t last_packet_rx_time_message;
	Axises speedTarget;

	void serialize(uint8_t* data) //size of data should be least 28
	{
		*reinterpret_cast<uint64_t*>(&data[0]) = sendTime_ms;
		*reinterpret_cast<uint64_t*>(&data[8]) = last_packet_rx_time_message;
		for(size_t i = 0; i < 6; ++i)
		{
			*reinterpret_cast<int16_t*>(&data[16 + i * 2]) = speedTarget[i];
		}
	}
	static ControllerPacket deserialize(const uint8_t* data) //size of data should be least 28
	{
		ControllerPacket packet;
		packet.sendTime_ms = *reinterpret_cast<const uint64_t*>(&data[0]);
		packet.last_packet_rx_time_message = *reinterpret_cast<const uint64_t*>(&data[8]);
		for(size_t i = 0; i < 6; ++i)
		{
			packet.speedTarget[i] = *reinterpret_cast<const int16_t*>(&data[16 + i * 2]);
		}
		return packet;
	}
	static constexpr size_t serializedSize()
	{
		return 28;
	}
};

struct ControllerPacketBuffer
{
private:
	uint8_t buf[ControllerPacket::serializedSize()];
public:
	uint8_t* get()
	{
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct SubmarinePacket
{
	uint64_t sendTime_ms;
	uint64_t last_packet_rx_time_message;
	Axises currentSpeed;
	Axises currentPos;
	float depth;

	void serialize(uint8_t* data) // size of data should be least 44
	{
		*reinterpret_cast<uint64_t*>(&data[0]) = sendTime_ms;
		*reinterpret_cast<uint64_t*>(&data[8]) = last_packet_rx_time_message;
		for(size_t i = 0; i < 6; ++i)
		{
			*reinterpret_cast<int16_t*>(&data[16 + i * 2]) = currentSpeed[i];
			*reinterpret_cast<int16_t*>(&data[28 + i * 2]) = currentPos[i];
		}
		*reinterpret_cast<const float*>(&data[40]) = packet.depth;
	}
	static SubmarinePacket deserialize(const uint8_t* data) // size of data should be least 44
	{
		SubmarinePacket packet;
		packet.sendTime_ms = *reinterpret_cast<const uint64_t*>(&data[0]);
		packet.last_packet_rx_time_message = *reinterpret_cast<const uint64_t*>(&data[8]);
		for(size_t i = 0; i < 6; ++i)
		{
			packet.currentSpeed[i] = *reinterpret_cast<const int16_t*>(&data[16 + i * 2]);
			packet.currentPos[i] = *reinterpret_cast<const int16_t*>(&data[28 + i * 2]);
		}
		packet.depth = *reinterpret_cast<const float*>(&data[40]);
		return packet;
	}

	static constexpr size_t serializedSize()
	{
		return 44;
	}
};


#endif