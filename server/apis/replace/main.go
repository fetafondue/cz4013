package replace

import "github.com/cz4013/server/apis/replace/messages"

// Main entrypoint
func Handler(fileStorePath string, data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.ReplaceResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	// TODO
	_ = req
	return []byte("")
}
