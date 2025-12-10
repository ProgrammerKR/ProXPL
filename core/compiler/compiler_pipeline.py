from core.lexer.lexer import Lexer
from core.parser.parser import Parser
from core.semantic.type_checker import TypeChecker
from core.ir.ir_format import IRModule, IRFunction, Instruction, OpCode
from core.optimizer.optimizer import Optimizer
from core.vm.vm import VM
from core.runtime.importer import Importer
import time
from core.ast import ast_nodes as ast

class CompilerPipeline:
    def __init__(self):
        self.optimizer = Optimizer()
        self.vm = VM()
        self.importer = Importer()

    def compile(self, source_code: str) -> IRModule:
        print("[1/5] Tokenizing...")
        start = time.time()
        lexer = Lexer(source_code)
        tokens = lexer.tokenize()
        print(f"      Tokens: {len(tokens)} ({time.time() - start:.4f}s)")

        print("[2/5] Parsing...")
        start = time.time()
        parser = Parser(tokens)
        ast_root = parser.parse()
        print(f"      AST Nodes: {len(ast_root)} ({time.time() - start:.4f}s)")

        print("[2.5/5] resolving Modules...")
        start = time.time()
        # Resolve imports in the main AST
        # We need to manually expand the main AST imports because Importer.load_module expects a name.
        # But here we have the code already.
        # Let's add a helper to expand existing AST.
        final_ast = self.expand_imports(ast_root)
        print(f"      Total Nodes after expansion: {len(final_ast)} ({time.time() - start:.4f}s)")

        print("[3/5] Semantic Analysis...")
        start = time.time()
        checker = TypeChecker()
        errors = checker.check(final_ast)
        if errors:
            for e in errors:
                print(f"      Error: {e}")
            raise Exception("Compilation failed due to semantic errors.")
        print(f"      Analysis passed ({time.time() - start:.4f}s)")

        print("[4/5] Generating IR...")
        start = time.time()
        # Simplified AST -> IR generation
        # In a real impl, this would be a Visitor on the AST
        ir_module = self.generate_ir_dummy(final_ast) # Placeholder
        print(f"      IR Generated ({time.time() - start:.4f}s)")

        print("[5/5] Optimizing...")
        start = time.time()
        ir_module = self.optimizer.optimize(ir_module)
        print(f"      Optimization complete ({time.time() - start:.4f}s)")

        return ir_module

    def expand_imports(self, statements: list) -> list:
        expanded = []
        for stmt in statements:
            if isinstance(stmt, ast.UseDecl):
                for module in stmt.modules:
                    mod_stmts = self.importer.load_module(module)
                    if mod_stmts:
                        expanded.extend(mod_stmts)
            else:
                expanded.append(stmt)
        return expanded

    def run(self, source_code: str):
        try:
            ir_module = self.compile(source_code)
            print("\n[Running Native VM]...")
            self.vm.run(ir_module)
        except Exception as e:
            print(f"Pipeline Error: {e}")

    def generate_ir_dummy(self, ast_nodes) -> IRModule:
        # Just create a dummy IR for the "run" command to prove flow
        module = IRModule()
        main = IRFunction("main")
        block = main.new_block("entry")
        
        # Add a test print instruction
        # Ideally we walk the AST here
        block.add(Instruction(OpCode.CALL, arg2="print", arg3=["[System] ProXPL Pipeline is working!"]))
        block.add(Instruction(OpCode.RETURN, arg1=0))
        
        module.add_function(main)
        return module
