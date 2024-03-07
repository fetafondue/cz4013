package server

import (
	"fmt"

	"github.com/cz4013/server/apis"
	"github.com/cz4013/server/apis/delete"
	"github.com/cz4013/server/apis/read"
	"github.com/cz4013/server/apis/replace"
	"github.com/cz4013/server/apis/subscribe"
	"github.com/cz4013/server/apis/write"
	"github.com/cz4013/server/common"
)

func validateRequest(req []byte) error {
	// request should at least contain MessageType
	if len(req) < common.MessageTypeLength {
		return fmt.Errorf("invalid request: byte array length is too short")
	}
	return nil
}

func getMessageType(req []byte) apis.MessageType {
	// extract MessageType
	return apis.MessageType(req[0])
}

func RouteRequest(udp_request []byte) []byte {
	err := validateRequest(udp_request)
	if err != nil {
		return []byte(err.Error())
	}

	messageType := getMessageType(udp_request)
	req_data := udp_request[1:]

	switch messageType {
	case apis.READ:
		return read.Handler(req_data)
	case apis.WRITE:
		return write.Handler(req_data)
	case apis.SUBSCRIBE:
		return subscribe.Handler(req_data)
	case apis.REPLACE:
		return replace.Handler(req_data)
	case apis.DELETE:
		return delete.Handler(req_data)
	default:
		return []byte("invalid request: not a supported message type")
	}
}
