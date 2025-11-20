#!/usr/bin/env python3
"""
Patch WireGuard-ESP32 library to add shutdown_timer_only() method.
This script is automatically called by PlatformIO after library installation.
"""
import os
import sys

def patch_file(filepath, old_content, new_content, description):
    """Apply a patch to a file if not already applied."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check if already patched
        if new_content in content:
            print(f"✓ {description} - already patched")
            return True
        
        # Apply patch
        if old_content in content:
            content = content.replace(old_content, new_content)
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"✓ {description} - patch applied")
            return True
        else:
            print(f"✗ {description} - patch location not found!")
            return False
    except Exception as e:
        print(f"✗ {description} - error: {e}")
        return False

def main():
    # Get the library path from environment or use default
    lib_path = os.path.join('.pio', 'libdeps', 'ESP32_S3_16_8_YELLOW_AP', 'WireGuard-ESP32')
    
    if not os.path.exists(lib_path):
        print(f"Error: WireGuard-ESP32 library not found at {lib_path}")
        sys.exit(1)
    
    print("=" * 70)
    print("Patching WireGuard-ESP32 library...")
    print("=" * 70)
    
    success = True
    
    # Patch 1: Add method declaration to header
    header_file = os.path.join(lib_path, 'src', 'WireGuard-ESP32.h')
    old_header = '''public:
    bool begin(const IPAddress& localIP, const char* privateKey, const char* remotePeerAddress, const char* remotePeerPublicKey, uint16_t remotePeerPort);
    void end();
    bool is_initialized() const { return this->_is_initialized; }
};'''
    
    new_header = '''public:
    bool begin(const IPAddress& localIP, const char* privateKey, const char* remotePeerAddress, const char* remotePeerPublicKey, uint16_t remotePeerPort);
    void end();
    void shutdown_timer_only(); // New: Safe shutdown without netif_remove()
    bool is_initialized() const { return this->_is_initialized; }
};'''
    
    success &= patch_file(header_file, old_header, new_header, "Header file (WireGuard-ESP32.h)")
    
    # Patch 2: Add method implementation
    cpp_file = os.path.join(lib_path, 'src', 'WireGuard.cpp')
    old_cpp = '''	this->_is_initialized = true;
	return true;
}

void WireGuard::end() {
	if( !this->_is_initialized ) return;'''
    
    new_cpp = '''	this->_is_initialized = true;
	return true;
}

// CRITICAL: New method to stop timer and safely remove interface
void WireGuard::shutdown_timer_only() {
	if( !this->_is_initialized ) return;
	
	log_i(TAG "Safe WireGuard shutdown starting...");
	
	// Step 1: Disconnect peer FIRST (stops sending keepalives)
	wireguardif_disconnect(wg_netif, wireguard_peer_index);
	log_i(TAG "Peer disconnected");
	
	// Step 2: Bring interface down (stops routing NEW packets)
	netif_set_down(wg_netif);
	netif_set_link_down(wg_netif);
	log_i(TAG "Interface and link disabled");
	
	// Step 3: Wait for in-flight packets to finish (critical!)
	// This prevents TCP race condition where packets are still being sent
	vTaskDelay(pdMS_TO_TICKS(100));
	log_i(TAG "Waited for in-flight packets");
	
	// Step 4: Shutdown (stops timer)
	wireguardif_shutdown(wg_netif);
	log_i(TAG "Timer stopped");
	
	// Step 5: Remove netif from lwIP (now safe - no more traffic)
	netif_remove(wg_netif);
	log_i(TAG "Network interface removed");
	
	// Step 6: Mark as not initialized
	this->_is_initialized = false;
	wg_netif = nullptr;
	wireguard_peer_index = WIREGUARDIF_INVALID_INDEX;
}

void WireGuard::end() {
	if( !this->_is_initialized ) return;'''
    
    success &= patch_file(cpp_file, old_cpp, new_cpp, "Implementation file (WireGuard.cpp)")
    
    print("=" * 70)
    if success:
        print("✓ All patches applied successfully!")
        print("=" * 70)
        sys.exit(0)
    else:
        print("✗ Some patches failed to apply!")
        print("=" * 70)
        sys.exit(1)

if __name__ == "__main__":
    main()
