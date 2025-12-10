import os
import time
from core.compiler.compiler_pipeline import CompilerPipeline

class Builder:
    def __init__(self):
        self.pipeline = CompilerPipeline()
        
    def build(self, source_path: str, output_path: str = None) -> bool:
        if not os.path.exists(source_path):
            print(f"Error: Source file {source_path} not found.")
            return False
            
        try:
            with open(source_path, 'r') as f:
                code = f.read()
                
            print("Compiling...")
            start = time.time()
            ir_module = self.pipeline.compile(code)
            
            # Serialize IR to file (mock)
            out_file = output_path if output_path else source_path.replace(".prox", ".prob")
            with open(out_file, 'w') as f:
                f.write(str(ir_module))
                
            print(f"Output written to {out_file} ({time.time() - start:.4f}s)")
            return True
            
        except Exception as e:
            print(f"Build Error: {e}")
            return False
