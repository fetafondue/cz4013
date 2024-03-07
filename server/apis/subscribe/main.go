package subscribe

import "github.com/cz4013/server/apis/subscribe/messages"

// Main entrypoint
func Handler(data []byte) []byte {
	req, err := messages.UnmarshalRequest(data)
	if err != nil {
		return messages.MarshalResponse(messages.SubscribeResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		})
	}

	// TODO
	_ = req
	return []byte("")
}
