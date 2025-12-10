import random
import string
from core.lexer.lexer import Lexer, LexerError

class Fuzzer:
    """
    Generates random garbage input to test Lexer robustness.
    """
    
    def generate_garbage(self, length=100):
        chars = string.printable
        return ''.join(random.choice(chars) for _ in range(length))

    def fuzz(self, iterations=100):
        crashes = 0
        print(f"Starting fuzzer ({iterations} iterations)...")
        
        for i in range(iterations):
            code = self.generate_garbage()
            try:
                lexer = Lexer(code)
                lexer.tokenize()
            except LexerError:
                # Expected behavior for garbage
                pass
            except Exception as e:
                print(f"CRASH detected on input: {code[:20]}... Error: {e}")
                crashes += 1
                
        print(f"Fuzzing complete. Crashes found: {crashes}")

if __name__ == '__main__':
    Fuzzer().fuzz()
