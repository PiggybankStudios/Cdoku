/*
File:   gy_network_bus.h
Author: Taylor Robbins
Date:   07\23\2023
Description:
	** A network bus is a service that runs on top of a BufferedSocket_t and provides various common
	** communication patterns, like retrying packets, periodically synchronized state
*/

//TODO: Convert this file over to using scratch buffers and only being compiled if the scratch buffers macro was defined

#ifndef _GY_NETWORK_BUS_H
#define _GY_NETWORK_BUS_H

#if SOCKETS_SUPPORTED

//TODO: Add all the actual includes we need for this file
#include "gy_extras.h"

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define NETWORK_BUS_RESERVED_BASE_CMD         0x00000000
#define NETWORK_BUS_CLIENT_TO_SERVER_BASE_CMD 0x00000010
#define NETWORK_BUS_SERVER_TO_CLIENT_BASE_CMD 0x00000020
#define NETWORK_BUS_UNRESERVED_BASE_CMD       0x00000030

#define NETWORK_BUS_ATTN_STR         "~!#@"
#define NETWORK_BUS_ATTN_STR_LENGTH  4 //bytes

#define NETWORK_BUS_INCOMPLETE_CMD_TIMEOUT  1000 //ms
#define NETWORK_BUS_CONNECT_CMD_TIMEOUT     5000 //ms
#define NETWORK_BUS_RETRY_DELAY             1000 //ms

#define NETWORK_BUS_DEFAULT_MAX_NUM_TRIES  4 //tries

// +--------------------------------------------------------------+
// |                         Enumerations                         |
// +--------------------------------------------------------------+
enum NetworkBusCmd_t
{
	NetworkBusCmd_None = 0,
	NetworkBusRsp_None = 0,
	
	NetworkBusCmd_NewClient = NETWORK_BUS_CLIENT_TO_SERVER_BASE_CMD,
	NetworkBusCmd_Disconnect,
	NetworkBusCmd_Success,
	NetworkBusCmd_Failure,
	NetworkBusCmd_Ping,
	NetworkBusCmd_Ack,
	NetworkBusCmd_Last,
	
	NetworkBusRsp_IdAssigned = NETWORK_BUS_SERVER_TO_CLIENT_BASE_CMD,
	NetworkBusRsp_Kicked,
	NetworkBusRsp_Success,
	NetworkBusRsp_Failure,
	NetworkBusRsp_Ping,
	NetworkBusRsp_Ack,
	NetworkBusRsp_Last,
};
CompileAssert(NetworkBusCmd_Last <= NETWORK_BUS_SERVER_TO_CLIENT_BASE_CMD);
CompileAssert(NetworkBusRsp_Last <= NETWORK_BUS_UNRESERVED_BASE_CMD);
#ifdef GYLIB_HEADER_ONLY
const char* GetNetworkBusCmdStr(NetworkBusCmd_t cmd);
#else
const char* GetNetworkBusCmdStr(NetworkBusCmd_t cmd)
{
	switch (cmd)
	{
		case NetworkBusCmd_None:           return "Cmd_None";
		case NetworkBusCmd_NewClient:      return "Cmd_NewClient";
		case NetworkBusCmd_Disconnect:     return "Cmd_Disconnect";
		case NetworkBusCmd_Success:        return "Cmd_Success";
		case NetworkBusCmd_Failure:        return "Cmd_Failure";
		case NetworkBusCmd_Ping:           return "Cmd_Ping";
		case NetworkBusCmd_Ack:            return "Cmd_Ack";
		case NetworkBusCmd_Last:           return "Cmd_Last";
		case NetworkBusRsp_IdAssigned:     return "Rsp_IdAssigned";
		case NetworkBusRsp_Kicked:         return "Rsp_Kicked";
		case NetworkBusRsp_Success:        return "Rsp_Success";
		case NetworkBusRsp_Failure:        return "Rsp_Failure";
		case NetworkBusRsp_Ping:           return "Rsp_Ping";
		case NetworkBusRsp_Ack:            return "Rsp_Ack";
		case NetworkBusRsp_Last:           return "Rsp_Last";
		default: return "Unknown";
	}
}
#endif

enum NetworkBusState_t
{
	NetworkBusState_None = 0,
	NetworkBusState_Disconnected,
	NetworkBusState_JustOpened,
	NetworkBusState_WaitingForClientId,
	NetworkBusState_Connected,
	NetworkBusState_NumStates,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetNetworkBusStateStr(NetworkBusState_t enumValue);
#else
const char* GetNetworkBusStateStr(NetworkBusState_t enumValue)
{
	switch (enumValue)
	{
		case NetworkBusState_None:               return "None";
		case NetworkBusState_Disconnected:       return "Disconnected";
		case NetworkBusState_JustOpened:         return "JustOpened";
		case NetworkBusState_WaitingForClientId: return "WaitingForClientId";
		default: return "Unknown";
	}
}
#endif

enum ResponseCheckResult_t
{
	ResponseCheckResult_NA = 0, //not applicable, aka this cmd is not a response to our RetryPacket
	ResponseCheckResult_Failure,
	ResponseCheckResult_Success,
	ResponseCheckResult_NumResults,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetResponseCheckResultStr(ResponseCheckResult_t enumValue);
#else
const char* GetResponseCheckResultStr(ResponseCheckResult_t enumValue)
{
	switch (enumValue)
	{
		case ResponseCheckResult_NA:      return "NA";
		case ResponseCheckResult_Failure: return "Failure";
		case ResponseCheckResult_Success: return "Success";
		default: return "Unknown";
	}
}
#endif

enum NbPacketHandleFlags_t
{
	NbPacketHandleFlags_None        = 0x00,
	NbPacketHandleFlags_NbInternal  = 0x01,
	NbPacketHandleFlags_RetryPacket = 0x02,
	NbPacketHandleFlags_All         = (NbPacketHandleFlags_NbInternal | NbPacketHandleFlags_RetryPacket),
};
#ifdef GYLIB_HEADER_ONLY
const char* GetNbPacketHandleFlagsStr(NbPacketHandleFlags_t enumValue);
#else
const char* GetNbPacketHandleFlagsStr(NbPacketHandleFlags_t enumValue)
{
	switch (enumValue)
	{
		case NbPacketHandleFlags_None:        return "None";
		case NbPacketHandleFlags_NbInternal:  return "NbInternal";
		case NbPacketHandleFlags_RetryPacket: return "RetryPacket";
		case NbPacketHandleFlags_All:         return "All";
		default: return "Unknown";
	}
}
#endif

// +--------------------------------------------------------------+
// |                    Function Pointer Types                    |
// +--------------------------------------------------------------+
//NOTE: NB stands for "Network Bus"

#define NB_GET_PROGRAM_TIME_DEFINITION(functionName) u64 functionName(struct NetworkBus_t* bus)
typedef NB_GET_PROGRAM_TIME_DEFINITION(NbGetProgramTime_f);

//return true if the command was handled
#define NB_HANDLE_COMMAND_DEFINITION(functionName) bool functionName(struct NetworkBus_t* bus, struct NetworkBusClient_t* sourceClient, NbPacketHandleFlags_t handleFlags, struct NetworkCmdHeader_t header, const u8* payloadPntr)
typedef NB_HANDLE_COMMAND_DEFINITION(NbHandleCommand_f);

//return true if the command is a valid response for the RetryPacket_t
#define NB_RESPONSE_CHECK_DEFINITION(functionName) Tribool_t functionName(struct NetworkBus_t* bus, struct RetryPacket_t* packet, struct NetworkCmdHeader_t header, const u8* payloadPntr)
typedef NB_RESPONSE_CHECK_DEFINITION(NbResponseCheck_f);

#define NB_RETRY_PACKET_FINISHED_DEFINITION(functionName) void functionName(struct NetworkBus_t* bus, struct RetryPacket_t* packet, bool success, struct NetworkCmdHeader_t rspHeader, const u8* rspPayloadPntr)
typedef NB_RETRY_PACKET_FINISHED_DEFINITION(NbRetryPacketFinished_f);

#define NB_CLIENT_CONNECTED_OR_DISCONNECTED_DEFINITION(functionName) void functionName(struct NetworkBus_t* bus, struct NetworkBusClient_t* client, bool connected)
typedef NB_CLIENT_CONNECTED_OR_DISCONNECTED_DEFINITION(NbClientConnectedOrDisconnected_f);

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
START_PACK();
struct ATTR_PACKED NetworkCmdHeader_t
{
	char attn[NETWORK_BUS_ATTN_STR_LENGTH];
	u64 packetId;
	u32 cmd;
	u64 length;
	u64 clientId;
	//TODO: We should probably have some sort of CRC or similar to make sure our data is coming through uncorrupted
};
END_PACK();

struct RetryPacket_t
{
	u32 expectedRspCmd;
	u32 failureRspCmd;
	
	u64 destClientId;
	NetworkCmdHeader_t header;
	u8* payloadPntr = nullptr;
	
	u64 numTries;
	u64 maxNumTries;
	u64 lastTryTime;
};

struct NetworkBusClient_t
{
	u64 id;
	IpAddressAndPort_t address;
	u64 lastCommunicationTime;
	u64 lastPingSuccessTime;
	u64 lastPingTryTime;
};

struct NetworkBusCallbacks_t
{
	void* contextPntr;
	NbGetProgramTime_f* GetProgramTime;
	NbHandleCommand_f* HandleCommand;
	NbResponseCheck_f* ResponseCheck;
	NbRetryPacketFinished_f* RetryPacketFinished;
	NbClientConnectedOrDisconnected_f* ClientConnectedOrDisconnected;
};

struct NetworkBus_t
{
	MemArena_t* allocArena;
	MemArena_t* tempArena;
	BufferedSocket_t* socket;
	NetworkBusCallbacks_t callbacks;
	
	//both sides
	NetworkBusState_t state;
	u64 lastStateChangeTime;
	bool isServerSide;
	
	//server side
	u64 nextServerPacketId;
	u64 nextClientId;
	
	//client side
	u64 nextClientPacketId;
	u64 clientId;
	u64 lastPingSuccessTime;
	
	VarArray_t clients; //NetworkBusClient_t
	VarArray_t retryPackets; //RetryPacket_t
};

START_PACK();
struct ATTR_PACKED NetworkBusStandardPayload_t
{
	u64 sentPacketId;
	u32 sentCmd;
};
END_PACK();

#ifdef GYLIB_HEADER_ONLY
Serializable_t NewSerializable_NetworkBusStandardPayload(NetworkBusStandardPayload_t* successOrFailurePayload);
#else
Serializable_t NewSerializable_NetworkBusStandardPayload(NetworkBusStandardPayload_t* successOrFailurePayload) { return NewSerializable(SzFuncs_BinaryCopy, successOrFailurePayload); }
#endif

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeRetryPacket(NetworkBus_t* bus, RetryPacket_t* packet);
	void FreeNetworkBus(NetworkBus_t* bus);
	void CreateNetworkBus(NetworkBus_t* busOut, BufferedSocket_t* socket, MemArena_t* memArena, MemArena_t* tempArena);
	void NetworkBusSetCallbacks(NetworkBus_t* bus, void* contextPntr, NbGetProgramTime_f* getProgramTimeFunc, NbHandleCommand_f* handleCommandFunc, NbResponseCheck_f* responseCheckFunc, NbRetryPacketFinished_f* retryPacketFinishedFunc, NbClientConnectedOrDisconnected_f* clientConnectedOrDisconnectedFunc);
	bool NetworkBusSendCmdWithPayload(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u32 cmd, u64 payloadLength, const void* payloadPntr);
	bool NetworkBusSendCmdStandardPayload(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u32 cmd, u64 sentPacketId, u32 sentCmd);
	bool NetworkBusSendCmd(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u32 cmd, Serializable_t payloadSerializable = Serializable_Empty);
	bool NetworkBusSendSuccess(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u64 sentPacketId, u32 sentCmd);
	bool NetworkBusSendFailure(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u64 sentPacketId, u32 sentCmd);
	u64 NetworkBusSendRetryPacketWithPayload(NetworkBus_t* bus, NetworkBusClient_t* client, u32 cmd, u32 expectedRspCmd, u32 failureRspCmd, u64 payloadLength, const void* payloadPntr, u64 maxNumTries = NETWORK_BUS_DEFAULT_MAX_NUM_TRIES);
	u64 NetworkBusSendRetryPacket(NetworkBus_t* bus, NetworkBusClient_t* client, u32 cmd, u32 expectedRspCmd, u32 failureRspCmd, Serializable_t payloadSerializable = Serializable_Empty, u64 maxNumTries = NETWORK_BUS_DEFAULT_MAX_NUM_TRIES);
	NetworkBusClient_t* FindNetworkBusClientById(NetworkBus_t* bus, u64 clientId);
	NetworkBusClient_t* FindNetworkBusClientByAddress(NetworkBus_t* bus, IpAddressAndPort_t address);
	bool TryFindCmdInBuffer(BufferedSocketBuffer_t* buffer, u64 programTime, NetworkCmdHeader_t* headerOut, u8** payloadPntrOut);
	void PopCmdInBuffer(BufferedSocketBuffer_t* buffer, NetworkCmdHeader_t* header, u8* payloadPntr);
	void NetworkBusChangeState(NetworkBus_t* bus, NetworkBusState_t newState);
	void NetworkBusStartConnection(NetworkBus_t* bus);
	void NetworkUpdateConnectionProcess(NetworkBus_t* bus, u64 programTime);
	void NetworkBusFinishRetryPacket(NetworkBus_t* bus, RetryPacket_t* retryPacket, u64 retryPacketIndex, bool success, NetworkCmdHeader_t header, u8* payloadPntr);
	void NetworkBusHandleCmd(NetworkBus_t* bus, BufferedSocketBuffer_t* buffer, NetworkCmdHeader_t header, u8* payloadPntr);
	bool NetworkBusServiceBuffer(NetworkBus_t* bus, u64 programTime, BufferedSocketBuffer_t* buffer);
	void UpdateNetworkBus(NetworkBus_t* bus);
#else

// +--------------------------------------------------------------+
// |                  Create and Free Functions                   |
// +--------------------------------------------------------------+
void FreeRetryPacket(NetworkBus_t* bus, RetryPacket_t* packet)
{
	NotNull2(bus, packet);
	NotNull(bus->allocArena);
	FreeMem(bus->allocArena, packet->payloadPntr, packet->header.length);
	ClearPointer(packet);
}
void FreeNetworkBus(NetworkBus_t* bus)
{
	NotNull(bus);
	FreeVarArray(&bus->clients);
	VarArrayLoop(&bus->retryPackets, rIndex)
	{
		VarArrayLoopGet(RetryPacket_t, retryPacket, &bus->retryPackets, rIndex);
		FreeRetryPacket(bus, retryPacket);
	}
	FreeVarArray(&bus->retryPackets);
	ClearPointer(bus);
}

void CreateNetworkBus(NetworkBus_t* busOut, BufferedSocket_t* socket, MemArena_t* memArena, MemArena_t* tempArena)
{
	NotNull(busOut);
	NotNull2(memArena, tempArena);
	Assert(DoesMemArenaSupportPushAndPop(tempArena));
	ClearPointer(busOut);
	busOut->allocArena = memArena;
	busOut->tempArena = tempArena;
	busOut->socket = socket;
	busOut->state = NetworkBusState_JustOpened;
	
	busOut->nextServerPacketId = 1;
	busOut->nextClientPacketId = 1;
	busOut->nextClientId = 1;
	
	CreateVarArray(&busOut->retryPackets, memArena, sizeof(RetryPacket_t));
}

void NetworkBusSetCallbacks(NetworkBus_t* bus, void* contextPntr,
	NbGetProgramTime_f* getProgramTimeFunc,
	NbHandleCommand_f* handleCommandFunc,
	NbResponseCheck_f* responseCheckFunc,
	NbRetryPacketFinished_f* retryPacketFinishedFunc,
	NbClientConnectedOrDisconnected_f* clientConnectedOrDisconnectedFunc)
{
	NotNull(bus);
	NotNull(getProgramTimeFunc);
	bus->callbacks.contextPntr = contextPntr;
	bus->callbacks.GetProgramTime = getProgramTimeFunc;
	bus->callbacks.HandleCommand = handleCommandFunc;
	bus->callbacks.ResponseCheck = responseCheckFunc;
	bus->callbacks.RetryPacketFinished = retryPacketFinishedFunc;
	bus->callbacks.ClientConnectedOrDisconnected = clientConnectedOrDisconnectedFunc;
}

// +--------------------------------------------------------------+
// |                        Send Commands                         |
// +--------------------------------------------------------------+
bool NetworkBusSendCmdWithPayload(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u32 cmd, u64 payloadLength, const void* payloadPntr)
{
	NotNull2(bus, bus->socket);
	AssertIf(bus->isServerSide, client != nullptr);
	AssertIf(!bus->isServerSide, client == nullptr);
	
	if (packetId == 0)
	{
		if (bus->isServerSide)
		{
			packetId = bus->nextServerPacketId;
			bus->nextServerPacketId++;
		}
		else
		{
			packetId = bus->nextClientPacketId;
			bus->nextClientPacketId++;
		}
	}
	
	NetworkCmdHeader_t cmdHeader = {};
	MyMemCopy(&cmdHeader.attn[0], NETWORK_BUS_ATTN_STR, NETWORK_BUS_ATTN_STR_LENGTH);
	cmdHeader.cmd = cmd;
	cmdHeader.packetId = packetId;
	cmdHeader.length = payloadLength;
	cmdHeader.clientId = (bus->isServerSide ? 0 : bus->clientId);
	
	bool allSendsSucceeded = true;
	if (client != nullptr)
	{
		allSendsSucceeded &= BufferedSocketWriteTo(bus->socket, client->address, sizeof(NetworkCmdHeader_t), &cmdHeader);
		if (payloadLength > 0) { allSendsSucceeded &= BufferedSocketWriteTo(bus->socket, client->address, payloadLength, payloadPntr); }
	}
	else
	{
		allSendsSucceeded &= BufferedSocketWrite(bus->socket, sizeof(NetworkCmdHeader_t), &cmdHeader);
		if (payloadLength > 0) { allSendsSucceeded &= BufferedSocketWrite(bus->socket, payloadLength, payloadPntr); }
	}
	
	return allSendsSucceeded;
}
bool NetworkBusSendCmdStandardPayload(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u32 cmd, u64 sentPacketId, u32 sentCmd)
{
	Assert(cmd == NetworkBusRsp_Success || cmd == NetworkBusRsp_Failure || cmd == NetworkBusCmd_Success || cmd == NetworkBusCmd_Failure);
	NetworkBusStandardPayload_t standardPayload = {};
	standardPayload.sentPacketId = sentPacketId;
	standardPayload.sentCmd = sentCmd;
	PushMemMark(bus->tempArena);
	MyStr_t payloadSerialized = Serialize(NewSerializable_NetworkBusStandardPayload(&standardPayload), bus->tempArena);
	bool result = NetworkBusSendCmdWithPayload(bus, client, packetId, cmd, payloadSerialized.length, payloadSerialized.bytes);
	PopMemMark(bus->tempArena);
	return result;
}
bool NetworkBusSendCmd(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u32 cmd, Serializable_t payloadSerializable = Serializable_Empty)
{
	NotNull(bus);
	NotNull(bus->tempArena);
	bool result = false;
	if (IsEmpty(payloadSerializable))
	{
		PushMemMark(bus->tempArena);
		MyStr_t payloadSerialized = Serialize(payloadSerializable, bus->tempArena);
		result = NetworkBusSendCmdWithPayload(bus, client, packetId, cmd, payloadSerialized.length, payloadSerialized.bytes);
		PopMemMark(bus->tempArena);
	}
	else
	{
		result = NetworkBusSendCmdWithPayload(bus, client, packetId, cmd, 0, nullptr);
	}
	return result;
}
bool NetworkBusSendSuccess(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u64 sentPacketId, u32 sentCmd)
{
	NotNull(bus);
	return NetworkBusSendCmdStandardPayload(bus, client, packetId, NetworkBusRsp_Success, sentPacketId, sentCmd);
}
bool NetworkBusSendFailure(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u64 sentPacketId, u32 sentCmd)
{
	NotNull(bus);
	return NetworkBusSendCmdStandardPayload(bus, client, packetId, NetworkBusRsp_Failure, sentPacketId, sentCmd);
}

u64 NetworkBusSendRetryPacketWithPayload(NetworkBus_t* bus, NetworkBusClient_t* client, u32 cmd, u32 expectedRspCmd, u32 failureRspCmd, u64 payloadLength, const void* payloadPntr, u64 maxNumTries = NETWORK_BUS_DEFAULT_MAX_NUM_TRIES)
{
	NotNull(bus);
	AssertIf(bus->isServerSide, client != nullptr);
	AssertIf(!bus->isServerSide, client == nullptr);
	
	u64 newPacketId = 0;
	if (bus->isServerSide)
	{
		newPacketId = bus->nextServerPacketId;
		bus->nextServerPacketId++;
	}
	else
	{
		newPacketId = bus->nextClientPacketId;
		bus->nextClientPacketId++;
	}
	
	RetryPacket_t* newPacket = VarArrayAdd(&bus->retryPackets, RetryPacket_t);
	NotNull(newPacket);
	ClearPointer(newPacket);
	newPacket->expectedRspCmd = expectedRspCmd;
	newPacket->failureRspCmd = failureRspCmd;
	newPacket->maxNumTries = maxNumTries;
	newPacket->destClientId = (client != nullptr ? client->id : 0);
	newPacket->header.cmd = cmd;
	newPacket->header.length = payloadLength;
	newPacket->header.clientId = (bus->isServerSide ? 0 : bus->clientId);
	newPacket->header.packetId = newPacketId;
	newPacket->payloadPntr = AllocArray(bus->allocArena, u8, payloadLength);
	NotNull(newPacket->payloadPntr);
	MyMemCopy(newPacket->payloadPntr, payloadPntr, payloadLength);
	
	NetworkBusSendCmdWithPayload(bus, client, newPacketId, newPacket->header.cmd, newPacket->header.length, newPacket->payloadPntr);
	
	return newPacketId;
}
u64 NetworkBusSendRetryPacket(NetworkBus_t* bus, NetworkBusClient_t* client, u32 cmd, u32 expectedRspCmd, u32 failureRspCmd, Serializable_t payloadSerializable = Serializable_Empty, u64 maxNumTries = NETWORK_BUS_DEFAULT_MAX_NUM_TRIES)
{
	NotNull(bus);
	NotNull(bus->tempArena);
	u64 result = 0;
	if (IsEmpty(payloadSerializable))
	{
		PushMemMark(bus->tempArena);
		MyStr_t payloadSerialized = Serialize(payloadSerializable, bus->tempArena);
		result = NetworkBusSendRetryPacketWithPayload(bus, client, cmd, expectedRspCmd, failureRspCmd, payloadSerialized.length, payloadSerialized.bytes, maxNumTries);
		PopMemMark(bus->tempArena);
	}
	else
	{
		result = NetworkBusSendRetryPacketWithPayload(bus, client, cmd, expectedRspCmd, failureRspCmd, 0, nullptr, maxNumTries);
	}
	return result;
}

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
NetworkBusClient_t* FindNetworkBusClientById(NetworkBus_t* bus, u64 clientId)
{
	NotNull(bus);
	Assert(bus->isServerSide);
	VarArrayLoop(&bus->clients, cIndex)
	{
		VarArrayLoopGet(NetworkBusClient_t, client, &bus->clients, cIndex);
		if (client->id == clientId) { return client; }
	}
	return nullptr;
}
NetworkBusClient_t* FindNetworkBusClientByAddress(NetworkBus_t* bus, IpAddressAndPort_t address)
{
	NotNull(bus);
	Assert(bus->isServerSide);
	VarArrayLoop(&bus->clients, cIndex)
	{
		VarArrayLoopGet(NetworkBusClient_t, client, &bus->clients, cIndex);
		if (AreIpAddressAndPortsEqual(client->address, address)) { return client; }
	}
	return nullptr;
}

bool TryFindCmdInBuffer(BufferedSocketBuffer_t* buffer, u64 programTime, NetworkCmdHeader_t* headerOut, u8** payloadPntrOut)
{
	bool result = false;
	u64 numPoppedBytes = 0;
	while (buffer->used - numPoppedBytes >= sizeof(NetworkCmdHeader_t))
	{
		u64 numBytesLeft = (buffer->used - numPoppedBytes);
		NetworkCmdHeader_t* headerPntr = (NetworkCmdHeader_t*)&buffer->pntr[numPoppedBytes];
		if (MyMemCompare(headerPntr->attn, NETWORK_BUS_ATTN_STR, NETWORK_BUS_ATTN_STR_LENGTH) != 0)
		{
			numPoppedBytes++;
			continue;
		}
		
		numBytesLeft -= sizeof(NetworkCmdHeader_t);
		if (numBytesLeft >= headerPntr->length)
		{
			MyMemCopy(headerOut, headerPntr, sizeof(NetworkCmdHeader_t));
			*payloadPntrOut = &buffer->pntr[sizeof(NetworkCmdHeader_t)]; //NOTE: We don't offset by numPoppedBytes here because we're about to shift everything down after the loop
			result = true;
			break;
		}
	}
	
	if (!result && numPoppedBytes < buffer->used && TimeSinceBy(programTime, buffer->lastDataTime) >= NETWORK_BUS_INCOMPLETE_CMD_TIMEOUT)
	{
		GyLibPrintLine_D("Popping %llu bytes because it's been %llums since we last got any data from this source", buffer->used - numPoppedBytes, TimeSinceBy(programTime, buffer->lastDataTime));
		numPoppedBytes = buffer->used;
	}
	
	//Pop bytes off the front
	if (numPoppedBytes > 0) { BufferedSocketBufferPop(buffer, numPoppedBytes); }
	
	return result;
}
void PopCmdInBuffer(BufferedSocketBuffer_t* buffer, NetworkCmdHeader_t* header, u8* payloadPntr)
{
	u64 numBytesToPop = sizeof(NetworkCmdHeader_t) + header->length;
	Assert(buffer->used >= numBytesToPop);
	Assert(payloadPntr == &buffer->pntr[sizeof(NetworkCmdHeader_t)]);
	BufferedSocketBufferPop(buffer, numBytesToPop);
}

// +--------------------------------------------------------------+
// |                      Connection Process                      |
// +--------------------------------------------------------------+
void NetworkBusChangeState(NetworkBus_t* bus, NetworkBusState_t newState)
{
	NotNull(bus);
	NotNull(bus->callbacks.GetProgramTime);
	if (bus->state != newState)
	{
		bus->state = newState;
		bus->lastStateChangeTime = bus->callbacks.GetProgramTime(bus);
	}
}
void NetworkBusStartConnection(NetworkBus_t* bus)
{
	NotNull(bus);
	NotNull(bus->socket);
	Assert(IsSocketOpen(bus->socket));
	Assert(bus->state == NetworkBusState_JustOpened || bus->state == NetworkBusState_Disconnected);
	bus->isServerSide = (bus->socket->socket.type == SocketType_MultiDestination);
	
	if (bus->isServerSide)
	{
		NotNull(bus->allocArena);
		CreateVarArray(&bus->clients, bus->allocArena, sizeof(NetworkBusClient_t));
		NetworkBusChangeState(bus, NetworkBusState_Connected);
	}
	else
	{
		NetworkBusSendCmd(bus, nullptr, 0, NetworkBusCmd_NewClient);
		NetworkBusChangeState(bus, NetworkBusState_WaitingForClientId);
	}
}

void NetworkUpdateConnectionProcess(NetworkBus_t* bus, u64 programTime)
{
	if (bus->state == NetworkBusState_WaitingForClientId)
	{
		Assert(!bus->isServerSide);
		if (TimeSinceBy(programTime, bus->lastStateChangeTime) >= NETWORK_BUS_CONNECT_CMD_TIMEOUT)
		{
			GyLibWriteLine_W("Network bus failed to establish a connection!");
			NetworkBusChangeState(bus, NetworkBusState_Disconnected);
		}
	}
}

// +--------------------------------------------------------------+
// |                       Handle Commands                        |
// +--------------------------------------------------------------+
void NetworkBusFinishRetryPacket(NetworkBus_t* bus, RetryPacket_t* retryPacket, u64 retryPacketIndex, bool success, NetworkCmdHeader_t header, u8* payloadPntr)
{
	NotNull2(bus, retryPacket);
	if (bus->callbacks.RetryPacketFinished != nullptr)
	{
		bus->callbacks.RetryPacketFinished(bus, retryPacket, success, header, payloadPntr);
	}
	VarArrayRemove(&bus->retryPackets, retryPacketIndex, RetryPacket_t);
}

void NetworkBusHandleCmd(NetworkBus_t* bus, BufferedSocketBuffer_t* buffer, NetworkCmdHeader_t header, u8* payloadPntr)
{
	NotNull(bus->callbacks.GetProgramTime);
	u64 programTime = bus->callbacks.GetProgramTime(bus);
	MyStr_t payloadStr = NewStr(header.length, (char*)payloadPntr);
	
	NetworkBusClient_t* client = nullptr;
	if (bus->isServerSide && bus->state == NetworkBusState_Connected)
	{
		NotNull(buffer);
		client = FindNetworkBusClientByAddress(bus, buffer->address);
	}
	
	// +==============================+
	// | Match Retry Packet Responses |
	// +==============================+
	u8 packetHandleFlags = NbPacketHandleFlags_None;
	VarArrayLoop(&bus->retryPackets, rIndex)
	{
		VarArrayLoopGet(RetryPacket_t, retryPacket, &bus->retryPackets, rIndex);
		if (header.cmd == retryPacket->expectedRspCmd || header.cmd == retryPacket->failureRspCmd ||
			(bus->isServerSide && (header.cmd == NetworkBusCmd_Success || header.cmd == NetworkBusCmd_Failure)) ||
			(!bus->isServerSide && (header.cmd == NetworkBusRsp_Success || header.cmd == NetworkBusRsp_Failure)))
		{
			bool isResponse = false;
			bool isResponsePositive = false;
			
			NetworkBusStandardPayload_t standardPayload = {};
			if (header.cmd == NetworkBusCmd_Success || header.cmd == NetworkBusCmd_Failure ||
				header.cmd == NetworkBusRsp_Success || header.cmd == NetworkBusRsp_Failure)
			{
				if (Deserialize(NewSerializable_NetworkBusStandardPayload(&standardPayload), payloadStr))
				{
					if (standardPayload.sentPacketId == retryPacket->header.packetId &&
						standardPayload.sentCmd == retryPacket->header.cmd)
					{
						isResponsePositive = (header.cmd == NetworkBusCmd_Success || header.cmd == NetworkBusRsp_Success);
						isResponse = true;
					}
				}
			}
			
			if (bus->callbacks.ResponseCheck != nullptr && bus->callbacks.ResponseCheck(bus, retryPacket, header, payloadPntr))
			{
				isResponse = true;
			}
			
			if (isResponse)
			{
				NetworkBusFinishRetryPacket(bus, retryPacket, rIndex, (header.cmd == retryPacket->expectedRspCmd), header, payloadPntr);
				rIndex--; //Not needed because of break; below, but added just in-case we remove that later
				FlagSet(packetHandleFlags, NbPacketHandleFlags_RetryPacket);
				break;
			}
		}
	}
	
	if (bus->isServerSide)
	{
		switch (header.cmd)
		{
			// +==============================+
			// |   NetworkBusCmd_NewClient    |
			// +==============================+
			case NetworkBusCmd_NewClient:
			{
				if (bus->state == NetworkBusState_Connected)
				{
					NetworkBusClient_t* newClient = VarArrayAdd(&bus->clients, NetworkBusClient_t);
					NotNull(newClient);
					ClearPointer(newClient);
					newClient->id = bus->nextClientId;
					bus->nextClientId++;
					newClient->address = buffer->address;
					newClient->lastCommunicationTime = programTime;
					newClient->lastPingSuccessTime = programTime;
					newClient->lastPingTryTime = programTime;
					
					NetworkBusSendCmd(bus, newClient, 0, NetworkBusRsp_IdAssigned, NewSerializable_U64(&newClient->id));
					FlagSet(packetHandleFlags, NbPacketHandleFlags_NbInternal);
					
					if (bus->callbacks.ClientConnectedOrDisconnected != nullptr) { bus->callbacks.ClientConnectedOrDisconnected(bus, newClient, true); }
				}
			} break;
			
			// +==============================+
			// |      NetworkBusCmd_Ping      |
			// +==============================+
			case NetworkBusCmd_Ping:
			{
				if (client != nullptr)
				{
					NetworkBusSendCmd(bus, client, 0, NetworkBusRsp_Ack);
					FlagSet(packetHandleFlags, NbPacketHandleFlags_NbInternal);
				}
			} break;
			// +==============================+
			// |      NetworkBusCmd_Ack       |
			// +==============================+
			case NetworkBusCmd_Ack:
			{
				if (client != nullptr)
				{
					client->lastPingSuccessTime = bus->callbacks.GetProgramTime(bus);
					FlagSet(packetHandleFlags, NbPacketHandleFlags_NbInternal);
				}
			} break;
		}
	}
	else
	{
		switch (header.cmd)
		{
			// +==============================+
			// |   NetworkBusRsp_IdAssigned   |
			// +==============================+
			case NetworkBusRsp_IdAssigned:
			{
				u64 newClientId = 0;
				if (Deserialize(NewSerializable_U64(&newClientId), payloadStr))
				{
					if (bus->state == NetworkBusState_WaitingForClientId)
					{
						bus->clientId = newClientId;
						NetworkBusChangeState(bus, NetworkBusState_Connected);
					}
					else
					{
						//TODO: Print out an error? Store an error or warning for calling code to see?
					}
				}
				else
				{
					//TODO: Print out an error? Store an error or warning for calling code to see?
				}
				
				FlagSet(packetHandleFlags, NbPacketHandleFlags_NbInternal);
			} break;
			
			// +==============================+
			// |      NetworkBusRsp_Ping      |
			// +==============================+
			case NetworkBusRsp_Ping:
			{
				NetworkBusSendCmd(bus, nullptr, 0, NetworkBusCmd_Ack);
				FlagSet(packetHandleFlags, NbPacketHandleFlags_NbInternal);
			} break;
			// +==============================+
			// |      NetworkBusRsp_Ack       |
			// +==============================+
			case NetworkBusRsp_Ack:
			{
				bus->lastPingSuccessTime = bus->callbacks.GetProgramTime(bus);
				FlagSet(packetHandleFlags, NbPacketHandleFlags_NbInternal);
			} break;
		}
	}
	
	if (bus->state == NetworkBusState_Connected)
	{
		bool handled = bus->callbacks.HandleCommand(bus, client, (NbPacketHandleFlags_t)packetHandleFlags, header, payloadPntr);
		UNUSED(handled);
		//TODO: Do we do something with handled? Maybe print out a warning if not handled?
	}
}

bool NetworkBusServiceBuffer(NetworkBus_t* bus, u64 programTime, BufferedSocketBuffer_t* buffer)
{
	NotNull2(bus, buffer);
	NetworkCmdHeader_t header = {};
	u8* payloadPntr = nullptr;
	
	bool foundCommand = false;
	bool foundAnyCommands = false;
	do
	{
		foundCommand = TryFindCmdInBuffer(buffer, programTime, &header, &payloadPntr);
		if (foundCommand)
		{
			foundAnyCommands = true;
			NetworkBusHandleCmd(bus, buffer, header, payloadPntr);
			PopCmdInBuffer(buffer, &header, payloadPntr);
		}
	} while(foundCommand);
	
	return foundAnyCommands;
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void UpdateNetworkBus(NetworkBus_t* bus)
{
	NotNull2(bus, bus->socket);
	if (bus->state == NetworkBusState_Disconnected) { return; }
	u64 programTime = bus->callbacks.GetProgramTime(bus);
	
	if (!IsSocketOpen(bus->socket))
	{
		GyLibWriteLine_W("Socket was closed for network bus!");
		NetworkBusChangeState(bus, NetworkBusState_Disconnected);
		return;
	}
	
	// +==============================+
	// |    Service All Rx Buffers    |
	// +==============================+
	for (u64 bufferIndex = 0; bufferIndex < ArrayCount(bus->socket->buffers); bufferIndex++)
	{
		BufferedSocketBuffer_t* buffer = &bus->socket->buffers[bufferIndex];
		if (buffer->type == BufferedSocketBufferType_Rx)
		{
			bool foundAnyCommands = NetworkBusServiceBuffer(bus, programTime, buffer);
			UNUSED(foundAnyCommands); //TODO: Do we want to do something with this?
		}
	}
	
	if (bus->state == NetworkBusState_Connected)
	{
		// +==============================+
		// |    Check on RetryPackets     |
		// +==============================+
		VarArrayLoop(&bus->retryPackets, rIndex)
		{
			VarArrayLoopGet(RetryPacket_t, retryPacket, &bus->retryPackets, rIndex);
			if (TimeSinceBy(programTime, retryPacket->lastTryTime) >= NETWORK_BUS_RETRY_DELAY)
			{
				NetworkBusClient_t* client = (bus->isServerSide) ? FindNetworkBusClientById(bus, retryPacket->header.clientId) : nullptr;
				bool failPacket = false;
				if (bus->isServerSide && client == nullptr) { failPacket = true; }
				if (retryPacket->numTries >= retryPacket->maxNumTries) { failPacket = true; }
				
				if (!failPacket)
				{
					GyLibPrintLine_W("Retrying packet %llu %llu/%llu (cmd %u, payload %llu)",
						retryPacket->numTries+1,
						retryPacket->maxNumTries,
						retryPacket->header.packetId,
						retryPacket->header.cmd,
						retryPacket->header.length
					);
					if (NetworkBusSendCmdWithPayload(bus, client, retryPacket->header.packetId, retryPacket->header.cmd, retryPacket->header.length, retryPacket->payloadPntr))
					{
						retryPacket->numTries++;
						retryPacket->lastTryTime = programTime;
					}
					else { failPacket = true; }
				}
				
				if (failPacket)
				{
					NetworkCmdHeader_t fakeHeader = {};
					NetworkBusFinishRetryPacket(bus, retryPacket, rIndex, false, fakeHeader, nullptr);
					rIndex--;
					continue;
				}
			}
		}
		
		//TODO: Ping clients if they haven't communicated in a while, kick ones that haven't communicated in a long while
	}
	else
	{
		NetworkUpdateConnectionProcess(bus, programTime);
	}
}

#endif //GYLIB_HEADER_ONLY

#endif // SOCKETS_SUPPORTED

#endif //  _GY_NETWORK_BUS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
NETWORK_BUS_RESERVED_BASE_CMD
NETWORK_BUS_CLIENT_TO_SERVER_BASE_CMD
NETWORK_BUS_SERVER_TO_CLIENT_BASE_CMD
NETWORK_BUS_UNRESERVED_BASE_CMD
NETWORK_BUS_ATTN_STR
NETWORK_BUS_ATTN_STR_LENGTH
NETWORK_BUS_INCOMPLETE_CMD_TIMEOUT
NETWORK_BUS_CONNECT_CMD_TIMEOUT
NETWORK_BUS_RETRY_DELAY
NETWORK_BUS_DEFAULT_MAX_NUM_TRIES
NetworkBusCmd_None
NetworkBusRsp_None
NetworkBusCmd_NewClient
NetworkBusCmd_Disconnect
NetworkBusCmd_Success
NetworkBusCmd_Failure
NetworkBusCmd_Ping
NetworkBusCmd_Ack
NetworkBusCmd_Last
NetworkBusRsp_IdAssigned
NetworkBusRsp_Kicked
NetworkBusRsp_Success
NetworkBusRsp_Failure
NetworkBusRsp_Ping
NetworkBusRsp_Ack
NetworkBusRsp_Last
NetworkBusState_None
NetworkBusState_Disconnected
NetworkBusState_JustOpened
NetworkBusState_WaitingForClientId
NetworkBusState_Connected
NetworkBusState_NumStates
ResponseCheckResult_NA
ResponseCheckResult_Failure
ResponseCheckResult_Success
ResponseCheckResult_NumResults
NbPacketHandleFlags_None
NbPacketHandleFlags_NbInternal
NbPacketHandleFlags_RetryPacket
NbPacketHandleFlags_All
@Types
NetworkBusCmd_t
NetworkBusState_t
ResponseCheckResult_t
NbPacketHandleFlags_t
NbGetProgramTime_f
NbHandleCommand_f
NbResponseCheck_f
NbRetryPacketFinished_f
NbClientConnectedOrDisconnected_f
NetworkCmdHeader_t
RetryPacket_t
NetworkBusClient_t
NetworkBusCallbacks_t
NetworkBus_t
NetworkBusStandardPayload_t
@Functions
const char* GetNetworkBusCmdStr(NetworkBusCmd_t cmd)
const char* GetNetworkBusStateStr(NetworkBusState_t enumValue)
const char* GetResponseCheckResultStr(ResponseCheckResult_t enumValue)
const char* GetNbPacketHandleFlagsStr(NbPacketHandleFlags_t enumValue)
u64 NB_GET_PROGRAM_TIME_DEFINITION(struct NetworkBus_t* bus)
bool NB_HANDLE_COMMAND_DEFINITION(struct NetworkBus_t* bus, struct NetworkBusClient_t* sourceClient, NbPacketHandleFlags_t handleFlags, struct NetworkCmdHeader_t header, const u8* payloadPntr)
Tribool_t NB_RESPONSE_CHECK_DEFINITION(struct NetworkBus_t* bus, struct RetryPacket_t* packet, struct NetworkCmdHeader_t header, const u8* payloadPntr)
void NB_RETRY_PACKET_FINISHED_DEFINITION(struct NetworkBus_t* bus, struct RetryPacket_t* packet, bool success, struct NetworkCmdHeader_t rspHeader, const u8* rspPayloadPntr)
void NB_CLIENT_CONNECTED_OR_DISCONNECTED_DEFINITION(struct NetworkBus_t* bus, struct NetworkBusClient_t* client, bool connected)
Serializable_t NewSerializable_NetworkBusStandardPayload(NetworkBusStandardPayload_t* successOrFailurePayload)
void FreeRetryPacket(NetworkBus_t* bus, RetryPacket_t* packet)
void FreeNetworkBus(NetworkBus_t* bus)
void CreateNetworkBus(NetworkBus_t* busOut, BufferedSocket_t* socket, MemArena_t* memArena, MemArena_t* tempArena)
void NetworkBusSetCallbacks(NetworkBus_t* bus, void* contextPntr, NbGetProgramTime_f* getProgramTimeFunc, NbHandleCommand_f* handleCommandFunc, NbResponseCheck_f* responseCheckFunc, NbRetryPacketFinished_f* retryPacketFinishedFunc, NbClientConnectedOrDisconnected_f* clientConnectedOrDisconnectedFunc)
bool NetworkBusSendCmdWithPayload(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u32 cmd, u64 payloadLength, const void* payloadPntr)
bool NetworkBusSendCmdStandardPayload(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u32 cmd, u64 sentPacketId, u32 sentCmd)
bool NetworkBusSendCmd(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u32 cmd, Serializable_t payloadSerializable = Serializable_Empty)
bool NetworkBusSendSuccess(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u64 sentPacketId, u32 sentCmd)
bool NetworkBusSendFailure(NetworkBus_t* bus, NetworkBusClient_t* client, u64 packetId, u64 sentPacketId, u32 sentCmd)
u64 NetworkBusSendRetryPacketWithPayload(NetworkBus_t* bus, NetworkBusClient_t* client, u32 cmd, u32 expectedRspCmd, u32 failureRspCmd, u64 payloadLength, const void* payloadPntr, u64 maxNumTries = NETWORK_BUS_DEFAULT_MAX_NUM_TRIES)
u64 NetworkBusSendRetryPacket(NetworkBus_t* bus, NetworkBusClient_t* client, u32 cmd, u32 expectedRspCmd, u32 failureRspCmd, Serializable_t payloadSerializable = Serializable_Empty, u64 maxNumTries = NETWORK_BUS_DEFAULT_MAX_NUM_TRIES)
NetworkBusClient_t* FindNetworkBusClientById(NetworkBus_t* bus, u64 clientId)
NetworkBusClient_t* FindNetworkBusClientByAddress(NetworkBus_t* bus, IpAddressAndPort_t address)
bool TryFindCmdInBuffer(BufferedSocketBuffer_t* buffer, u64 programTime, NetworkCmdHeader_t* headerOut, u8** payloadPntrOut)
void PopCmdInBuffer(BufferedSocketBuffer_t* buffer, NetworkCmdHeader_t* header, u8* payloadPntr)
void NetworkBusChangeState(NetworkBus_t* bus, NetworkBusState_t newState)
void NetworkBusStartConnection(NetworkBus_t* bus)
void NetworkUpdateConnectionProcess(NetworkBus_t* bus, u64 programTime)
void NetworkBusFinishRetryPacket(NetworkBus_t* bus, RetryPacket_t* retryPacket, u64 retryPacketIndex, bool success, NetworkCmdHeader_t header, u8* payloadPntr)
void NetworkBusHandleCmd(NetworkBus_t* bus, BufferedSocketBuffer_t* buffer, NetworkCmdHeader_t header, u8* payloadPntr)
bool NetworkBusServiceBuffer(NetworkBus_t* bus, u64 programTime, BufferedSocketBuffer_t* buffer)
void UpdateNetworkBus(NetworkBus_t* bus)
*/