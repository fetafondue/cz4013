package common

// type lengths
const Uint64ByteLength = 8
const Uint32ByteLength = 4
const BoolByteLength = 1
const MessageTypeLength = 1

// file path
const FilesDir = "/disk"

// invocation semantics
type InvocationSemantic int

const (
	AT_LEAST_ONCE InvocationSemantic = iota
	AT_MOST_ONCE
)
