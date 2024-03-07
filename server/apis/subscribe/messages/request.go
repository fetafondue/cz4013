package messages

import (
	"encoding/binary"
	"fmt"

	"github.com/cz4013/server/common"
)

// SubscribeRequest represents a client request message for the Subscribe method
type SubscribeRequest struct {
	Pathname               string
	MonitorIntervalSeconds uint32
}

func validateMarshalledRequest(req []byte) (pathnameLen int, err error) {
	// ensure that the byte slice is at least its minimum length (2 uint32s)
	if len(req) < 2*common.Uint32ByteLength {
		return 0, fmt.Errorf("invalid request: byte array length is too short")
	}

	pathnameLen = int(binary.BigEndian.Uint32(req[0:common.Uint32ByteLength]))

	// ensure that the byte slice has the correct length
	if len(req) != 2*common.Uint32ByteLength+pathnameLen {
		return 0, fmt.Errorf("invalid request: incorrect byte array length")
	}

	return pathnameLen, nil
}

// UnmarshalRequest unmarshals a byte slice into a SubscribeRequest struct
func UnmarshalRequest(req []byte) (*SubscribeRequest, error) {
	pathnameLen, err := validateMarshalledRequest(req)
	if err != nil {
		return nil, err
	}

	// extract fields
	pathname := string(req[common.Uint32ByteLength : common.Uint32ByteLength+pathnameLen])
	monitorIntervalSeconds := binary.BigEndian.Uint32(req[common.Uint32ByteLength+pathnameLen:])

	return &SubscribeRequest{
		Pathname:               pathname,
		MonitorIntervalSeconds: monitorIntervalSeconds,
	}, nil
}
