import time
import os
import sys
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
from cli.build.builder import Builder

class BuildHandler(FileSystemEventHandler):
    def __init__(self, file_path):
        self.file_path = file_path
        self.builder = Builder()
        self.last_build = 0

    def on_modified(self, event):
        if event.src_path.endswith(self.file_path):
            # Debounce
            if time.time() - self.last_build < 1.0:
                return
            
            print(f"\n[Watcher] {self.file_path} changed. Rebuilding...")
            self.builder.build(self.file_path)
            self.last_build = time.time()

class Watcher:
    def __init__(self, file_path):
        self.file_path = file_path

    def start(self):
        event_handler = BuildHandler(self.file_path)
        observer = Observer()
        directory = os.path.dirname(os.path.abspath(self.file_path))
        observer.schedule(event_handler, directory, recursive=False)
        observer.start()
        print(f"[Watcher] Watching {self.file_path} for changes...")
        
        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            observer.stop()
        observer.join()

if __name__ == "__main__":
    # Test watcher
    pass
