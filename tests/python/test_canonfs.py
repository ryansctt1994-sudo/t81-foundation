import sys
import os
import shutil

# Add build directory to path
sys.path.append(os.path.abspath("build"))

try:
    import t81_python
except ImportError:
    # try looking in lib folder inside build if cmake puts it there
    # On linux it might be directly in build or build/lib
    sys.path.append(os.path.abspath("build/lib"))
    try:
        import t81_python
    except ImportError:
        print("Could not import t81_python module. Make sure to build it first.")
        sys.exit(1)

def test_canonfs_in_memory():
    print("Testing In-Memory Driver...")
    driver = t81_python.make_in_memory_driver()

    data = b"Hello T81 CanonFS!"
    # ObjectType.RawBlock should be available
    ref = driver.write_object(t81_python.ObjectType.RawBlock, data)
    print(f"Written object, ref: {ref}")

    # Check hash size
    hash_bytes = ref.hash_bytes
    print(f"Hash bytes length: {len(hash_bytes)}")
    assert len(hash_bytes) == 32

    read_data = driver.read_object_bytes(ref)
    print(f"Read data: {read_data}")

    assert read_data == data

def test_canonfs_persistent():
    print("Testing Persistent Driver...")
    test_dir = "test_canon_store"
    if os.path.exists(test_dir):
        shutil.rmtree(test_dir)

    driver = t81_python.make_persistent_driver(test_dir)

    data = b"Persistent Data"
    ref = driver.write_object(t81_python.ObjectType.RawBlock, data)

    read_data = driver.read_object_bytes(ref)
    assert read_data == data

    # Re-open to verify persistence
    # Note: unique_ptr<Driver> ownership is transferred to python, so driver goes out of scope and is destroyed?
    # Actually python holds it. We create a new one.
    driver2 = t81_python.make_persistent_driver(test_dir)
    read_data2 = driver2.read_object_bytes(ref)
    assert read_data2 == data

    shutil.rmtree(test_dir)

if __name__ == "__main__":
    try:
        test_canonfs_in_memory()
        test_canonfs_persistent()
        print("CanonFS Python bindings tests passed!")
    except Exception as e:
        print(f"Test failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
