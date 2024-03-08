package subscribe

import (
	"fmt"
	"net"

	"github.com/cz4013/server/apis/subscribe/messages"
)

// Main entrypoint
func Handler(fileStorePath string, clientAddr *net.UDPAddr, data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.SubscribeResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	fmt.Printf("Received request from client %s: %+v", clientAddr, req)

	// TODO
	_ = req
	return []byte("")
}
