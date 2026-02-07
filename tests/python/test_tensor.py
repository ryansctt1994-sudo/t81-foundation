import sys
import os
sys.path.append(os.path.join(os.getcwd(), 'build'))

try:
    import t81_python
    print("Successfully imported t81_python")

    val1 = t81_python.T81Int(10)
    val2 = t81_python.T81Int(20)
    res = val1 + val2
    print(f"10 + 20 = {res}")

    t1 = t81_python.Tensor1D3(t81_python.T81Int(5))
    print(f"Tensor1D3 size: {t1.size()}")
    print(f"Tensor1D3[0]: {t1[0]}")

    t2 = t81_python.Tensor2D33(t81_python.T81Int(1))
    t2[1, 1] = t81_python.T81Int(42)
    print(f"Tensor2D33[1,1]: {t2[1,1]}")

    t3 = t2 + t2
    print(f"Tensor2D33[1,1] + [1,1]: {t3[1,1]}")

    print("Python bindings verification successful!")
except Exception as e:
    import traceback
    traceback.print_exc()
    print(f"Verification failed: {e}")
    sys.exit(1)
