package write

import (
	"fmt"
	"net"

	"github.com/cz4013/server/apis/write/messages"
	"github.com/cz4013/server/apis/write/service"
)

// Main entrypoint
func Handler(fileStorePath string, clientAddr *net.UDPAddr, data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.WriteResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	fmt.Printf("Received request from client %s: %+v", clientAddr, req)

	res := service.Write(fileStorePath, req)

	fmt.Printf("Response to send to client %s: %+v", clientAddr, res)

	return messages.MarshalResponse(res)
}
