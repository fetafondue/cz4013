package messages

// ReplaceResponse represents a server response message for the Replace method
type ReplaceResponse struct {
	Success bool
}

// MarshalResponse marshals a ReplaceResponse struct into a byte slice
func MarshalResponse(res ReplaceResponse) []byte {
	var successByte byte
	if res.Success {
		successByte = 1
	}

	return []byte{successByte}
}
