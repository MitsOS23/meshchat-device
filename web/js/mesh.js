// Mesh network visualization
class MeshVisualizer {
    constructor() {
        this.canvas = null;
        this.ctx = null;
        this.nodes = new Map(); // deviceId -> {x, y, connections, rssi, lastSeen}
        this.isVisible = false;
        this.animationId = null;
    }

    initialize() {
        this.canvas = document.getElementById('meshCanvas');
        this.ctx = this.canvas.getContext('2d');
        
        // Setup overlay controls
        const closeMeshBtn = document.getElementById('closeMeshBtn');
        closeMeshBtn.addEventListener('click', () => this.hide());
        
        // Add mesh view button to header (optional)
        this.addMeshButton();
        
        // Add our own device as center node
        this.addNode('self', {
            x: this.canvas.width / 2,
            y: this.canvas.height / 2,
            connections: [],
            rssi: -50,
            lastSeen: Date.now(),
            isOwnDevice: true
        });
    }

    addMeshButton() {
        const header = document.querySelector('.header-content');
        const meshBtn = document.createElement('button');
        meshBtn.textContent = '🕸️ Network';
        meshBtn.className = 'mesh-btn';
        meshBtn.style.cssText = `
            background: #2196F3;
            color: white;
            border: none;
            padding: 0.5rem 1rem;
            border-radius: 6px;
            cursor: pointer;
            font-size: 0.9rem;
        `;
        meshBtn.addEventListener('click', () => this.show());
        header.appendChild(meshBtn);
    }

    show() {
        document.getElementById('meshOverlay').style.display = 'flex';
        this.isVisible = true;
        this.startAnimation();
    }

    hide() {
        document.getElementById('meshOverlay').style.display = 'none';
        this.isVisible = false;
        this.stopAnimation();
    }

    startAnimation() {
        if (!this.animationId) {
            this.animate();
        }
    }

    stopAnimation() {
        if (this.animationId) {
            cancelAnimationFrame(this.animationId);
            this.animationId = null;
        }
    }

    animate() {
        this.draw();
        this.animationId = requestAnimationFrame(() => this.animate());
    }

    addNode(deviceId, nodeData) {
        // Auto-position new nodes if no position specified
        if (!nodeData.x || !nodeData.y) {
            const position = this.calculateNodePosition(deviceId);
            nodeData.x = position.x;
            nodeData.y = position.y;
        }
        
        this.nodes.set(deviceId, {
            ...nodeData,
            lastSeen: Date.now()
        });
        
        if (this.isVisible) {
            this.draw();
        }
    }

    updateNode(deviceId, updates) {
        const node = this.nodes.get(deviceId);
        if (node) {
            Object.assign(node, updates, { lastSeen: Date.now() });
            if (this.isVisible) {
                this.draw();
            }
        }
    }

    removeNode(deviceId) {
        this.nodes.delete(deviceId);
        // Remove connections to this node
        for (const [id, node] of this.nodes) {
            node.connections = node.connections.filter(conn => conn.deviceId !== deviceId);
        }
        
        if (this.isVisible) {
            this.draw();
        }
    }

    addConnection(fromDevice, toDevice, rssi = -70) {
        const fromNode = this.nodes.get(fromDevice);
        const toNode = this.nodes.get(toDevice);
        
        if (fromNode && toNode) {
            // Remove existing connection if it exists
            fromNode.connections = fromNode.connections.filter(conn => conn.deviceId !== toDevice);
            
            // Add new connection
            fromNode.connections.push({
                deviceId: toDevice,
                rssi: rssi,
                timestamp: Date.now()
            });
        }
    }

    calculateNodePosition(deviceId) {
        const centerX = this.canvas.width / 2;
        const centerY = this.canvas.height / 2;
        
        // Arrange nodes in a circle around the center
        const nodeCount = this.nodes.size;
        const angle = (nodeCount * 2 * Math.PI) / 8; // Spread around circle
        const radius = 60 + (nodeCount * 10); // Expand radius as more nodes are added
        
        return {
            x: centerX + Math.cos(angle) * radius,
            y: centerY + Math.sin(angle) * radius
        };
    }

    draw() {
        if (!this.ctx) return;
        
        // Clear canvas
        this.ctx.fillStyle = '#f5f5f5';
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
        
        const now = Date.now();
        
        // Draw connections first (so they appear behind nodes)
        this.drawConnections();
        
        // Draw nodes
        for (const [deviceId, node] of this.nodes) {
            this.drawNode(deviceId, node, now);
        }
        
        // Draw legend
        this.drawLegend();
    }

    drawConnections() {
        for (const [deviceId, node] of this.nodes) {
            for (const connection of node.connections) {
                const targetNode = this.nodes.get(connection.deviceId);
                if (targetNode) {
                    this.drawConnection(node, targetNode, connection.rssi);
                }
            }
        }
    }

    drawConnection(fromNode, toNode, rssi) {
        const strength = Math.max(0, (100 + rssi) / 40); // Convert RSSI to strength (0-1)
        
        this.ctx.beginPath();
        this.ctx.moveTo(fromNode.x, fromNode.y);
        this.ctx.lineTo(toNode.x, toNode.y);
        
        // Color based on signal strength
        if (strength > 0.7) {
            this.ctx.strokeStyle = '#4CAF50'; // Strong - Green
        } else if (strength > 0.4) {
            this.ctx.strokeStyle = '#FF9800'; // Medium - Orange
        } else {
            this.ctx.strokeStyle = '#f44336'; // Weak - Red
        }
        
        this.ctx.lineWidth = Math.max(1, strength * 3);
        this.ctx.globalAlpha = 0.6;
        this.ctx.stroke();
        this.ctx.globalAlpha = 1;
    }

    drawNode(deviceId, node, currentTime) {
        const isStale = (currentTime - node.lastSeen) > 60000; // 1 minute
        
        // Node circle
        this.ctx.beginPath();
        this.ctx.arc(node.x, node.y, 15, 0, 2 * Math.PI);
        
        if (node.isOwnDevice) {
            this.ctx.fillStyle = '#2196F3'; // Blue for own device
        } else if (isStale) {
            this.ctx.fillStyle = '#9E9E9E'; // Gray for stale nodes
        } else {
            this.ctx.fillStyle = '#4CAF50'; // Green for active nodes
        }
        
        this.ctx.fill();
        
        // Border
        this.ctx.strokeStyle = '#fff';
        this.ctx.lineWidth = 2;
        this.ctx.stroke();
        
        // Device icon/text
        this.ctx.fillStyle = '#fff';
        this.ctx.font = '12px Arial';
        this.ctx.textAlign = 'center';
        this.ctx.textBaseline = 'middle';
        
        if (node.isOwnDevice) {
            this.ctx.fillText('ME', node.x, node.y);
        } else {
            this.ctx.fillText(deviceId.substring(0, 2), node.x, node.y);
        }
        
        // Device label
        this.ctx.fillStyle = '#333';
        this.ctx.font = '10px Arial';
        this.ctx.textAlign = 'center';
        this.ctx.textBaseline = 'top';
        
        const label = node.isOwnDevice ? 'This Device' : deviceId.substring(0, 8);
        this.ctx.fillText(label, node.x, node.y + 20);
        
        // RSSI indicator
        if (node.rssi) {
            this.ctx.fillStyle = '#666';
            this.ctx.font = '8px Arial';
            this.ctx.fillText(`${node.rssi}dBm`, node.x, node.y + 32);
        }
    }

    drawLegend() {
        const legendX = 10;
        const legendY = 10;
        
        this.ctx.fillStyle = 'rgba(255, 255, 255, 0.9)';
        this.ctx.fillRect(legendX, legendY, 150, 80);
        this.ctx.strokeStyle = '#ddd';
        this.ctx.strokeRect(legendX, legendY, 150, 80);
        
        this.ctx.fillStyle = '#333';
        this.ctx.font = '12px Arial';
        this.ctx.textAlign = 'left';
        this.ctx.textBaseline = 'top';
        
        this.ctx.fillText('Network Status:', legendX + 5, legendY + 5);
        
        // Connection strength indicators
        const indicators = [
            { color: '#4CAF50', text: 'Strong (-40 to -60 dBm)' },
            { color: '#FF9800', text: 'Medium (-60 to -80 dBm)' },
            { color: '#f44336', text: 'Weak (-80+ dBm)' }
        ];
        
        this.ctx.font = '10px Arial';
        indicators.forEach((indicator, index) => {
            const y = legendY + 20 + (index * 15);
            
            // Color indicator
            this.ctx.fillStyle = indicator.color;
            this.ctx.fillRect(legendX + 5, y, 10, 2);
            
            // Text
            this.ctx.fillStyle = '#333';
            this.ctx.fillText(indicator.text, legendX + 20, y - 2);
        });
    }

    // Update network from received data
    updateNetworkFromData(networkData) {
        if (networkData.routes) {
            // Clear existing connections
            for (const [id, node] of this.nodes) {
                node.connections = [];
            }
            
            // Add nodes and connections from route data
            networkData.routes.forEach(route => {
                // Add destination node if not exists
                if (!this.nodes.has(route.destination)) {
                    this.addNode(route.destination, {
                        connections: [],
                        rssi: route.rssi || -70,
                        lastSeen: Date.now()
                    });
                }
                
                // Add next hop node if not exists
                if (!this.nodes.has(route.nextHop)) {
                    this.addNode(route.nextHop, {
                        connections: [],
                        rssi: route.rssi || -70,
                        lastSeen: Date.now()
                    });
                }
                
                // Add connection
                this.addConnection('self', route.nextHop, route.rssi);
            });
        }
        
        if (networkData.neighbors) {
            networkData.neighbors.forEach(neighbor => {
                if (!this.nodes.has(neighbor.deviceId)) {
                    this.addNode(neighbor.deviceId, {
                        connections: [],
                        rssi: neighbor.rssi,
                        lastSeen: Date.now()
                    });
                } else {
                    this.updateNode(neighbor.deviceId, {
                        rssi: neighbor.rssi
                    });
                }
                
                this.addConnection('self', neighbor.deviceId, neighbor.rssi);
            });
        }
    }

    // Cleanup old nodes
    cleanup() {
        const now = Date.now();
        const timeout = 5 * 60 * 1000; // 5 minutes
        
        for (const [deviceId, node] of this.nodes) {
            if (!node.isOwnDevice && (now - node.lastSeen) > timeout) {
                this.removeNode(deviceId);
            }
        }
    }
}

// Global instance
let meshVisualizer = null;

function initializeMesh() {
    meshVisualizer = new MeshVisualizer();
    meshVisualizer.initialize();

    // Cleanup old nodes periodically
    setInterval(() => {
        if (meshVisualizer) {
            meshVisualizer.cleanup();
        }
    }, 60000); // Every minute
}

// Export for use in other modules
window.meshVisualizer = meshVisualizer;
