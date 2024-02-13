# Overview

- Review layers
- TCP vs. UDP
- Writing UDP client code
- Writing UDP server code
- UDP Dangers / Shortcomings

## TCP vs. UDP
- IP (network layer) does nothing to guarantee arrival
- TCP:
  - Connection-based
  - Guarantees packet arrival in order
- UDP:
  - Send-and-pray
- UDP is fast, and TCP's guarantees have some drawbacks (esp. for
  games)
- Also, sometimes you don't care if the data actually gets there bc
  it's immediately stale just after it arrives
  - Streaming
  - Short-and-sweet protocols (DNS)
  - *Video Games*, i.e., player position

## Code
`sendto`, `recvfrom`

## UDP Dangers / Shortcomings
- When you `recvfrom` the data can come from anywhere 🙁
  - Share some information with your request that identifies your
    specific request
  - Check the return address
  - Use `connect()` (doesn't do much but automatically check return
    address for you)
	- Also lets you use `send()`/`recv()` if you like
