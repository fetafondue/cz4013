package main

import (
	"fmt"
	"os"

	"github.com/cz4013/server/apis"
	"github.com/cz4013/server/apis/delete"
	"github.com/cz4013/server/apis/read"
	"github.com/cz4013/server/apis/replace"
	"github.com/cz4013/server/apis/subscribe"
	"github.com/cz4013/server/apis/write"
	"github.com/cz4013/server/common"
)

func getFullFileStorePath(currentDir string) string {
	return currentDir + common.FilesDir
}

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

func routeRequest(fileStorePath string, udp_request []byte) []byte {
	err := validateRequest(udp_request)
	if err != nil {
		return []byte(err.Error())
	}

	// get request message type
	messageType := getMessageType(udp_request)
	req_data := udp_request[1:]

	// route request to the corresponding server method based on message type
	switch messageType {
	case apis.READ:
		return read.Handler(fileStorePath, req_data)
	case apis.WRITE:
		return write.Handler(fileStorePath, req_data)
	case apis.SUBSCRIBE:
		return subscribe.Handler(fileStorePath, req_data)
	case apis.REPLACE:
		return replace.Handler(fileStorePath, req_data)
	case apis.DELETE:
		return delete.Handler(fileStorePath, req_data)
	default:
		return []byte("invalid request: not a supported message type")
	}
}

func main() {
	// get file store path of server
	currentDir, err := os.Getwd()
	if err != nil {
		fmt.Println("Error: Unable to get working directory, quitting.")
		return
	}
	// the absolute path to the server's file store directory
	fileStorePath := getFullFileStorePath(currentDir)
	fmt.Println("Started server with file directory: fileStorePath, ")

	// TODO: delete this for a udp listener that calls routeRequest
	data := []byte("\x02\x00\x00\x00\x08\x66\x69\x6c\x65\x2e\x74\x78\x74\x00\x00\x00\x01\x00\x00\x00\x03\x65\x65\x65")
	res := routeRequest(fileStorePath, data)
	fmt.Println(res)
}
