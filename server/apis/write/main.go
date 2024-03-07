package write

import "github.com/cz4013/server/apis/write/messages"

// Main entrypoint
func Handler(data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.WriteResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	// TODO
	_ = req
	return []byte("")
}
