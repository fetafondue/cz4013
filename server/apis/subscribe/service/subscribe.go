package service

import (
	"fmt"
	"net"
	"os"
	"sync"
	"time"

	"github.com/cz4013/server/apis/subscribe/messages"
	"github.com/cz4013/server/common"
	"github.com/cz4013/server/network"
)

type Subscription struct {
	ClientAddr   *net.UDPAddr
	Pathname     string
	ExpireTime   time.Time
}

var (
	subscriptions   map[string][]*Subscription
	subscriptionsMu sync.Mutex
)

func init() {
	subscriptions = make(map[string][]*Subscription) // pathName -> subscrptions
}

func Subscribe(clientAddr *net.UDPAddr, fileStorePath string, req *messages.SubscribeRequest) messages.SubscribeResponse {
	// sanity check: whether path exists
	filePath := common.ConcatenatePaths(fileStorePath, req.Pathname)
	_, err := os.ReadFile(filePath)
	if err != nil {
		return messages.SubscribeResponse{
			Success:      false,
			ErrorMessage: err.Error(),
		}
	}

	expireTime := time.Now().Add(time.Duration(req.MonitorIntervalSeconds) * time.Second)
	subscription := &Subscription{
		ClientAddr: clientAddr,
		Pathname:   req.Pathname,
		ExpireTime: expireTime,
	}

	subscriptionsMu.Lock()
	subscriptions[req.Pathname] = append(subscriptions[req.Pathname], subscription)
	subscriptionsMu.Unlock()

	// Start a goroutine to remove the subscription after the monitor interval expires
	go func() {
		time.Sleep(time.Duration(req.MonitorIntervalSeconds) * time.Second)
		subscriptionsMu.Lock()
		for i, sub := range subscriptions[req.Pathname] {
			if sub == subscription {
				subscriptions[req.Pathname] = append(subscriptions[req.Pathname][:i], subscriptions[req.Pathname][i+1:]...)
				break
			}
		}
		subscriptionsMu.Unlock()
	}()
	
	return messages.SubscribeResponse{Success: true}
}

func NotifySubscribers(pathname string, updatedContent []byte) error {
    subscriptionsMu.Lock()
    defer subscriptionsMu.Unlock()

    // Check if there are any subscribers for the updated file
    if subs, ok := subscriptions[pathname]; ok {
        // Iterate over the list of subscribers
        for i := 0; i < len(subs); {
            sub := subs[i]

            // Check if the subscription is still active
            if time.Now().Before(sub.ExpireTime) {
                // Send the updated content to the subscriber
                err := network.SendToClient(sub.ClientAddr, updatedContent)
				if err != nil {
					return fmt.Errorf("error sending data to client: %w", err)
				}
                // Move to the next subscriber
                i++
            } else {
                // Subscription has expired, remove it from the list
                subs = append(subs[:i], subs[i+1:]...)
            }
        }

        // Update the list of subscribers for the file
        if len(subs) == 0 {
            delete(subscriptions, pathname) // No more subscribers, remove the entry
        } else {
            subscriptions[pathname] = subs // Update the list of subscribers
        }
    }
	return nil
}