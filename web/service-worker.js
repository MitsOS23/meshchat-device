const CACHE_NAME = 'meshchat-v1.0.0';
const urlsToCache = [
  './',
  './index.html',
  './css/style.css',
  './js/bluetooth.js',
  './js/chat.js',
  './js/mesh.js',
  './manifest.json',
  './icons/icon-192x192.png'
];

// Install event - cache resources
self.addEventListener('install', event => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then(cache => {
        console.log('Opened cache');
        return cache.addAll(urlsToCache);
      })
  );
});

// Fetch event - serve from cache when offline
self.addEventListener('fetch', event => {
  event.respondWith(
    caches.match(event.request)
      .then(response => {
        // Cache hit - return response
        if (response) {
          return response;
        }

        return fetch(event.request).then(response => {
          // Check if we received a valid response
          if (!response || response.status !== 200 || response.type !== 'basic') {
            return response;
          }

          // Clone the response
          const responseToCache = response.clone();

          caches.open(CACHE_NAME)
            .then(cache => {
              cache.put(event.request, responseToCache);
            });

          return response;
        });
      })
  );
});

// Activate event - clean up old caches
self.addEventListener('activate', event => {
  event.waitUntil(
    caches.keys().then(cacheNames => {
      return Promise.all(
        cacheNames.map(cacheName => {
          if (cacheName !== CACHE_NAME) {
            console.log('Deleting old cache:', cacheName);
            return caches.delete(cacheName);
          }
        })
      );
    })
  );
});

// Handle messages from main thread
self.addEventListener('message', event => {
  if (event.data && event.data.type === 'SKIP_WAITING') {
    self.skipWaiting();
  }
});

// Background sync for offline messages
self.addEventListener('sync', event => {
  if (event.tag === 'background-sync-messages') {
    event.waitUntil(syncMessages());
  }
});

async function syncMessages() {
  // This would sync pending messages when connection is restored
  try {
    const pendingMessages = await getStoredMessages();
    for (const message of pendingMessages) {
      await sendMessage(message);
    }
    await clearStoredMessages();
  } catch (error) {
    console.error('Background sync failed:', error);
  }
}

// Push notifications for new messages
self.addEventListener('push', event => {
  const options = {
    body: event.data ? event.data.text() : 'New mesh message received',
    icon: './icons/icon-192x192.png',
    badge: './icons/icon-72x72.png',
    vibrate: [100, 50, 100],
    data: {
      dateOfArrival: Date.now(),
      primaryKey: '1'
    },
    actions: [
      {
        action: 'view',
        title: 'View Message',
        icon: './icons/view-icon.png'
      },
      {
        action: 'reply',
        title: 'Reply',
        icon: './icons/reply-icon.png'
      }
    ]
  };

  event.waitUntil(
    self.registration.showNotification('MeshChat', options)
  );
});

// Handle notification clicks
self.addEventListener('notificationclick', event => {
  event.notification.close();

  if (event.action === 'view' || event.action === '') {
    // Open the app
    event.waitUntil(
      clients.openWindow('./')
    );
  } else if (event.action === 'reply') {
    // Handle quick reply (could open a minimal interface)
    event.waitUntil(
      clients.openWindow('./?action=reply')
    );
  }
});
