package messages

// DeleteResponse represents a server response message for the Delete method
type DeleteResponse struct {
	Success bool
}

// MarshalResponse marshals a DeleteResponse struct into a byte slice
func MarshalResponse(res DeleteResponse) []byte {
	var successByte byte
	if res.Success {
		successByte = 1
	}

	return []byte{successByte}
}
