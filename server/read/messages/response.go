package messages

import (
	"encoding/binary"

	"github.com/cz4013/server/common"
)

// ReadResponse represents a server response message for the Read method
type ReadResponse struct {
	Content string
}

// MarshalResponse marshals a ReadResponse struct into a byte slice
func MarshalResponse(res ReadResponse) []byte {
	contentBytes := []byte(res.Content)
	contentLen := len(contentBytes)

	// buffer consists of: contentLen (uint32), Content (str)
	bufLen := common.Uint32ByteLength + contentLen
	buf := make([]byte, bufLen)

	// insert Content length and itself into buf
	binary.BigEndian.PutUint32(buf[0:common.Uint32ByteLength], uint32(contentLen))
	copy(buf[common.Uint32ByteLength:common.Uint32ByteLength+contentLen], contentBytes)

	return buf
}
