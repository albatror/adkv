import socket
import json
import threading
import time
import struct # For packing/unpacking the length header

# Attempt to import structs for PlayerData, but make it optional for now
# as this client_ipc module might be tested or run where py_apex_dma is not directly in path
try:
    from py_apex_dma import structs # Assuming this path might be valid in a larger context
except ImportError:
    print("client_ipc: Could not import py_apex_dma.structs. PlayerData type hint might be nominal.")
    # Define a placeholder if needed for type hinting, or just use dict.
    class PlayerDataPlaceholder: pass
    structs = PlayerDataPlaceholder() # Make structs.PlayerData accessible nominally


SERVER_HOST = "localhost"
SERVER_PORT = 65432

class IPCClient:
    def __init__(self, host, port, client_settings_instance, server_data_handler_cb):
        self.host = host
        self.port = port
        self.client_settings = client_settings_instance # Instance of ClientSettings
        self.server_data_handler_cb = server_data_handler_cb # Callback for received data

        self.sock: socket.socket = None
        self.is_running: bool = False
        self.connected: bool = False
        self.receive_thread: threading.Thread = None
        self.lock = threading.Lock() # For thread-safe access to shared resources like socket

    def connect(self) -> bool:
        with self.lock:
            if self.connected:
                return True
            try:
                self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.sock.settimeout(3.0) # Timeout for connection attempt
                self.sock.connect((self.host, self.port))
                self.sock.settimeout(None) # Reset timeout for blocking operations after connect
                self.connected = True
                print("IPCClient: Connected to server.")
                # Immediately send current settings upon connection
                # self.send_settings() # This might be better called explicitly after connect() returns true
                return True
            except socket.error as e:
                print(f"IPCClient: Connection failed: {e}")
                self.sock = None
                self.connected = False
                return False

    def disconnect(self):
        with self.lock:
            if self.sock:
                try:
                    self.sock.shutdown(socket.SHUT_RDWR)
                    self.sock.close()
                except socket.error as e:
                    print(f"IPCClient: Error during disconnect: {e}")
                finally:
                    self.sock = None
                    self.connected = False
                    print("IPCClient: Disconnected.")
            else: # If socket is already None, ensure connected is False
                self.connected = False


    def _send_message(self, message_dict: dict) -> bool:
        if not self.is_client_connected(): # Use the renamed method
            print("IPCClient: Not connected. Cannot send message.")
            return False

        with self.lock: # Ensure exclusive access to socket for sending
            if not self.sock: # Check socket again inside lock
                self.connected = False # Mark as disconnected if sock is None
                return False
            try:
                json_data = json.dumps(message_dict)
                message_bytes = json_data.encode('utf-8')
                # Prepend 4-byte length header (big-endian)
                header = struct.pack('>I', len(message_bytes))
                self.sock.sendall(header + message_bytes)
                # print(f"IPCClient: Sent: {message_dict['type']}")
                return True
            except socket.error as e:
                print(f"IPCClient: Socket error during send: {e}. Disconnecting.")
                self.disconnect() # Disconnect on send error
                return False
            except Exception as e:
                print(f"IPCClient: Error encoding/sending message: {e}")
                return False

    def send_settings(self):
        """Sends the current client settings to the server."""
        if hasattr(self.client_settings, 'to_dict') and callable(getattr(self.client_settings, 'to_dict')):
            # print("IPCClient: Sending settings...")
            return self._send_message({"type": "settings", "payload": self.client_settings.to_dict()})
        else:
            print("IPCClient: Error: client_settings_instance does not have a to_dict() method.")
            return False

    def _receive_loop(self):
        buffer = b""
        payload_len = None

        while self.is_running:
            if not self.is_client_connected(): # Use the renamed method
                print("IPCClient: Not connected. Attempting to reconnect...")
                if not self.connect(): # connect() already sets self.connected
                    time.sleep(5) # Wait before retrying connection
                    continue
                else: # Successfully connected, try sending settings again
                    self.send_settings()


            if not self.sock: # Should not happen if connected is True, but as a safeguard
                print("IPCClient: Socket is None in receive loop despite being connected. Re-evaluating connection.")
                self.disconnect() # This will set self.connected to False
                continue

            try:
                # Set a timeout for recv so the loop can check self.is_running periodically
                self.sock.settimeout(1.0)
                data = self.sock.recv(4096)
                self.sock.settimeout(None) # Reset for other operations if any

                if not data:
                    print("IPCClient: Connection closed by server (received empty data).")
                    self.disconnect()
                    continue

                buffer += data

                while True:
                    if payload_len is None: # Need to read header
                        if len(buffer) >= 4:
                            payload_len = struct.unpack('>I', buffer[:4])[0]
                            buffer = buffer[4:]
                        else:
                            break # Not enough data for header yet

                    if payload_len is not None: # Header received, try to get payload
                        if len(buffer) >= payload_len:
                            payload_bytes = buffer[:payload_len]
                            buffer = buffer[payload_len:]
                            payload_len = None # Reset for next message

                            try:
                                message_str = payload_bytes.decode('utf-8')
                                message_dict = json.loads(message_str)
                                if self.server_data_handler_cb:
                                    self.server_data_handler_cb(message_dict)
                            except json.JSONDecodeError:
                                print("IPCClient: Could not decode JSON from server.")
                            except UnicodeDecodeError:
                                print("IPCClient: Could not decode UTF-8 data from server.")
                        else:
                            break # Not enough data for full payload yet

            except socket.timeout:
                # This is expected, just allows checking self.is_running
                continue
            except socket.error as e:
                print(f"IPCClient: Socket error during receive: {e}. Disconnecting.")
                self.disconnect()
            except Exception as e:
                print(f"IPCClient: Unexpected error in receive loop: {e}")
                self.disconnect() # Disconnect on other errors too
                time.sleep(1) # Brief pause before trying to recover

        print("IPCClient: Receive loop ended.")


    def start(self):
        if self.is_running:
            print("IPCClient: Already running.")
            return

        print("IPCClient: Starting...")
        self.is_running = True
        # Attempt initial connection synchronously before starting thread
        # if self.connect():
            # self.send_settings() # Send initial settings

        self.receive_thread = threading.Thread(target=self._receive_loop, daemon=True)
        self.receive_thread.start()

    def stop(self):
        print("IPCClient: Stopping...")
        self.is_running = False
        if self.receive_thread and self.receive_thread.is_alive():
            self.receive_thread.join(timeout=5.0) # Wait for the thread to finish
            if self.receive_thread.is_alive():
                print("IPCClient: Receive thread did not terminate in time.")
        self.disconnect()
        print("IPCClient: Stopped.")

    def is_client_connected(self) -> bool: # Renamed from is_connected to avoid dpg name clashes
        return self.connected


if __name__ == '__main__':
    # Example Usage (requires a server to connect to)

    # Dummy ClientSettings with to_dict
    class DummySettings:
        def to_dict(self):
            return {"dummy_setting": True, "value": 123}

    def dummy_handler(data):
        print(f"Test IPC Handler received: {data}")

    settings_instance = DummySettings()
    ipc_client = IPCClient(SERVER_HOST, SERVER_PORT, settings_instance, dummy_handler)

    ipc_client.start()

    try:
        count = 0
        while True:
            time.sleep(5)
            if ipc_client.is_client_connected():
                print(f"Main thread: Client connected. Sending updated settings... ({count})")
                settings_instance.current_time = time.time() # Simulate settings change
                ipc_client.send_settings()
                count +=1
            else:
                print("Main thread: Client not connected.")
            if count > 3: # Run for a bit then stop
                break
    except KeyboardInterrupt:
        print("Test interrupted by user.")
    finally:
        ipc_client.stop()
```
