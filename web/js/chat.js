// Chat UI and message handling
class ChatManager {
    constructor() {
        this.currentContact = null;
        this.messages = new Map(); // contactId -> messages array
        this.contacts = new Map(); // contactId -> contact info
        this.messageId = 1;
        
        // Add broadcast contact by default
        this.contacts.set('broadcast', {
            id: 'broadcast',
            name: 'Broadcast',
            status: 'Everyone',
            avatar: '📢',
            lastSeen: Date.now()
        });
    }

    initialize() {
        this.setupEventListeners();
        this.loadStoredMessages();
        this.selectContact('broadcast'); // Start with broadcast selected
    }

    setupEventListeners() {
        // Message sending
        const sendBtn = document.getElementById('sendBtn');
        const messageText = document.getElementById('messageText');
        
        sendBtn.addEventListener('click', () => this.sendMessage());
        messageText.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                this.sendMessage();
            }
        });

        // Emergency button
        const emergencyBtn = document.getElementById('emergencyBtn');
        emergencyBtn.addEventListener('click', () => this.sendEmergencyMessage());

        // Contact selection
        document.getElementById('contacts').addEventListener('click', (e) => {
            const contact = e.target.closest('.contact');
            if (contact) {
                const contactId = contact.dataset.id;
                this.selectContact(contactId);
            }
        });
    }

    selectContact(contactId) {
        // Update UI
        document.querySelectorAll('.contact').forEach(el => {
            el.classList.remove('active');
        });
        
        const contactElement = document.querySelector(`[data-id="${contactId}"]`);
        if (contactElement) {
            contactElement.classList.add('active');
        }

        this.currentContact = contactId;
        
        // Update chat title
        const contact = this.contacts.get(contactId);
        const chatTitle = document.getElementById('chatTitle');
        chatTitle.textContent = contact ? contact.name : contactId;

        // Show/hide emergency button (only for broadcast)
        const emergencyBtn = document.getElementById('emergencyBtn');
        emergencyBtn.style.display = contactId === 'broadcast' ? 'block' : 'none';

        // Show message input
        document.getElementById('messageInputContainer').style.display = 'block';

        // Load messages for this contact
        this.displayMessages(contactId);
    }

    displayMessages(contactId) {
        const messagesContainer = document.getElementById('messages');
        messagesContainer.innerHTML = '';

        const messages = this.messages.get(contactId) || [];
        
        if (messages.length === 0) {
            if (contactId === 'broadcast') {
                messagesContainer.innerHTML = `
                    <div class="welcome-message">
                        <h3>Broadcast Channel</h3>
                        <p>Messages sent here will reach all devices in the mesh network.</p>
                        <p>Perfect for emergency communications and group announcements.</p>
                    </div>
                `;
            } else {
                messagesContainer.innerHTML = `
                    <div class="welcome-message">
                        <h3>No messages yet</h3>
                        <p>Start a conversation with this device.</p>
                    </div>
                `;
            }
            return;
        }

        messages.forEach(message => {
            this.addMessageElement(message);
        });

        // Scroll to bottom
        messagesContainer.scrollTop = messagesContainer.scrollHeight;
    }

    addMessageElement(message) {
        const messagesContainer = document.getElementById('messages');
        
        const messageElement = document.createElement('div');
        messageElement.className = `message ${message.sent ? 'sent' : 'received'}`;
        messageElement.dataset.messageId = message.id;
        
        if (message.emergency) {
            messageElement.classList.add('emergency');
        }

        const timeStr = new Date(message.timestamp).toLocaleTimeString([], { 
            hour: '2-digit', 
            minute: '2-digit' 
        });

        messageElement.innerHTML = `
            <div class="message-text">${this.escapeHtml(message.text)}</div>
            <div class="message-meta">
                <span class="message-time">${timeStr}</span>
                ${message.sent ? `<span class="message-status ${message.status || 'pending'}">${this.getStatusIcon(message.status)}</span>` : ''}
            </div>
        `;

        messagesContainer.appendChild(messageElement);
        
        // Remove welcome message if it exists
        const welcomeMessage = messagesContainer.querySelector('.welcome-message');
        if (welcomeMessage) {
            welcomeMessage.remove();
        }

        // Scroll to bottom
        messagesContainer.scrollTop = messagesContainer.scrollHeight;
    }

    async sendMessage() {
        const messageText = document.getElementById('messageText');
        const text = messageText.value.trim();
        
        if (!text || !this.currentContact) {
            return;
        }

        if (!window.bluetoothManager || !window.bluetoothManager.isConnected) {
            alert('Not connected to MeshChat device');
            return;
        }

        const message = {
            id: this.messageId++,
            text: text,
            timestamp: Date.now(),
            sent: true,
            status: 'pending',
            emergency: false,
            contactId: this.currentContact
        };

        // Add to local storage
        this.addMessage(this.currentContact, message);
        
        // Add to UI
        this.addMessageElement(message);

        // Clear input
        messageText.value = '';

        try {
            // Send via Bluetooth
            if (this.currentContact === 'broadcast') {
                await window.bluetoothManager.sendBroadcast(text);
            } else {
                await window.bluetoothManager.sendTextMessage(this.currentContact, text);
            }
            
            this.updateMessageStatus(message.id, 'sent');
        } catch (error) {
            console.error('Failed to send message:', error);
            this.updateMessageStatus(message.id, 'failed');
        }
    }

    async sendEmergencyMessage() {
        const text = prompt('Emergency message:');
        if (!text) return;

        if (!window.bluetoothManager || !window.bluetoothManager.isConnected) {
            alert('Not connected to MeshChat device');
            return;
        }

        const message = {
            id: this.messageId++,
            text: `🚨 EMERGENCY: ${text}`,
            timestamp: Date.now(),
            sent: true,
            status: 'pending',
            emergency: true,
            contactId: 'broadcast'
        };

        // Add to local storage
        this.addMessage('broadcast', message);
        
        // Switch to broadcast if not already
        if (this.currentContact !== 'broadcast') {
            this.selectContact('broadcast');
        }
        
        // Add to UI
        this.addMessageElement(message);

        try {
            await window.bluetoothManager.sendBroadcast(text, true);
            this.updateMessageStatus(message.id, 'sent');
        } catch (error) {
            console.error('Failed to send emergency message:', error);
            this.updateMessageStatus(message.id, 'failed');
        }
    }

    addMessage(contactId, message) {
        if (!this.messages.has(contactId)) {
            this.messages.set(contactId, []);
        }
        
        this.messages.get(contactId).push(message);
        this.saveMessages();
    }

    updateMessageStatus(messageId, status) {
        // Update in storage
        for (const [contactId, messages] of this.messages) {
            const message = messages.find(m => m.id === messageId);
            if (message) {
                message.status = status;
                break;
            }
        }
        
        // Update in UI
        const messageElement = document.querySelector(`[data-message-id="${messageId}"]`);
        if (messageElement) {
            const statusElement = messageElement.querySelector('.message-status');
            if (statusElement) {
                statusElement.className = `message-status ${status}`;
                statusElement.textContent = this.getStatusIcon(status);
            }
        }
        
        this.saveMessages();
    }

    getStatusIcon(status) {
        switch (status) {
            case 'sent': return '✓';
            case 'delivered': return '✓✓';
            case 'failed': return '✗';
            default: return '⏳';
        }
    }

    addContact(contactInfo) {
        this.contacts.set(contactInfo.id, contactInfo);
        this.updateContactsList();
    }

    updateContactsList() {
        const contactsContainer = document.getElementById('contacts');
        
        // Keep broadcast contact at top
        const broadcastContact = contactsContainer.querySelector('[data-id="broadcast"]');
        contactsContainer.innerHTML = '';
        if (broadcastContact) {
            contactsContainer.appendChild(broadcastContact);
        }

        // Add other contacts
        for (const [id, contact] of this.contacts) {
            if (id === 'broadcast') continue;
            
            const contactElement = document.createElement('div');
            contactElement.className = 'contact';
            contactElement.dataset.id = id;
            
            const timeDiff = Date.now() - contact.lastSeen;
            const lastSeenStr = timeDiff < 60000 ? 'Now' : 
                              timeDiff < 3600000 ? `${Math.floor(timeDiff/60000)}m ago` :
                              `${Math.floor(timeDiff/3600000)}h ago`;
            
            contactElement.innerHTML = `
                <div class="contact-avatar">${contact.avatar || '📱'}</div>
                <div class="contact-info">
                    <span class="contact-name">${contact.name || id}</span>
                    <span class="contact-status">${lastSeenStr}</span>
                </div>
            `;
            
            contactsContainer.appendChild(contactElement);
        }
    }

    receiveMessage(senderId, text, isEmergency = false, timestamp = Date.now()) {
        // Add sender as contact if not exists
        if (!this.contacts.has(senderId)) {
            this.addContact({
                id: senderId,
                name: senderId.substring(0, 8) + '...',
                lastSeen: timestamp
            });
        } else {
            // Update last seen
            this.contacts.get(senderId).lastSeen = timestamp;
            this.updateContactsList();
        }

        const message = {
            id: this.messageId++,
            text: text,
            timestamp: timestamp,
            sent: false,
            emergency: isEmergency,
            contactId: senderId
        };

        // Add to storage
        this.addMessage(senderId, message);

        // Add to UI if this contact is currently selected
        if (this.currentContact === senderId) {
            this.addMessageElement(message);
        }

        // Show notification if not in focus or different contact
        if (document.hidden || this.currentContact !== senderId) {
            this.showNotification(senderId, text, isEmergency);
        }
    }

    showNotification(senderId, text, isEmergency) {
        if (!('Notification' in window)) {
            return;
        }

        if (Notification.permission === 'granted') {
            const title = isEmergency ? '🚨 Emergency Message' : 'New Message';
            const body = `From ${this.contacts.get(senderId)?.name || senderId}: ${text}`;
            
            new Notification(title, {
                body: body,
                icon: './icons/icon-192x192.png',
                tag: 'meshchat-message'
            });
        }
    }

    saveMessages() {
        try {
            const data = {
                messages: Object.fromEntries(this.messages),
                contacts: Object.fromEntries(this.contacts)
            };
            localStorage.setItem('meshchat-data', JSON.stringify(data));
        } catch (error) {
            console.error('Failed to save messages:', error);
        }
    }

    loadStoredMessages() {
        try {
            const data = localStorage.getItem('meshchat-data');
            if (data) {
                const parsed = JSON.parse(data);
                
                if (parsed.messages) {
                    this.messages = new Map(Object.entries(parsed.messages));
                }
                
                if (parsed.contacts) {
                    this.contacts = new Map(Object.entries(parsed.contacts));
                    this.updateContactsList();
                }
            }
        } catch (error) {
            console.error('Failed to load stored messages:', error);
        }
    }

    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}

// Global instance
let chatManager = null;

function initializeChat() {
    chatManager = new ChatManager();
    chatManager.initialize();

    // Request notification permission
    if ('Notification' in window && Notification.permission === 'default') {
        Notification.requestPermission();
    }
}

function addMessageToChat(text, sent = false, emergency = false, timestamp = Date.now()) {
    if (chatManager) {
        if (sent) {
            // This would be for sent messages, but we handle those in sendMessage
        } else {
            // Received message - for now assume from broadcast
            chatManager.receiveMessage('broadcast', text, emergency, timestamp);
        }
    }
}

function updateMessageStatus(messageId, status) {
    if (chatManager) {
        chatManager.updateMessageStatus(messageId, status);
    }
}

// Export for use in other modules
window.chatManager = chatManager;
