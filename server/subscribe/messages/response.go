package messages

// SubscribeResponse represents a server response message for the Subscribe method
type SubscribeResponse struct {
	Success bool
}

// MarshalResponse marshals a SubscribeResponse struct into a byte slice
func MarshalResponse(res SubscribeResponse) []byte {
	var successByte byte
	if res.Success {
		successByte = 1
	}

	return []byte{successByte}
}
