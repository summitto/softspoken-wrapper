package main

import (
	"fmt"

	ot "github.com/summitto/ot-wrapper/pkg"
)

func testServer() {
	fmt.Println("Starting example test server")
	manager := ot.NewOTManagerGo(true, false)
	defer ot.DeleteOTManagerGo(manager)

	fmt.Printf("is connected: %t\n", manager.IsConnected())
	fmt.Printf("is server: %t\n", manager.IsServer())

	manager.Connect("127.0.0.1:12345")

	fmt.Printf("is connected: %t\n", manager.IsConnected())

	input := ot.NewUInt8Vector()
	defer ot.DeleteUInt8Vector(input)

	buffer := []byte{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
		0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	}

	for _, val := range buffer {
		input.Add(val)
	}

	manager.RespondWithData(input)
	fmt.Println("Example server test done")
}

func testClient() {
	fmt.Println("Starting example test client")
	manager := ot.NewOTManagerGo(false, false)
	defer ot.DeleteOTManagerGo(manager)

	fmt.Printf("is connected: %t\n", manager.IsConnected())
	fmt.Printf("is server: %t\n", manager.IsServer())

	manager.Connect("127.0.0.1:12345")

	fmt.Printf("is connected: %t\n", manager.IsConnected())

	choices := ot.NewUInt8Vector() // Set doesn't seem to work
	defer ot.DeleteUInt8Vector(choices)
	choices.Add(2) // 2 is 01 in little endian

	result := manager.RequestData(choices, 2)
	resultBuf := make([]byte, result.Size())

	fmt.Println("Result length:", result.Size())

	for i := 0; i < int(result.Size()); i++ {
		resultBuf[i] = result.Get(i)
	}

	fmt.Println("Received bytes:", resultBuf)
	fmt.Println("Example client test done")
}

func main() {
	testServer()
	testClient()
}
