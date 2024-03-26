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
	messageType := getMessageType(udp_request)
	messageID := getMessageId(udp_request) //todo
	data := udp_request[common.MessageTypeLength+common.Uint32ByteLength:]

	// route request to the corresponding server method based on message type
	switch messageType {
	case READ:
		return read.Handler(fileStorePath, clientAddr, data)
	case WRITE:
		return write.Handler(fileStorePath, clientAddr, data)
	case SUBSCRIBE:
		return subscribe.Handler(fileStorePath, clientAddr, data)
	case REPLACE:
		return replace.Handler(fileStorePath, clientAddr, data)
	case DELETE:
		return delete.Handler(fileStorePath, clientAddr, data)
	case GET_LAST_MODIFIED_TIME:
		return getlastmodifiedtime.Handler(fileStorePath, clientAddr, data)
	default:
		return []byte("invalid request: not a supported message type")
	}
}
