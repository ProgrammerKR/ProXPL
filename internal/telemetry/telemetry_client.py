import requests
import json
import threading

class TelemetryClient:
    """
    Collects anonymous usage data to improve ProXPL.
    """
    def __init__(self):
        self.endpoint = "https://telemetry.proxpl.dev/collect"
        self.enabled = True
        self.session_id = None

    def start_session(self):
        self.session_id = "sess_" + str(int(time.time()))
        
    def track_event(self, event_name: str, properties: dict = {}):
        if not self.enabled: return
        
        # Send in background thread to avoid blocking compiler
        threading.Thread(target=self._send, args=(event_name, properties)).start()

    def _send(self, event_name: str, properties: dict):
        try:
            payload = {
                "event": event_name,
                "session": self.session_id,
                "properties": properties
            }
            # requests.post(self.endpoint, json=payload, timeout=2)
            # Commented out to prevent actual network calls during dev/test
            pass
        except Exception:
            pass # Fail silently

import time
telemetry = TelemetryClient()
