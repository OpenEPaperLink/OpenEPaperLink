Import("env")
import os
import time
from os.path import join, isfile

def patch_wireguard():
    """Patch WireGuard-ESP32 library after it's installed"""
    print("=" * 70)
    print("Pre-build: Patching WireGuard-ESP32 library...")
    print("=" * 70)
    
    # Get the library path for current environment
    libdeps_dir = env.subst("$PROJECT_LIBDEPS_DIR")
    pioenv = env.subst("$PIOENV")
    wg_lib_path = join(libdeps_dir, pioenv, "WireGuard-ESP32")
    
    # Wait for library to be installed (max 60 seconds)
    for i in range(60):
        if os.path.exists(wg_lib_path):
            break
        if i == 0:
            print(f"Waiting for WireGuard-ESP32 library installation at {wg_lib_path}...")
        time.sleep(1)
    
    if not os.path.exists(wg_lib_path):
        print(f"Error: WireGuard-ESP32 library not found at {wg_lib_path} after waiting")
        print("=" * 70)
        return
    
    header_file = join(wg_lib_path, "src", "WireGuard-ESP32.h")
    cpp_file = join(wg_lib_path, "src", "WireGuard.cpp")
    
    success = True
    
    # Patch header file
    if isfile(header_file):
        with open(header_file, 'r', encoding='utf-8') as f:
            header_content = f.read()
        
        # Check if already patched
        if 'shutdown_timer_only' in header_content:
            print("✓ Header file (WireGuard-ESP32.h) - already patched")
        else:
            old_header = '''    bool begin(const IPAddress& localIP, const char* privateKey, const char* remotePeerAddress, const char* remotePeerPublicKey, uint16_t remotePeerPort);
    void end();
    bool is_initialized() const { return this->_is_initialized; }'''
            
            new_header = '''    bool begin(const IPAddress& localIP, const char* privateKey, const char* remotePeerAddress, const char* remotePeerPublicKey, uint16_t remotePeerPort);
    void end();
    void shutdown_timer_only();
    bool is_initialized() const { return this->_is_initialized; }'''
            
            if old_header in header_content:
                header_content = header_content.replace(old_header, new_header)
                with open(header_file, 'w', encoding='utf-8') as f:
                    f.write(header_content)
                print("✓ Header file (WireGuard-ESP32.h) - patch applied")
            else:
                print("✗ Header file (WireGuard-ESP32.h) - patch location not found!")
                success = False
    else:
        print(f"✗ Header file not found: {header_file}")
        success = False
    
    # Patch implementation file
    if isfile(cpp_file):
        with open(cpp_file, 'r', encoding='utf-8') as f:
            cpp_content = f.read()
        
        # Check if already patched
        if 'shutdown_timer_only' in cpp_content:
            print("✓ Implementation file (WireGuard.cpp) - already patched")
        else:
            old_cpp = '''\tthis->_is_initialized = true;
\treturn true;
}

void WireGuard::end() {'''
            
            new_cpp = '''\tthis->_is_initialized = true;
\treturn true;
}

void WireGuard::shutdown_timer_only() {
\tif( !this->_is_initialized ) return;
\t
\tlog_i(TAG "Safe WireGuard shutdown starting...");
\t
\t// Step 1: Disconnect peer FIRST (stops sending keepalives)
\twireguardif_disconnect(wg_netif, wireguard_peer_index);
\tlog_i(TAG "Peer disconnected");
\t
\t// Step 2: Bring interface down (stops routing NEW packets)
\tnetif_set_down(wg_netif);
\tnetif_set_link_down(wg_netif);
\tlog_i(TAG "Interface and link disabled");
\t
\t// Step 3: Wait for in-flight packets to finish (critical!)
\t// This prevents TCP race condition where packets are still being sent
\tvTaskDelay(pdMS_TO_TICKS(100));
\tlog_i(TAG "Waited for in-flight packets");
\t
\t// Step 4: Restore default interface
\tnetif_set_default(previous_default_netif);
\tprevious_default_netif = nullptr;
\t
\t// Step 5: Shutdown (stops timer via sys_untimeout)
\twireguardif_shutdown(wg_netif);
\tlog_i(TAG "Timer stopped");
\t
\t// Step 6: Remove netif from lwIP (now safe - no more traffic)
\tnetif_remove(wg_netif);
\tlog_i(TAG "Network interface removed");
\t
\t// Step 7: Mark as not initialized
\tthis->_is_initialized = false;
\twg_netif = nullptr;
\twireguard_peer_index = WIREGUARDIF_INVALID_INDEX;
}

void WireGuard::end() {'''
            
            if old_cpp in cpp_content:
                cpp_content = cpp_content.replace(old_cpp, new_cpp)
                with open(cpp_file, 'w', encoding='utf-8') as f:
                    f.write(cpp_content)
                print("✓ Implementation file (WireGuard.cpp) - patch applied")
            else:
                print("✗ Implementation file (WireGuard.cpp) - patch location not found!")
                success = False
    else:
        print(f"✗ Implementation file not found: {cpp_file}")
        success = False
    
    print("=" * 70)
    if success:
        print("✓ All patches applied successfully!")
    else:
        print("✗ Patching failed - build may fail!")
    print("=" * 70)

# Run patch immediately when script is loaded (after library installation)
patch_wireguard()
