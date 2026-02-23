import sys
import os

# Add build directory to path
sys.path.append(os.path.abspath("build"))

import t81_python

def test_t81int():
    a = t81_python.T81Int(42)
    b = t81_python.T81Int(10)
    c = a + b
    print(f"42 + 10 = {c}")
    # Repr test
    print(f"Repr: {repr(a)}")

def test_t729tensor():
    t = t81_python.T729DynamicTensor([2, 2], [1.0, 2.0, 3.0, 4.0])
    print(f"Tensor shape: {t.shape}")
    print(f"Tensor data: {t.data}")
    assert t.shape == [2, 2]
    assert t.data == [1.0, 2.0, 3.0, 4.0]

def test_vm():
    source = """
    fn main() -> i32 {
        var x = 10;
        var y = 20;
        return x + y;
    }
    """
    # Use the new compile_and_run helper
    vm_temp = t81_python.make_interpreter_vm()
    program = t81_python.compile(source)
    vm_temp.load_program(program)
    vm_temp.run_to_halt()
    result = vm_temp.get_register(0)
    print(f"VM result: {result}")
    print("Trace:")
    for t in vm_temp.trace:
        print(f"  {t}")

    assert result == 30

    # Test explicit VM usage and register mutation
    program = t81_python.compile(source)
    vm = t81_python.make_interpreter_vm()
    vm.load_program(program)

    # Mutate register before running (though it will be overwritten by the program)
    vm.set_register(1, 100)
    print(f"R1 before run: {vm.get_register(1)}")
    assert vm.get_register(1) == 100

    vm.run_to_halt()
    print(f"R0 after run: {vm.get_register(0)}")
    assert vm.get_register(0) == 30

    # Check axion log for mutation
    print("Axion Log:")
    for entry in vm.axion_log:
        print(f"  {entry}")

    mutation_found = any("register mutation R1" in entry for entry in vm.axion_log)
    assert mutation_found

if __name__ == "__main__":
    test_t81int()
    test_t729tensor()
    test_vm()
    print("All Python binding tests passed!")
