package read

import (
	"github.com/cz4013/server/apis/read/messages"
	"github.com/cz4013/server/apis/read/service"
)

// Main entrypoint
func Handler(fileStorePath string, data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.ReadResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	res := service.Read(fileStorePath, req)

	return messages.MarshalResponse(res)
}
