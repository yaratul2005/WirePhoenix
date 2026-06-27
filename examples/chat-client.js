/**
 * PhoenixWire Chat Client - Frontend JavaScript
 */

class ChatClient {
    constructor() {
        this.socket = null;
        this.isConnected = false;
        this.currentUser = null;
        this.users = new Map();
        this.messages = [];
        this.stats = {
            sent: 0,
            received: 0,
            connectedAt: null
        };

        this.initializeUI();
        this.bindEvents();
    }

    initializeUI() {
        // Generate random user on load
        this.generateRandomUser();
        this.updateUserDisplay();
    }

    bindEvents() {
        // Connection buttons
        document.getElementById('connectBtn').addEventListener('click', () => this.showConnectionModal());
        document.getElementById('disconnectBtn').addEventListener('click', () => this.disconnect());
        document.getElementById('modalConnectBtn').addEventListener('click', () => this.handleConnect());
        document.getElementById('modalCancelBtn').addEventListener('click', () => this.hideConnectionModal());

        // Message sending
        document.getElementById('sendBtn').addEventListener('click', () => this.sendMessage());
        document.getElementById('messageInput').addEventListener('keypress', (e) => {
            if (e.key === 'Enter' && !e.shiftKey) {
                e.preventDefault();
                this.sendMessage();
            }
        });

        // Enter key on host/port inputs
        document.getElementById('hostInput').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.handleConnect();
        });
        document.getElementById('portInput').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.handleConnect();
        });
    }

    generateRandomUser() {
        const firstNames = ['Alice', 'Bob', 'Charlie', 'Diana', 'Eve', 'Frank', 'Grace', 'Henry', 'Ivy', 'Jack'];
        const lastNames = ['Smith', 'Johnson', 'Williams', 'Brown', 'Jones', 'Garcia', 'Miller', 'Davis', 'Rodriguez', 'Martinez'];
        const colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#FFA07A', '#98D8C8', '#F7DC6F', '#BB8FCE', '#85C1E2', '#F8B88B', '#52C79A'];
        const statuses = ['Just joined!', 'Ready to chat', 'Saying hello', 'Online now', 'Available'];

        const firstName = firstNames[Math.floor(Math.random() * firstNames.length)];
        const lastName = lastNames[Math.floor(Math.random() * lastNames.length)];

        this.currentUser = {
            id: 'user_' + Math.random().toString(36).substr(2, 9),
            firstName,
            lastName,
            fullName: `${firstName} ${lastName}`,
            username: firstName.toLowerCase() + '_' + lastName.toLowerCase().substr(0, 3) + '_' + Math.floor(Math.random() * 900 + 100),
            avatarColor: colors[Math.floor(Math.random() * colors.length)],
            status: statuses[Math.floor(Math.random() * statuses.length)],
            avatarChar: firstName[0] + lastName[0],
            joinedAt: Date.now()
        };
    }

    updateUserDisplay() {
        if (!this.currentUser) return;

        const avatar = document.getElementById('userAvatar');
        avatar.textContent = this.currentUser.avatarChar;
        avatar.style.backgroundColor = this.currentUser.avatarColor;

        document.getElementById('userName').textContent = this.currentUser.fullName;
        document.getElementById('userStatus').textContent = this.isConnected ? 'Online' : 'Offline';
        document.getElementById('userStatus').classList.toggle('online', this.isConnected);
    }

    showConnectionModal() {
        document.getElementById('connectionModal').classList.add('active');
        document.getElementById('hostInput').focus();
    }

    hideConnectionModal() {
        document.getElementById('connectionModal').classList.remove('active');
    }

    handleConnect() {
        const host = document.getElementById('hostInput').value || '127.0.0.1';
        const port = parseInt(document.getElementById('portInput').value) || 8080;
        const customUsername = document.getElementById('usernameInput').value;

        if (customUsername) {
            this.currentUser.username = customUsername;
        }

        this.hideConnectionModal();
        this.connect(host, port);
    }

    connect(host, port) {
        this.showToast('Connecting...', 'info');

        // Simulate connection (in real implementation, would use WebSocket)
        setTimeout(() => {
            this.isConnected = true;
            this.stats.connectedAt = Date.now();
            this.stats.sent = 0;
            this.stats.received = 0;

            // Update UI
            this.updateConnectionStatus(true);
            this.clearWelcomeMessage();
            this.updateUserDisplay();

            document.getElementById('connectBtn').disabled = true;
            document.getElementById('disconnectBtn').disabled = false;
            document.getElementById('messageInput').disabled = false;
            document.getElementById('sendBtn').disabled = false;

            this.showToast(`Connected as ${this.currentUser.username}`, 'success');

            // Add system message
            this.addSystemMessage(`${this.currentUser.fullName} joined the chat`, 'join');

            // Simulate receiving other users
            this.simulateOtherUsers();

            // Start simulation
            this.startMessageSimulation();
            this.startStatsUpdate();

        }, 1000);
    }

    disconnect() {
        this.isConnected = false;
        this.socket = null;

        document.getElementById('connectBtn').disabled = false;
        document.getElementById('disconnectBtn').disabled = true;
        document.getElementById('messageInput').disabled = true;
        document.getElementById('sendBtn').disabled = true;

        this.updateConnectionStatus(false);
        this.updateUserDisplay();
        this.showToast('Disconnected', 'info');

        // Add system message
        this.addSystemMessage(`${this.currentUser.fullName} left the chat`, 'leave');

        // Clear users
        this.users.clear();
        this.refreshUsersList();
    }

    sendMessage() {
        const input = document.getElementById('messageInput');
        const text = input.value.trim();

        if (!text) return;

        this.addMessage({
            id: Math.random().toString(36),
            user: this.currentUser,
            text: text,
            timestamp: new Date(),
            isUser: true
        });

        this.stats.sent++;
        input.value = '';
        input.focus();

        // Simulate receiving responses
        this.simulateResponses();
    }

    simulateResponses() {
        // Random delay, then add responses from other users
        const delay = Math.random() * 2000 + 500;
        setTimeout(() => {
            if (!this.isConnected) return;

            const users = Array.from(this.users.values());
            if (users.length > 0) {
                const randomUser = users[Math.floor(Math.random() * users.length)];
                const messages = [
                    'That\'s interesting!',
                    'I agree!',
                    'Great point!',
                    'Thanks for sharing!',
                    'Amazing!',
                    'Totally agree with that',
                    'Good idea!',
                    'Let me think about that...',
                    'Awesome!',
                    'Very cool!'
                ];

                this.addMessage({
                    id: Math.random().toString(36),
                    user: randomUser,
                    text: messages[Math.floor(Math.random() * messages.length)],
                    timestamp: new Date(),
                    isUser: false
                });

                this.stats.received++;
            }
        }, delay);
    }

    addMessage(message) {
        this.messages.push(message);
        const container = document.getElementById('messagesContainer');

        const msgEl = document.createElement('div');
        msgEl.className = `message ${message.isUser ? 'user' : 'other'}`;

        const avatarEl = document.createElement('div');
        avatarEl.className = 'message-avatar';
        avatarEl.textContent = message.user.avatarChar;
        avatarEl.style.backgroundColor = message.user.avatarColor;

        const contentEl = document.createElement('div');
        contentEl.className = 'message-content';

        const headerEl = document.createElement('div');
        headerEl.className = 'message-header';
        headerEl.textContent = message.user.fullName;

        const bubbleEl = document.createElement('div');
        bubbleEl.className = 'message-bubble';
        bubbleEl.textContent = message.text;

        const timeEl = document.createElement('div');
        timeEl.className = 'message-time';
        timeEl.textContent = this.formatTime(message.timestamp);

        contentEl.appendChild(headerEl);
        contentEl.appendChild(bubbleEl);
        contentEl.appendChild(timeEl);

        msgEl.appendChild(avatarEl);
        msgEl.appendChild(contentEl);

        container.appendChild(msgEl);
        container.scrollTop = container.scrollHeight;

        this.updateStats();
    }

    addSystemMessage(text, type = 'info') {
        const container = document.getElementById('messagesContainer');

        const msgEl = document.createElement('div');
        msgEl.className = `system-message ${type}`;
        msgEl.textContent = '✦ ' + text + ' ✦';

        container.appendChild(msgEl);
        container.scrollTop = container.scrollHeight;
    }

    simulateOtherUsers() {
        const firstNames = ['Alice', 'Bob', 'Charlie', 'Diana', 'Eve', 'Frank', 'Grace', 'Henry'];
        const lastNames = ['Smith', 'Johnson', 'Williams', 'Brown', 'Jones', 'Garcia', 'Miller', 'Davis'];
        const colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#FFA07A', '#98D8C8', '#F7DC6F', '#BB8FCE', '#85C1E2'];

        const numUsers = Math.floor(Math.random() * 3) + 2;

        for (let i = 0; i < numUsers; i++) {
            const firstName = firstNames[Math.floor(Math.random() * firstNames.length)];
            const lastName = lastNames[Math.floor(Math.random() * lastNames.length)];

            const user = {
                id: 'user_' + Math.random().toString(36).substr(2, 9),
                firstName,
                lastName,
                fullName: `${firstName} ${lastName}`,
                username: firstName.toLowerCase() + '_' + lastName.toLowerCase().substr(0, 3),
                avatarColor: colors[Math.floor(Math.random() * colors.length)],
                avatarChar: firstName[0] + lastName[0]
            };

            this.users.set(user.id, user);
            this.addSystemMessage(`${user.fullName} joined the chat`, 'join');
        }

        this.refreshUsersList();
    }

    refreshUsersList() {
        const usersList = document.getElementById('usersList');
        usersList.innerHTML = '';

        if (this.users.size === 0) {
            usersList.innerHTML = '<div class="no-users">No other users online</div>';
            return;
        }

        this.users.forEach((user) => {
            const userEl = document.createElement('div');
            userEl.className = 'user-item';

            const avatarEl = document.createElement('div');
            avatarEl.className = 'user-item-avatar';
            avatarEl.textContent = user.avatarChar;
            avatarEl.style.backgroundColor = user.avatarColor;

            const nameEl = document.createElement('div');
            nameEl.className = 'user-item-name';
            nameEl.textContent = user.fullName;

            const statusEl = document.createElement('div');
            statusEl.className = 'user-item-status';

            userEl.appendChild(avatarEl);
            userEl.appendChild(nameEl);
            userEl.appendChild(statusEl);

            usersList.appendChild(userEl);
        });

        this.updateStats();
    }

    startMessageSimulation() {
        setInterval(() => {
            if (!this.isConnected) return;

            if (Math.random() > 0.7) {
                const users = Array.from(this.users.values());
                if (users.length > 0) {
                    const randomUser = users[Math.floor(Math.random() * users.length)];
                    const messages = [
                        'Hey everyone!',
                        'How is everyone doing?',
                        'That\'s interesting!',
                        'I agree with that',
                        'Great point!',
                        'Let me think about that...',
                        'Thanks for sharing!',
                        'Amazing work!',
                        'I\'m learning so much!',
                        'This is awesome!'
                    ];

                    this.addMessage({
                        id: Math.random().toString(36),
                        user: randomUser,
                        text: messages[Math.floor(Math.random() * messages.length)],
                        timestamp: new Date(),
                        isUser: false
                    });

                    this.stats.received++;
                }
            }
        }, 3000 + Math.random() * 5000);
    }

    startStatsUpdate() {
        setInterval(() => {
            if (!this.isConnected) {
                this.updateStats();
            }
        }, 1000);
    }

    updateStats() {
        document.getElementById('statSent').textContent = this.stats.sent;
        document.getElementById('statReceived').textContent = this.stats.received;
        document.getElementById('statUsers').textContent = this.users.size + 1;

        if (this.stats.connectedAt) {
            const elapsed = Math.floor((Date.now() - this.stats.connectedAt) / 1000);
            const minutes = Math.floor(elapsed / 60);
            const seconds = elapsed % 60;
            document.getElementById('statTime').textContent = 
                `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
        }
    }

    updateConnectionStatus(connected) {
        const statusDot = document.getElementById('statusDot');
        const connectionText = document.getElementById('connectionText');

        if (connected) {
            statusDot.classList.add('connected');
            connectionText.textContent = 'Connected';
            connectionText.style.color = 'var(--success-color)';
        } else {
            statusDot.classList.remove('connected');
            connectionText.textContent = 'Disconnected';
            connectionText.style.color = 'var(--muted-text)';
        }
    }

    clearWelcomeMessage() {
        const container = document.getElementById('messagesContainer');
        const welcome = container.querySelector('.welcome-message');
        if (welcome) {
            welcome.remove();
        }
    }

    formatTime(date) {
        const now = new Date();
        const hours = String(date.getHours()).padStart(2, '0');
        const minutes = String(date.getMinutes()).padStart(2, '0');
        return `${hours}:${minutes}`;
    }

    showToast(message, type = 'info') {
        let toastId = type === 'error' ? 'errorToast' : 'successToast';
        const toast = document.getElementById(toastId);

        toast.textContent = message;
        toast.classList.add('show');

        setTimeout(() => {
            toast.classList.remove('show');
        }, 3000);
    }
}

// Initialize on page load
document.addEventListener('DOMContentLoaded', () => {
    window.chatClient = new ChatClient();
});
