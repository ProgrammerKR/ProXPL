import os
from typing import Dict, List, Set, Optional
from core.lexer.lexer import Lexer
from core.parser.parser import Parser
from core.ast import ast_nodes as ast

class circular_dependency_error(Exception): pass

class Importer:
    """
    Handles resolving and loading modules.
    - Resolves paths: CWD -> packages/ -> stdlib/
    - Caches loaded modules/ASTs to prevent re-execution (optional, or just single-include).
    - Detects circular imports.
    """
    def __init__(self, root_dir: str = "."):
        self.root_dir = os.path.abspath(root_dir)
        # loaded_modules: map resolved_path -> (AST, scope/env if we were doing persistent env)
        # For now, we only need to return the AST to be injected or run.
        # IF we want "execute only once", we should track that. 
        # But commonly in dynamic langs, "use" might just import symbols.
        # User prompt: "Execute imported module code only once"
        self.loaded_modules: Set[str] = set() 
        self.processing_stack: List[str] = [] # For cycle detection
        
        # Paths to search
        self.search_paths = [
            self.root_dir,
            os.path.join(self.root_dir, "packages"),
            os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "stdlib"), # .../ProXPL/stdlib
        ]

    def resolve_path(self, module_name: str) -> str:
        # module_name could be "math", "utils/io"
        # We look for module_name + ".prox" in search_paths
        filename = module_name + ".prox"
        
        for path in self.search_paths:
            full_path = os.path.join(path, filename)
            if os.path.exists(full_path):
                return os.path.abspath(full_path)
                
        # Also check for package index/main logic if we were fancy, but for now just single file
        raise FileNotFoundError(f"Module '{module_name}' not found in paths: {self.search_paths}")

    def load_module(self, module_name: str) -> Optional[List[ast.Stmt]]:
        try:
            full_path = self.resolve_path(module_name)
        except FileNotFoundError as e:
            print(f"Import Error: {e}")
            return None # Or raise
            
        if full_path in self.loaded_modules:
            return [] # Already loaded, don't execute again. 
                      # NOTE: If we need symbols, we'd need to return the scope.
                      # But current VM/Interpreter likely runs everything in global or passed scope.
                      # "Execute imported module code only once" implies side effects.
                      # If it declares functions/classes, they are side effects on the environment.
        
        if full_path in self.processing_stack:
            cycle = " -> ".join([os.path.basename(p) for p in self.processing_stack] + [os.path.basename(full_path)])
            raise circular_dependency_error(f"Circular dependency detected: {cycle}")
            
        self.processing_stack.append(full_path)
        
        try:
            with open(full_path, 'r') as f:
                source = f.read()
            
            # recursive parsing?
            # Ideally: Parse -> AST. 
            # Check AST for more "use" decls? 
            # Flattening approach: CompilerPipeline uses Importer to flatten everything into one big AST?
            # Or VM handles it?
            # "Execute imported module code only once" -> Runtime behavior.
            
            lexer = Lexer(source)
            tokens = lexer.tokenize()
            parser = Parser(tokens)
            statements = parser.parse()
            
            # Recursive load (pre-scan)
            # Strategy: We assume the Interpreter/VM will run these statements.
            # But the VM needs to resolve 'use' inside *these* statements too.
            # So, we should probably resolve them via a Visitor or just let the runtime handle it?
            # If we do it here, we return a flat list? No, scoping issues.
            # The prompt says "Execute imported module code only once".
            
            # Let's verify imports inside this module first (Static resolution)
            # Actually, if we just return AST, the runner will encounter 'use' nodes.
            # So we either expand them NOW or the Runner expands them.
            # Expanding NOW is safer for circular checks.
            
            expanded_stmts = []
            for stmt in statements:
                if isinstance(stmt, ast.UseDecl):
                    for sub_mod in stmt.modules:
                         # Recursively load
                         sub_stmts_ = self.load_module(sub_mod)
                         if sub_stmts_:
                             expanded_stmts.extend(sub_stmts_)
                else:
                    expanded_stmts.append(stmt)
                    
            self.loaded_modules.add(full_path)
            return expanded_stmts

        finally:
            self.processing_stack.pop()
