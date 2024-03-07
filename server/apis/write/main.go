package write

import (
	"github.com/cz4013/server/apis/write/messages"
	"github.com/cz4013/server/apis/write/service"
)

// Main entrypoint
func Handler(fileStorePath string, data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.WriteResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	res := service.Write(fileStorePath, req)

	return messages.MarshalResponse(res)
}
