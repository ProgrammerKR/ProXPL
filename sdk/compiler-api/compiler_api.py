from core.compiler.compiler_pipeline import CompilerPipeline

class CompilerAPI:
    """
    Public Python API for embedding ProXPL.
    """
    
    def __init__(self):
        self.pipeline = CompilerPipeline()
        
    def eval(self, source_code: str):
        """Compiles and runs a snippet of code immediately."""
        print("Compiling string...")
        self.pipeline.run(source_code)
        
    def compile_file(self, file_path: str, output_path: str = None):
        with open(file_path, 'r') as f:
            code = f.read()
        return self.pipeline.compile(code)

api = CompilerAPI()
