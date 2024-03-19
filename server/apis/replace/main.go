package replace

import (
	"log"
	"net"

	"github.com/cz4013/server/apis/replace/messages"
	"github.com/cz4013/server/apis/replace/service"
)

// Main entrypoint
func Handler(fileStorePath string, clientAddr *net.UDPAddr, data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.ReplaceResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	log.Printf("Received request from client %s: %+v\n", clientAddr, req)

	res := service.Replace(fileStorePath, req)

	log.Printf("Response to send to client %s: %+v\n", clientAddr, res)

	return messages.MarshalResponse(res)
}
