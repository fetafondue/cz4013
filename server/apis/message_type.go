package apis

type MessageType byte

const (
	READ                   MessageType = 1 << iota // 1 << 0 = 000001
	WRITE                                          // 1 << 1 = 000010
	SUBSCRIBE                                      // 1 << 2 = 000100
	REPLACE                                        // 1 << 3 = 001000
	DELETE                                         // 1 << 4 = 010000
	GET_LAST_MODIFIED_TIME                         // 1 << 5 = 100000
)
