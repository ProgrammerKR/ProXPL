import unittest
import sys
import os

# Add root directory to sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from core.tokens import KEYWORDS, OPERATORS
from core.types import DATA_TYPES
from core.vm.vm import VM

class TestLanguageSpecs(unittest.TestCase):
    def test_counts(self):
        # 45 Keywords
        self.assertEqual(len(KEYWORDS), 45, f"Expected 45 keywords, found {len(KEYWORDS)}: {KEYWORDS.keys()}")
        
        # 12 Data Types
        self.assertEqual(len(DATA_TYPES), 12, f"Expected 12 data types, found {len(DATA_TYPES)}: {DATA_TYPES}")
        
        # 42 Operators
        self.assertEqual(len(OPERATORS), 42, f"Expected 42 operators, found {len(OPERATORS)}")
        
        # 75 Built-in Functions
        vm = VM()
        # StdLib is registered in VM __init__
        self.assertEqual(len(vm.natives), 75, f"Expected 75 built-ins, found {len(vm.natives)}: {sorted(vm.natives.keys())}")

if __name__ == '__main__':
    unittest.main()
