import os
import sys
import shutil
import unittest
from contextlib import redirect_stdout
from io import StringIO

# Add project root to path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from cli.prm import install_pkg, remove_pkg, list_pkgs
from core.compiler.compiler_pipeline import CompilerPipeline

class TestPRMAndModules(unittest.TestCase):
    def setUp(self):
        # Clean packages dir
        if os.path.exists("packages"):
            shutil.rmtree("packages")
    
    def tearDown(self):
        if os.path.exists("packages"):
            shutil.rmtree("packages")

    def test_prm_install_and_import(self):
        print("\n--- Testing PRM Install + Import ---")
        
        # 1. Install 'utils/io' from mock registry
        install_pkg("utils/io")
        
        # Verify file exists
        self.assertTrue(os.path.exists("packages/utils/io.prox"))
        
        # 2. Create a script that uses it
        source_code = """
        use utils/io;
        
        func main() {
            print_line("Integration Test Successful");
        }
        main();
        """
        
        # 3. Run Pipeline
        captured_output = StringIO()
        pipeline = CompilerPipeline()
        
        # Mock print to capture output? 
        # The VM instructions call 'print'.
        # I'll rely on the fact that if it fails to resolve, it raises error.
        
        try:
            # We can't easily capture VM output without mocking, 
            # but we can check if it compiles and runs without error.
            pipeline.run(source_code, raise_errors=True)
        except Exception as e:
            self.fail(f"Pipeline failed: {e}")

    def test_circular_dependency(self):
        print("\n--- Testing Circular Dependency ---")
        os.makedirs("packages", exist_ok=True)
        
        with open("packages/A.prox", "w") as f:
            f.write("use B;")
            
        with open("packages/B.prox", "w") as f:
            f.write("use A;")
            
        source_code = "use A;"
        pipeline = CompilerPipeline()
        
        with self.assertRaises(Exception) as cm:
            pipeline.run(source_code, raise_errors=True)
            
        print(f"Caught expected error: {cm.exception}")
        self.assertIn("Circular dependency", str(cm.exception))

if __name__ == '__main__':
    unittest.main()
