package messages

// WriteResponse represents a server response message for the Write method
type WriteResponse struct {
	Success bool
}

// MarshalResponse marshals a WriteResponse struct into a byte slice
func MarshalResponse(res WriteResponse) []byte {
	var successByte byte
	if res.Success {
		successByte = 1
	}

	return []byte{successByte}
}
