# PhoenixWire Chat Demo & Load Testing Suite

This directory contains demonstration and testing scripts for the **WirePhoenix** high-performance real-time transport protocol.

## 📁 Files Overview

### Core Components

1. **RandomUserGenerator.php**
   - Utility class for generating realistic random user data
   - Creates random usernames, names, avatar colors, status messages
   - Generates random chat messages for testing
   - Provides configurable message frequency

2. **chat_server.php**
   - Full-featured WirePhoenix server implementation
   - Accepts multiple concurrent client connections
   - Automatically assigns random user data to each connection
   - Broadcasts join/leave/message events to all connected clients
   - Tracks session statistics and user metrics

3. **chat_client.php**
   - Single client instance for manual testing
   - Auto-connects to server with random user profile
   - Automatically sends messages at random intervals
   - Listens for and displays incoming messages and events
   - Handles reconnection and error scenarios

4. **multi_client_simulator.php**
   - Advanced load testing tool
   - Spawns multiple concurrent client instances
   - Tests server scalability and performance
   - Generates detailed performance metrics
   - Real-time statistics during test execution

---

## 🚀 Quick Start

### Prerequisites

- PHP 8.0+
- WirePhoenix library installed (via Composer)
- Linux/macOS or Windows with PHP CLI

### Installation

```bash
cd /path/to/WirePhoenix
composer install
```

---

## 💻 Usage Examples

### 1. Start the Server

Run the chat server on localhost:8080:

```bash
php examples/chat_server.php 127.0.0.1 8080
```

**Output:**
```
╔════════════════════════════════════════╗
║     PhoenixWire Chat Server Demo       ║
║     Version 1.0                        ║
╚════════════════════════════════════════╝

🚀 Starting server on 127.0.0.1:8080...

✨ Server ready. Waiting for connections...
═══════════════════════════════════════════
```

---

### 2. Connect a Single Client

In another terminal, connect a client:

```bash
php examples/chat_client.php 127.0.0.1 8080
```

**Output:**
```
╔════════════════════════════════════════╗
║     PhoenixWire Chat Client Demo       ║
║     Version 1.0                        ║
╚════════════════════════════════════════╝

👤 Your Profile:
   Username: alice_smi_456
   Full Name: Alice Smith
   User ID: user_a1b2c3d4
   Avatar: #FF6B6B
   Status: Just joined!

🔗 Connecting to 127.0.0.1:8080...

✅ Connected to server!
   Ready to send/receive messages.

[✓] bob_joh_789 joined the chat!
    Active users: 2

💬 [bob_joh_789]: That's interesting!

[You]: Great point!

💬 > 
```

---

### 3. Multiple Clients (Manual)

Open multiple terminals and run the client in each:

```bash
# Terminal 1
php examples/chat_client.php 127.0.0.1 8080

# Terminal 2
php examples/chat_client.php 127.0.0.1 8080

# Terminal 3
php examples/chat_client.php 127.0.0.1 8080
```

Each client will automatically send/receive messages.

---

### 4. Load Testing with Multi-Client Simulator

Run load test with 5 clients for 60 seconds:

```bash
php examples/multi_client_simulator.php 127.0.0.1 8080 5 60
```

**Output:**
```
╔════════════════════════════════════════════════╗
║   PhoenixWire Multi-Client Load Test Simulator ║
║   Version 1.0                                  ║
╚════════════════════════════════════════════════╝

📊 Test Configuration:
   Server: 127.0.0.1:8080
   Clients: 5
   Duration: 60s
   Start time: 2026-06-27 14:30:45

🚀 Spawning 5 clients...
   [1] alice_smi_456 initialized
   [2] bob_joh_789 initialized
   [3] charlie_wil_234 initialized
   [4] diana_bro_567 initialized
   [5] eve_jon_890 initialized

✨ All clients initialized. Starting simulation...
═══════════════════════════════════════════════════

📊 Status (T+10s, 50s remaining):
   Connected Clients: 5/5
   Total Messages Sent: 24
   Total Messages Received: 96
   Errors: 0
   Avg Messages/Client (Sent/Received): 4.8/19.2

📊 Status (T+20s, 40s remaining):
   Connected Clients: 5/5
   Total Messages Sent: 48
   Total Messages Received: 192
   Errors: 0
   Avg Messages/Client (Sent/Received): 9.6/38.4

[After test completes...]

╔════════════════════════════════════════════════╗
║              FINAL TEST REPORT                 ║
╚════════════════════════════════════════════════╝

📈 Test Results:
   Total Duration: 60.15s
   Clients Initialized: 5
   Peak Connected: 5
   Total Messages Sent: 120
   Total Messages Received: 480
   Total Errors: 0
   Messages/Second: 1.99
```

---

## 🎯 Testing Scenarios

### Scenario 1: Basic Connectivity Test

```bash
# Terminal 1 - Start server
php examples/chat_server.php 127.0.0.1 8080

# Terminal 2 - Connect one client
php examples/chat_client.php 127.0.0.1 8080
```

**What it tests:**
- ✓ Basic TCP connection
- ✓ Handshake protocol
- ✓ User registration
- ✓ Message sending/receiving
- ✓ Graceful disconnect

---

### Scenario 2: Multi-User Chat

```bash
# Terminal 1 - Start server
php examples/chat_server.php 127.0.0.1 8080

# Terminals 2-6 - Connect 5 clients
for i in {1..5}; do php examples/chat_client.php 127.0.0.1 8080 & done
```

**What it tests:**
- ✓ Concurrent connections
- ✓ Broadcast messaging
- ✓ Session management
- ✓ Connection state tracking
- ✓ Multiple message queues

---

### Scenario 3: Load Testing

```bash
# Terminal 1 - Start server
php examples/chat_server.php 127.0.0.1 8080

# Terminal 2 - Run load test (10 clients, 2 minutes)
php examples/multi_client_simulator.php 127.0.0.1 8080 10 120
```

**What it tests:**
- ✓ Server scalability
- ✓ Message throughput (messages/second)
- ✓ Memory usage under load
- ✓ Connection lifecycle at scale
- ✓ Error handling under stress

---

### Scenario 4: Long-Running Stability Test

```bash
# Terminal 1 - Start server
php examples/chat_server.php 127.0.0.1 8080

# Terminal 2 - Extended load test (20 clients, 10 minutes)
php examples/multi_client_simulator.php 127.0.0.1 8080 20 600
```

**What it tests:**
- ✓ Memory leak detection
- ✓ Connection stability over time
- ✓ Server recovery from edge cases
- ✓ Statistics accumulation accuracy

---

## 📊 Key Metrics

The load testing suite tracks:

| Metric | Purpose |
|--------|---------|
| **Connected Clients** | Real-time count of active connections |
| **Messages Sent** | Total outbound messages from all clients |
| **Messages Received** | Total inbound messages to all clients |
| **Errors** | Count of connection/protocol errors |
| **Messages/Second** | Throughput in messages per second |
| **Avg Messages/Client** | Distribution of message load |

---

## 🔧 Advanced Usage

### Custom Server Port

```bash
php examples/chat_server.php 0.0.0.0 9000
```

### Custom Client Count in Load Test

```bash
php examples/multi_client_simulator.php 127.0.0.1 8080 50 120
```
(50 clients, 120 seconds)

### Remote Server

```bash
php examples/chat_client.php 192.168.1.100 8080
```

---

## 🐛 Troubleshooting

### "Connection refused"
- Ensure server is running on the correct host:port
- Check firewall rules
- Verify PHP can create TCP sockets

### "Cannot load PHP extension"
- The extension is optional - StreamClient provides pure-PHP fallback
- Verify Composer autoload is working: `php -r "require 'vendor/autoload.php'; echo 'OK';"`

### Low message throughput in load test
- Check server CPU/memory usage
- Reduce client count and retest
- Verify network bandwidth with `netstat` or similar

### Client connects but doesn't receive messages
- Ensure broadcast logic is implemented on server
- Check message format is valid JSON
- Verify session tracking is working

---

## 📝 Data Generated

### Sample User Data
```json
{
  "id": "user_a1b2c3d4",
  "username": "alice_smi_456",
  "firstName": "Alice",
  "lastName": "Smith",
  "fullName": "Alice Smith",
  "avatarColor": "#FF6B6B",
  "status": "Just joined!",
  "joinedAt": 1719497445.2345,
  "messageCount": 15,
  "sessionId": "sess_0102030405060708"
}
```

### Sample Message
```json
{
  "type": "MESSAGE",
  "user": {
    "id": "user_a1b2c3d4",
    "username": "alice_smi_456",
    "fullName": "Alice Smith",
    "avatarColor": "#FF6B6B"
  },
  "message": "Great point!",
  "timestamp": 1719497450.5678
}
```

---

## 🎓 Learning Resources

These scripts demonstrate:

1. **PhoenixWire Client Connection**
   - How to instantiate `AsyncClient`
   - Connection lifecycle management
   - Event callbacks (onConnect, onMessage, onClose)

2. **PhoenixWire Server Implementation**
   - How to set up a listening server
   - Handling concurrent sessions
   - Broadcasting to multiple clients

3. **Real-time Communication Patterns**
   - User presence tracking
   - Message broadcast algorithms
   - Join/leave notification systems

4. **Load Testing Best Practices**
   - Creating realistic test data
   - Measuring throughput and latency
   - Generating meaningful performance reports

---

## 📄 License

These examples are part of the WirePhoenix project (MIT License).

---

## 🤝 Contributing

To add more test scenarios:

1. Create a new PHP file in this directory
2. Follow the same structure as existing scripts
3. Use `RandomUserGenerator` for consistent test data
4. Update this README with usage instructions

---

**Happy testing! 🚀**
