import base64
import hashlib

class ProXEncryptor:
    """
    Handles internal encryption for secure code packaging.
    Note: For a real product, use strong crypto libraries like cryptography/PyCryptodome.
    This simulates encryption using obfuscation + signing.
    """
    
    def encrypt_bytecode(self, bytecode: bytes, key: str) -> bytes:
        # Simple XOR obfuscation for demo
        # In reality: AES-256-GCM
        key_bytes = key.encode('utf-8')
        encrypted = bytearray()
        for i, b in enumerate(bytecode):
            encrypted.append(b ^ key_bytes[i % len(key_bytes)])
        return base64.b64encode(encrypted)

    def decrypt_bytecode(self, encrypted_b64: bytes, key: str) -> bytes:
        encrypted = base64.b64decode(encrypted_b64)
        key_bytes = key.encode('utf-8')
        decrypted = bytearray()
        for i, b in enumerate(encrypted):
            decrypted.append(b ^ key_bytes[i % len(key_bytes)])
        return bytes(decrypted)

encryptor = ProXEncryptor()
