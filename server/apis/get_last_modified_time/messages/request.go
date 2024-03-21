package messages

import (
	"encoding/binary"
	"fmt"

	"github.com/cz4013/server/common"
)

// GetLastModifiedTimeRequest represents a client request message for the GetLastModifiedTime method
type GetLastModifiedTimeRequest struct {
	Pathname string
}

func validateMarshalledRequest(req []byte) (pathnameLen int, err error) {
	// ensure that the byte slice is at least its minimum length (1 uint32)
	if len(req) < common.Uint32ByteLength {
		return 0, fmt.Errorf("invalid request: byte array length is too short")
	}

	pathnameLen = int(binary.BigEndian.Uint32(req[0:common.Uint32ByteLength]))

	// ensure that the byte slice has the correct length
	if len(req) != common.Uint32ByteLength+pathnameLen {
		return 0, fmt.Errorf("invalid request: incorrect byte array length")
	}

	return pathnameLen, nil
}

// UnmarshalRequest unmarshals a byte slice into a GetLastModifiedTimeRequest struct
func UnmarshalRequest(req []byte) (*GetLastModifiedTimeRequest, error) {
	pathnameLen, err := validateMarshalledRequest(req)
	if err != nil {
		return nil, err
	}

	// extract field
	pathname := string(req[common.Uint32ByteLength : common.Uint32ByteLength+pathnameLen])

	return &GetLastModifiedTimeRequest{
		Pathname: pathname,
	}, nil
}
