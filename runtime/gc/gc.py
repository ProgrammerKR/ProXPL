from runtime.memory.memory_manager import MemoryManager, ProXObject
from typing import List, Set

class GarbageCollector:
    def __init__(self, memory_manager: MemoryManager):
        self.mm = memory_manager

    def collect_garbage(self, roots: List[ProXObject]):
        """
        Performs a Mark-and-Sweep GC cycle.
        
        Args:
            roots: List of objects directly accessible (stack, globals).
        """
        print("[GC] Starting collection...")
        initial_count = len(self.mm.heap)
        
        # 1. MARK
        self.mark_from_roots(roots)
        
        # 2. SWEEP
        self.sweep()
        
        final_count = len(self.mm.heap)
        freed = initial_count - final_count
        print(f"[GC] Finished. Freed {freed} objects. Heap: {final_count} active objects.")

    def mark_from_roots(self, roots: List[ProXObject]):
        # Reset marks is technically not needed if we sweep correctly, 
        # but let's ensure fresh start or unmark at end of sweep.
        # Actually in mark-sweep, 'marked' is temporary for the cycle.
        
        # Unmark all first (inefficient but simple for simulation)
        for obj in self.mm.get_all_objects():
            obj.marked = False

        worklist = list(roots)
        while worklist:
            obj = worklist.pop()
            if obj and not obj.marked:
                obj.marked = True
                # Add children to worklist
                # For this simplified model, we assume 'value' might contain other ProXObjects
                # e.g. if it's a list or dict or class instance
                self.add_children(obj, worklist)

    def add_children(self, obj: ProXObject, worklist: List[ProXObject]):
        if isinstance(obj.value, list):
            for item in obj.value:
                if isinstance(item, ProXObject):
                    worklist.append(item)
        elif isinstance(obj.value, dict):
            for key, val in obj.value.items():
                if isinstance(key, ProXObject): worklist.append(key)
                if isinstance(val, ProXObject): worklist.append(val)
        # Add usage for class instances etc.

    def sweep(self):
        # Collect addresses to free
        to_free = []
        for address, obj in self.mm.heap.items():
            if not obj.marked:
                to_free.append(address)
        
        for addr in to_free:
            self.mm.free(addr)
