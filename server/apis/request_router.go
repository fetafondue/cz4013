package apis

import (
	"encoding/binary"
	"fmt"
	"net"

	"github.com/cz4013/server/apis/delete"
	getlastmodifiedtime "github.com/cz4013/server/apis/get_last_modified_time"
	"github.com/cz4013/server/apis/read"
	"github.com/cz4013/server/apis/replace"
	"github.com/cz4013/server/apis/subscribe"
	"github.com/cz4013/server/apis/write"
	"github.com/cz4013/server/common"
)

var messageResponses = make(map[uint32][]byte) // global map to store message IDs and their responses
var atLeastOnce = true

func validateRequest(req []byte) error {
	// request should at least contain MessageType & message ID (uint32)
	if len(req) < common.MessageTypeLength + common.Uint32ByteLength {
		return fmt.Errorf("invalid request: byte array length is too short")
	}
	return nil
}

func getMessageType(req []byte) MessageType {
	// extract MessageType
	return MessageType(req[0])
}

func getMessageId(req []byte) uint32 {
	return binary.BigEndian.Uint32(req[common.MessageTypeLength:common.MessageTypeLength+common.Uint32ByteLength])
}

func RouteRequest(fileStorePath string, clientAddr *net.UDPAddr, udp_request []byte) []byte {
	err := validateRequest(udp_request)
	if err != nil {
		return []byte(err.Error())
	}

	// get request message type, message ID & data
	messageID := getMessageId(udp_request) 
	if atLeastOnce {
		// Check if the message has already been processed
		if response, ok := messageResponses[messageID]; ok {
			// Duplicate message received, Resend the stored response
			return response
		}
	}
	messageType := getMessageType(udp_request)
	data := udp_request[common.MessageTypeLength+common.Uint32ByteLength:]

	// route request to the corresponding server method based on message type
	switch messageType {
	case READ:
		messageResponses[messageID] = read.Handler(fileStorePath, clientAddr, data)
		return messageResponses[messageID]
	case WRITE:
		messageResponses[messageID] = write.Handler(fileStorePath, clientAddr, data)
		return messageResponses[messageID]
	case SUBSCRIBE:
		messageResponses[messageID] = subscribe.Handler(fileStorePath, clientAddr, data)
		return messageResponses[messageID]
	case REPLACE:
		messageResponses[messageID] = replace.Handler(fileStorePath, clientAddr, data)
		return messageResponses[messageID]
	case DELETE:
		messageResponses[messageID] = delete.Handler(fileStorePath, clientAddr, data)
		return messageResponses[messageID]
	case GET_LAST_MODIFIED_TIME:
		messageResponses[messageID] = getlastmodifiedtime.Handler(fileStorePath, clientAddr, data)
		return messageResponses[messageID]
	default:
		return []byte("invalid request: not a supported message type")
	}
}
