# PhoenixWire Chat Frontend

A modern, responsive web-based chat interface for testing the PhoenixWire real-time transport protocol.

## 📁 Files

- **index.html** - Main chat interface
- **styles.css** - Modern dark theme styling
- **chat-client.js** - Frontend logic and simulated connections
- **serve.php** - Simple development server

## 🚀 Quick Start

### Option 1: Using PHP Built-in Server (Recommended)

```bash
cd /path/to/WirePhoenix/examples

# Navigate to the examples directory
cd examples

# Start PHP dev server on port 8000
php -S localhost:8000 serve.php

# Open browser to http://localhost:8000
```

### Option 2: Using XAMPP

If you're using XAMPP (which you have at c:\xampp):

```
1. Copy the examples folder to c:\xampp\htdocs\
   - Or create a symlink: mklink /D c:\xampp\htdocs\phoenix-chat c:\xampp\htdocs\wire\WirePhoenix\examples

2. Access via http://localhost/phoenix-chat
```

### Option 3: Using Python Simple HTTP Server

```bash
cd /path/to/examples
python -m http.server 8000
# or Python 3
python3 -m http.server 8000
```

### Option 4: Using Node.js

```bash
npm install -g http-server
cd /path/to/examples
http-server -p 8000
```

---

## 💻 Features

### User Management
- ✅ Automatic random user generation
- ✅ Custom username support
- ✅ Unique user profiles with:
  - Random first/last names
  - Avatar initials and colors
  - User ID and session tracking
  - Join time tracking

### Connection Management
- ✅ Connect/Disconnect buttons
- ✅ Connection status indicator (live dot animation)
- ✅ Real-time connection status display
- ✅ Host and port configuration
- ✅ Connection error handling

### Chat Features
- ✅ Send/receive messages
- ✅ User presence tracking
- ✅ Join/leave notifications
- ✅ Message timestamps
- ✅ Scrollable message history
- ✅ System messages
- ✅ Real-time user list

### Statistics Dashboard
- ✅ Messages sent counter
- ✅ Messages received counter
- ✅ Online users counter
- ✅ Connection time display (MM:SS format)
- ✅ Real-time stat updates

### UI/UX
- ✅ Modern dark theme
- ✅ Responsive design
- ✅ Smooth animations
- ✅ Toast notifications
- ✅ Avatar colors for visual distinction
- ✅ Professional layout with sidebar

---

## 🎮 Usage Guide

### 1. Start the Application

```bash
php -S localhost:8000 serve.php
```

### 2. Open in Browser

Navigate to: **http://localhost:8000**

### 3. Connect to Server

1. Click the **"Connect"** button
2. Enter connection details:
   - **Host**: Default `127.0.0.1`
   - **Port**: Default `8080`
   - **Username** (optional): Leave blank for random username
3. Click **"Connect"**

### 4. Chat

- Type messages in the input field
- Press `Enter` or click **"Send"** button
- View incoming messages from other users (simulated)
- Check online users in the sidebar
- View connection stats on the right panel

### 5. Disconnect

Click the **"Disconnect"** button to close the connection

---

## 🧪 Testing Scenarios

### Scenario 1: Single User Chat

1. Start the frontend
2. Connect with default settings
3. Send a few messages
4. Observe simulated responses from other users
5. Check statistics

### Scenario 2: Simulated Multi-User

1. Keep the chat open
2. Multiple simulated users appear in the sidebar
3. Random messages arrive from other users
4. Watch the statistics update
5. View join/leave notifications

### Scenario 3: Long Session

1. Connect and keep the chat open
2. Observe connection time incrementing
3. Watch message counts accumulate
4. Monitor real-time user presence changes
5. Test disconnect/reconnect

---

## 🔧 Advanced Configuration

### Change Default Port

Edit `chat-client.js` or use the connection modal:

```javascript
document.getElementById('portInput').value = 9000;
```

### Adjust Message Frequency

In `chat-client.js`, modify the simulation intervals:

```javascript
// Around line 250
}, 3000 + Math.random() * 5000);  // 3-8 seconds between messages
```

### Customize Colors

In `styles.css`, modify the CSS variables:

```css
:root {
    --primary-color: #FF6B35;      /* Main brand color */
    --secondary-color: #004E89;    /* Secondary color */
    --success-color: #27AE60;      /* Online indicator */
    --danger-color: #E74C3C;       /* Offline indicator */
}
```

### Add More User Names

In `chat-client.js`, expand the arrays:

```javascript
const firstNames = ['Alice', 'Bob', ...]; // Add more names
const lastNames = ['Smith', 'Johnson', ...]; // Add more last names
```

---

## 📊 Statistics Explained

| Stat | Meaning |
|------|---------|
| **Messages Sent** | Total messages you've sent in this session |
| **Messages Received** | Total messages received from other users |
| **Online Users** | Total users in chat (including yourself) |
| **Connection Time** | How long you've been connected (MM:SS) |

---

## 🐛 Troubleshooting

### Port Already in Use

```bash
# Change port number
php -S localhost:8001 serve.php
```

### CSS/JS Not Loading

- Ensure `styles.css` and `chat-client.js` are in the same directory as `index.html`
- Clear browser cache (Ctrl+Shift+Delete)
- Check browser console for errors (F12)

### No Simulated Messages Arriving

- This is normal if the connection modal hasn't been used
- Click "Connect" to start the simulation
- Check that messages are being sent successfully

### Styling Issues

- Ensure CSS file is being served (check Network tab in DevTools)
- Try a different browser
- Check for CSS syntax errors in browser console

---

## 🔌 Integrating with Real WirePhoenix Server

To connect to a real WirePhoenix server instead of simulation:

1. Modify `chat-client.js` in the `connect()` method
2. Replace simulation with actual WebSocket or AJAX calls
3. Update message handlers to use server responses
4. Implement proper error handling

Example integration point:

```javascript
connect(host, port) {
    // Replace this simulated connect with real connection
    const url = `ws://${host}:${port}`;
    
    this.socket = new WebSocket(url);
    
    this.socket.onopen = () => {
        this.isConnected = true;
        this.updateConnectionStatus(true);
        // ... rest of connection logic
    };
    
    this.socket.onmessage = (event) => {
        const message = JSON.parse(event.data);
        this.handleServerMessage(message);
    };
    
    // ... etc
}
```

---

## 📚 Project Structure

```
examples/
├── index.html           # Main chat UI
├── styles.css          # Styling
├── chat-client.js      # Frontend logic
├── serve.php           # Dev server
├── README.md           # This file
├── chat_server.php     # CLI server (PHP)
├── chat_client.php     # CLI client (PHP)
├── quick_test.php      # Installation test
├── multi_client_simulator.php
└── RandomUserGenerator.php
```

---

## 🎨 UI Preview

### Layout
```
┌─────────────────────────────────────────────────────────────┐
│ Sidebar │                Main Chat Area          │ Stats    │
│ ─────── │ ─────────────────────────────────────  │ ─────── │
│ Logo    │ Chat Header                            │ Stats   │
│ Profile │ ─────────────────────────────────────  │         │
│ Online  │                                        │         │
│ Users   │ Message History (scrollable)           │         │
│         │                                        │         │
│ Connect │ ─────────────────────────────────────  │         │
│ Disconnect                                       │         │
│         │ Message Input Area                     │         │
│         │ [Type message...] [Send]              │         │
└─────────────────────────────────────────────────────────────┘
```

---

## 📝 Browser Support

- ✅ Chrome/Chromium 90+
- ✅ Firefox 88+
- ✅ Safari 14+
- ✅ Edge 90+
- ✅ Mobile browsers (responsive)

---

## 🤝 Contributing

To enhance the frontend:

1. Modify `chat-client.js` for logic changes
2. Update `styles.css` for styling changes
3. Edit `index.html` for structure changes
4. Test across browsers

---

## 📄 License

Part of the PhoenixWire project (MIT License)

---

**Happy chatting! 🚀**
