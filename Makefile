CXX       ?= g++
CXXFLAGS  ?= -std=c++20 -O2 -Wall -Wextra
INCLUDES  := -Iinclude -Isrc
CC		  ?= cc
BUILD_DIR ?= build

# Default target (legacy ad-hoc example/test binaries)
.PHONY: all
all: examples tests

# CMake-first developer workflow (authoritative)
.PHONY: cmake-configure cmake-build cmake-test cmake-ritual
cmake-configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release

cmake-build:
	cmake --build $(BUILD_DIR) --parallel 1

cmake-test:
	ctest --test-dir $(BUILD_DIR) --output-on-failure -j1

cmake-ritual: cmake-configure cmake-build cmake-test

# -------- Core Sources --------
CORE_SRCS := \
  src/core/bigint.cpp \
  src/core/fraction.cpp \
  src/tisc/encoding.cpp \
  src/vm/vm.cpp \
  src/canonfs/in_memory_driver.cpp \
  src/canonfs/persistent_driver.cpp \
  src/canonfs/axion_hook.cpp \
  src/hanoi/error.cpp \
  src/hanoi/in_memory_kernel.cpp \
  src/axion/axion_api.cpp \
  src/axion/engine.cpp \
  src/axion/ethics.cpp \
  src/axion/policy_engine.cpp \
  src/axion/policy_serialization.cpp \
  src/tools/weights.cpp \
  src/tools/weights_t81w_emitter.cpp \
  src/cog/promotion.cpp \
  src/cog/tier1/symbolic.cpp \
  src/cog/tier2/reflective.cpp \
  src/cog/tier3/recursive.cpp \
  src/cog/tier4/tier4_loop.cpp \
  src/cog/tier4/distributed.cpp \
  src/cog/tier5/infinite.cpp \
  src/codec/base243.cpp \
  src/codec/base81.cpp \
  src/codec/base81_balanced.cpp \
  src/codec/base81_packed.cpp \
  src/codec/metadata.cpp \
  src/codec/trit_packing.cpp \
  src/vm/jit_compiler.cpp \
  src/hash/canonhash81.cpp \
  src/crypto/sha3.cpp \
  src/setun/bridge.cpp

IO_SRCS := src/io/tensor_loader.cpp

# -------- Examples --------
EXAMPLES := \
	build/t81_demo \
	build/t81_tensor_ops \
	build/t81_ir_roundtrip \
	build/axion_demo

examples: $(EXAMPLES)

build/t81_demo: examples/demo.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/demo.cpp $(CORE_SRCS) -o $@

build/t81_tensor_ops: examples/tensor_ops.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/tensor_ops.cpp $(CORE_SRCS) -o $@

build/t81_ir_roundtrip: examples/ir_roundtrip.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/ir_roundtrip.cpp $(CORE_SRCS) -o $@

build/axion_demo: examples/axion_demo.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/axion_demo.cpp $(CORE_SRCS) -o $@

build/t81_tensor_unary_test: tests/cpp/tensor_unary_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/tensor_unary_test.cpp $(CORE_SRCS) -o $@

# -------- Tests --------
TESTS := \
	build/t81_bigint_test \
	build/t81_fraction_test \
	build/t81_tensor_transpose_test \
	build/t81_tensor_slice_test \
	build/t81_tensor_reshape_test \
	build/t81_tensor_loader_test \
	build/t81_canonfs_io_test \
	build/t81_ir_encoding_test \
	build/t81_hash_stub_test \
	build/t81_axion_stub_test \
	build/t81_codec_base243_test \
	build/t81_tensor_shape_test \
	build/t81_ternary_arith_test \
	build/t81_tensor_matmul_test \
	build/t81_tensor_reduce_test \
	build/t81_tensor_broadcast_test \
	build/t81_entropy_test \
	build/t81_c_api_bigint_test \
	build/t81_tensor_unary_test \
	build/t81_ethics_test \
	build/t81_infinite_test \
	build/t81_property_invariants_test \
	build/t81_property_float_test

tests: $(TESTS)

build/t81_property_invariants_test: tests/cpp/test_property_invariants.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/test_property_invariants.cpp $(CORE_SRCS) -o $@

build/t81_property_float_test: tests/cpp/test_property_float.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/test_property_float.cpp $(CORE_SRCS) -o $@

build/t81_bigint_test: tests/cpp/bigint_roundtrip.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/bigint_roundtrip.cpp $(CORE_SRCS) -o $@

build/t81_fraction_test: tests/cpp/fraction_roundtrip.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/fraction_roundtrip.cpp $(CORE_SRCS) -o $@

build/t81_tensor_transpose_test: tests/cpp/tensor_transpose_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/tensor_transpose_test.cpp $(CORE_SRCS) -o $@

build/t81_tensor_slice_test: tests/cpp/tensor_slice_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/tensor_slice_test.cpp $(CORE_SRCS) -o $@

build/t81_tensor_reshape_test: tests/cpp/tensor_reshape_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/tensor_reshape_test.cpp $(CORE_SRCS) -o $@

build/t81_tensor_loader_test: tests/cpp/tensor_loader_test.cpp $(IO_SRCS) $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/tensor_loader_test.cpp $(IO_SRCS) $(CORE_SRCS) -o $@

build/t81_canonfs_io_test: tests/cpp/canonfs_io_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/canonfs_io_test.cpp $(CORE_SRCS) -o $@

build/t81_ir_encoding_test: tests/cpp/ir_encoding_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/ir_encoding_test.cpp $(CORE_SRCS) -o $@

build/t81_hash_stub_test: tests/cpp/hash_stub_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/hash_stub_test.cpp $(CORE_SRCS) -o $@

build/t81_axion_stub_test: tests/cpp/axion_stub_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/axion_stub_test.cpp $(CORE_SRCS) -o $@

build/t81_codec_base243_test: tests/cpp/codec_base243_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/codec_base243_test.cpp $(CORE_SRCS) -o $@

build/t81_tensor_shape_test: tests/cpp/tensor_shape_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/tensor_shape_test.cpp $(CORE_SRCS) -o $@

build/t81_ternary_arith_test: tests/cpp/ternary_arith_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/ternary_arith_test.cpp $(CORE_SRCS) -o $@

build/t81_tensor_matmul_test: tests/cpp/tensor_matmul_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/tensor_matmul_test.cpp $(CORE_SRCS) -o $@

build/t81_tensor_reduce_test: tests/cpp/tensor_reduce_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/tensor_reduce_test.cpp $(CORE_SRCS) -o $@

build/t81_tensor_broadcast_test: tests/cpp/tensor_broadcast_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/tensor_broadcast_test.cpp $(CORE_SRCS) -o $@

build/t81_entropy_test: tests/cpp/entropy_test.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/entropy_test.cpp $(CORE_SRCS) -o $@

build/t81_ethics_test: tests/cpp/test_ethics.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/test_ethics.cpp $(CORE_SRCS) -o $@

build/t81_infinite_test: tests/cpp/test_infinite_opcodes.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/cpp/test_infinite_opcodes.cpp $(CORE_SRCS) -o $@

# C API object (C++)
build/t81_c_api.o: src/c_api/t81_c_api.cpp $(CORE_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# C API bigint test (compile C + link with C++)
build/t81_c_api_bigint_test: tests/cpp/c_api_bigint_test.c build/t81_c_api.o $(CORE_SRCS)
	@mkdir -p build
	$(CC)  $(INCLUDES) -c tests/cpp/c_api_bigint_test.c -o build/c_api_bigint_test.o
	$(CXX) $(CXXFLAGS) build/c_api_bigint_test.o build/t81_c_api.o $(CORE_SRCS) -o $@

# Run all tests
.PHONY: run-tests
run-tests: tests
	@./build/t81_bigint_test || exit 1
	@./build/t81_fraction_test || exit 1
	@./build/t81_tensor_transpose_test || exit 1
	@./build/t81_tensor_slice_test || exit 1
	@./build/t81_tensor_reshape_test || exit 1
	@./build/t81_tensor_loader_test || exit 1
	@./build/t81_canonfs_io_test || exit 1
	@./build/t81_ir_encoding_test || exit 1
	@./build/t81_hash_stub_test || exit 1
	@./build/t81_axion_stub_test || exit 1
	@./build/t81_codec_base243_test || exit 1
	@./build/t81_tensor_shape_test || exit 1
	@./build/t81_ternary_arith_test || exit 1
	@./build/t81_tensor_matmul_test || exit 1
	@./build/t81_tensor_reduce_test || exit 1
	@./build/t81_tensor_broadcast_test || exit 1
	@./build/t81_entropy_test || exit 1
	@./build/t81_c_api_bigint_test || exit 1
	@./build/t81_ethics_test || exit 1
	@./build/t81_infinite_test || exit 1
	@./build/t81_property_invariants_test || exit 1
	@./build/t81_property_float_test || exit 1
	@echo "All tests passed."

# Convenience
.PHONY: run-examples
run-examples: examples
	@./build/t81_demo
	@./build/t81_tensor_ops
	@./build/t81_ir_roundtrip
	@./build/axion_demo

.PHONY: audit-governance
audit-governance:
	@./scripts/ci/run_workflow_audits.sh

# Clean
.PHONY: clean
clean:
	@rm -rf build demo
