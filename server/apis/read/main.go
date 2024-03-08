package read

import (
	"fmt"
	"net"

	"github.com/cz4013/server/apis/read/messages"
	"github.com/cz4013/server/apis/read/service"
)

// Main entrypoint
func Handler(fileStorePath string, clientAddr *net.UDPAddr, data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.ReadResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	fmt.Printf("Received request from client %s: %+v", clientAddr, req)

	res := service.Read(fileStorePath, req)

	fmt.Printf("Response to send to client %s: %+v", clientAddr, res)

	return messages.MarshalResponse(res)
}
