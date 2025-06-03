import socket
import json
import threading
import time
import struct

SERVER_HOST = "localhost" # Default host
SERVER_PORT = 65432       # Default port, matches client_ipc

class IPCServer:
    def __init__(self, game_context_ref, host=SERVER_HOST, port=SERVER_PORT):
        self.game_context_ref = game_context_ref  # Reference to the main GameContext
        self.host = host
        self.port = port
        self.server_socket: socket.socket = None
        self.client_connections: dict[socket.socket, any] = {} # Store conn: addr
        self.threads: list[threading.Thread] = []
        self.is_running: bool = False
        self.connections_lock = threading.Lock()
        print(f"IPCServer initialized for {self.host}:{self.port}")

    def _send_message(self, conn: socket.socket, message_dict: dict) -> bool:
        try:
            json_data = json.dumps(message_dict)
            message_bytes = json_data.encode('utf-8')
            header = struct.pack('>I', len(message_bytes))
            conn.sendall(header + message_bytes)
            # print(f"IPCServer: Sent {message_dict.get('type', 'message')} to {conn.getpeername()}")
            return True
        except socket.error as e:
            # print(f"IPCServer: Socket error while sending to {conn.getpeername()}: {e}")
            return False
        except Exception as e:
            # print(f"IPCServer: Error encoding/sending message: {e}")
            return False

    def _handle_client_thread(self, conn: socket.socket, addr):
        print(f"IPCServer: Client connected from {addr}")
        buffer = b""
        payload_len = None

        try:
            while self.is_running:
                try:
                    # Set a timeout for recv so the loop can check self.is_running periodically
                    conn.settimeout(1.0)
                    data_chunk = conn.recv(4096)
                    conn.settimeout(None) # Reset for other operations

                    if not data_chunk:
                        # print(f"IPCServer: Client {addr} disconnected (received empty data).")
                        break # Client closed connection

                    buffer += data_chunk

                    while True: # Process all messages in buffer
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

                                try:
                                    message_str = payload_bytes.decode('utf-8')
                                    data = json.loads(message_str)
                                    # print(f"IPCServer: Received from {addr}: {data.get('type', 'message')}")

                                    if data.get('type') == 'settings':
                                        if hasattr(self.game_context_ref, 'update_settings'):
                                            self.game_context_ref.update_settings(data.get('payload', {}))
                                        else:
                                            print("IPCServer: Error: game_context_ref has no update_settings method.")
                                    # Handle other message types if needed

                                except json.JSONDecodeError:
                                    print(f"IPCServer: Could not decode JSON from {addr}.")
                                except UnicodeDecodeError:
                                    print(f"IPCServer: Could not decode UTF-8 data from {addr}.")
                                finally:
                                    payload_len = None # Reset for next message
                            else:
                                break # Not enough data for full payload yet

                except socket.timeout:
                    # This is expected, allows checking self.is_running
                    continue
                except socket.error as e:
                    # print(f"IPCServer: Socket error with client {addr}: {e}")
                    break # Treat socket errors as disconnection
                except Exception as e:
                    print(f"IPCServer: Unexpected error with client {addr}: {e}")
                    break

        finally:
            print(f"IPCServer: Client {addr} session ended.")
            with self.connections_lock:
                if conn in self.client_connections:
                    del self.client_connections[conn]
            try:
                conn.shutdown(socket.SHUT_RDWR)
                conn.close()
            except socket.error:
                pass # Ignore errors on close, already disconnected or errored

    def broadcast_game_state(self):
        if not hasattr(self.game_context_ref, 'get_serializable_state'):
            print("IPCServer: Error: game_context_ref has no get_serializable_state method.")
            return

        game_state_data = {
            'type': 'game_state', # Using 'game_state' to be more generic than just 'esp_data'
            'payload': self.game_context_ref.get_serializable_state()
        }

        # print(f"IPCServer: Broadcasting game state. Payload keys: {game_state_data['payload'].keys()}")


        # Iterate over a copy of connections for safe removal
        # connections_to_remove = []
        # with self.connections_lock: # Lock during iteration over keys
        #     current_connections = list(self.client_connections.keys())

        # for conn in current_connections:
        #     if not self._send_message(conn, game_state_data):
        #         print(f"IPCServer: Failed to send game state to a client. Queuing for removal.")
        #         connections_to_remove.append(conn)

        # # Remove dead connections if any
        # if connections_to_remove:
        #     with self.connections_lock:
        #         for conn in connections_to_remove:
        #             if conn in self.client_connections:
        #                 print(f"IPCServer: Removing dead connection: {self.client_connections[conn]}")
        #                 del self.client_connections[conn]
        #                 try:
        #                     conn.close()
        #                 except socket.error: pass
        # Using a simpler approach for now: send and if it fails, client thread will handle cleanup.
        # The _send_message might be problematic if called from a different thread than _handle_client_thread
        # without proper locking around conn.sendall(). For now, assuming broadcast is called from main or a single dedicated thread.
        # For true broadcast safety, messages might need to be queued per client.
        # Simpler: let individual client threads handle their own sends if possible, or ensure this is called from a safe context.
        # For now, this broadcast is "best effort" and relies on client thread cleanup.

        with self.connections_lock:
            # Create a list of connections to iterate over, to avoid issues if a client disconnects during broadcast
            active_conns = list(self.client_connections.keys())

        # print(f"IPCServer: Broadcasting to {len(active_conns)} clients.")
        for conn in active_conns:
            if not self._send_message(conn, game_state_data):
                # print(f"IPCServer: Failed to broadcast to {self.client_connections.get(conn, 'unknown_client')}. It might have disconnected.")
                # The _handle_client_thread is responsible for removing the client from the list upon disconnection.
                pass


    def start_server(self):
        if self.is_running:
            print("IPCServer: Server is already running.")
            return

        self.is_running = True
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(5) # Max 5 pending connections
            print(f"IPCServer: Listening on {self.host}:{self.port}")

            while self.is_running:
                try:
                    self.server_socket.settimeout(1.0) # Timeout to allow checking self.is_running
                    conn, addr = self.server_socket.accept()
                    self.server_socket.settimeout(None)

                    with self.connections_lock:
                        self.client_connections[conn] = addr

                    client_thread = threading.Thread(target=self._handle_client_thread, args=(conn, addr), daemon=True)
                    self.threads.append(client_thread)
                    client_thread.start()

                except socket.timeout:
                    continue # Expected, allows checking self.is_running
                except socket.error as e:
                    if self.is_running: # Avoid error message if stopping
                        print(f"IPCServer: Error accepting connections: {e}")
                    break # Exit accept loop on significant error
                except Exception as e:
                    if self.is_running:
                        print(f"IPCServer: Unexpected error in accept loop: {e}")
                    break


        except Exception as e:
            print(f"IPCServer: Failed to start server: {e}")
            self.is_running = False # Ensure it's marked as not running
        finally:
            if self.server_socket and not self.is_running: # If loop exited due to error or stop
                try:
                    self.server_socket.close()
                    print("IPCServer: Server socket closed.")
                except socket.error as e:
                    print(f"IPCServer: Error closing server socket: {e}")
            print("IPCServer: Start_server loop ended.")


    def stop_server(self):
        print("IPCServer: Stopping server...")
        self.is_running = False # Signal all loops and threads to stop

        if self.server_socket:
            try:
                # To unblock server_socket.accept() if it's blocking without timeout
                # One way is to connect to it briefly if it's stuck without timeout
                # However, we used settimeout(1.0) in the accept loop, so it should unblock.
                self.server_socket.close()
                print("IPCServer: Server socket closed.")
            except socket.error as e:
                print(f"IPCServer: Error closing server socket: {e}")
            finally:
                self.server_socket = None

        with self.connections_lock:
            for conn in list(self.client_connections.keys()): # Iterate over a copy
                try:
                    conn.shutdown(socket.SHUT_RDWR)
                    conn.close()
                except socket.error:
                    pass # Ignore errors, connection might already be closed
            self.client_connections.clear()
            print("IPCServer: All client connections closed.")

        # print("IPCServer: Joining client handler threads...")
        # current_threads = self.threads[:] # Copy list for iteration
        # for thread in current_threads:
        #     if thread.is_alive():
        #         thread.join(timeout=1.0) # Wait for threads to finish
        #     if thread.is_alive():
        #         print(f"IPCServer: Thread {thread.name} did not terminate in time.")
        # self.threads.clear() # Clear after attempting to join
        # Daemon threads will exit automatically when main program exits if not joined.
        # For cleaner shutdown, explicit join is good, but daemon simplifies immediate exit.

        print("IPCServer: Stopped.")

if __name__ == '__main__':
    # Example Usage
    class MockGameContext:
        def update_settings(self, settings_data):
            print(f"MockGameContext: Settings updated: {settings_data}")

        def get_serializable_state(self):
            return {
                "status": "Running",
                "map_name": "Olympus",
                "spectators_count": 0,
                "allied_spectators_count": 0,
                "esp_players_data": [{"name": "TestPlayer", "health": 100, "shield": 50, "dist": 50.0}],
                "firing_range": False
            }

    mock_ctx = MockGameContext()
    server = IPCServer(mock_ctx)

    server_thread = threading.Thread(target=server.start_server, daemon=True)
    server_thread.start()

    try:
        while True:
            print("Main thread: Server running in background. Broadcasting state every 10s.")
            time.sleep(10)
            server.broadcast_game_state() # Periodically broadcast
            # To test stop, you'd need a condition here or external signal
    except KeyboardInterrupt:
        print("Main thread: Keyboard interrupt received.")
    finally:
        server.stop_server()
        if server_thread.is_alive():
             server_thread.join(timeout=2) # Wait for server thread to finish
        print("Main thread: Exiting.")
```
