package read

import "github.com/cz4013/server/apis/read/messages"

// Main entrypoint
func Handler(data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.ReadResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	// TODO
	_ = req
	return []byte("")
}
