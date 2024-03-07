package apis

type MessageType byte

const (
	READ      MessageType = 1 << iota // 1 << 0 = 00001
	WRITE                             // 1 << 1 = 00010
	SUBSCRIBE                         // 1 << 2 = 00100
	REPLACE                           // 1 << 3 = 01000
	DELETE                            // 1 << 4 = 10000
)
