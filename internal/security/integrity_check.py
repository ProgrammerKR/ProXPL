import hashlib
import os

class IntegrityChecker:
    """
    Verifies the integrity of ProXPL compiler and standard library files
    to prevent tampering.
    """
    
    def calculate_hash(self, file_path: str) -> str:
        sha256 = hashlib.sha256()
        with open(file_path, 'rb') as f:
            while True:
                data = f.read(65536)
                if not data:
                    break
                sha256.update(data)
        return sha256.hexdigest()

    def verify_file(self, file_path: str, expected_hash: str) -> bool:
        if not os.path.exists(file_path):
            return False
        current_hash = self.calculate_hash(file_path)
        return current_hash == expected_hash

checker = IntegrityChecker()
