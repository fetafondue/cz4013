package delete

import (
	"log"
	"net"

	"github.com/cz4013/server/apis/delete/messages"
)

// Main entrypoint
func Handler(fileStorePath string, clientAddr *net.UDPAddr, data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.DeleteResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	log.Printf("Received request from client %s: %+v", clientAddr, req)

	// TODO process request
	_ = req
	res := messages.DeleteResponse{}

	log.Printf("Response to send to client %s: %+v", clientAddr, res)

	return messages.MarshalResponse(res)
}
