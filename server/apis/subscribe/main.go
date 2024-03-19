package subscribe

import (
	"log"
	"net"

	"github.com/cz4013/server/apis/subscribe/messages"
	"github.com/cz4013/server/apis/subscribe/service"
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

	log.Printf("Received request from client %s: %+v\n", clientAddr, req)

	res := service.Subscribe(clientAddr, fileStorePath, req)

	log.Printf("Response to send to client %s: %+v\n", clientAddr, res)

	return messages.MarshalResponse(res)
}


// callback whenever the file is updated 
func HandleNotification(pathname string, updatedContent []byte) {
	
	service.NotifySubscribers(pathname, updatedContent)

}