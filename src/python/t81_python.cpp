#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "t81/core/T81Int.hpp"
#include "t81/core/T81Tensor.hpp"
#include <string>

namespace py = pybind11;
using namespace t81;

PYBIND11_MODULE(t81_python, m) {
    m.doc() = "T81 Foundation Python Bindings";

    // Bind T81Int<81>
    py::class_<T81Int<81>>(m, "T81Int")
        .def(py::init<int64_t>())
        .def("__add__", [](const T81Int<81>& a, const T81Int<81>& b) { return a + b; })
        .def("__sub__", [](const T81Int<81>& a, const T81Int<81>& b) { return a - b; })
        .def("__mul__", [](const T81Int<81>& a, const T81Int<81>& b) { return a * b; })
        .def("__repr__", [](const T81Int<81>& a) -> std::string {
            try {
                return std::string("<t81.T81Int value=") + std::to_string(a.to_int64()) + ">";
            } catch (...) {
                return std::string("<t81.T81Int value=(large)>");
            }
        })
        .def_static("max_value", []() { return T81Int<81>::kMaxValue; })
        .def_static("min_value", []() { return T81Int<81>::kMinValue; });

    // Bind T81Tensor<T81Int<81>, 1, 3>
    using Tensor1D3 = T81Tensor<T81Int<81>, 1, 3>;
    py::class_<Tensor1D3>(m, "Tensor1D3")
        .def(py::init<>())
        .def(py::init<T81Int<81>>())
        .def("__getitem__", [](Tensor1D3& t, size_t i) -> T81Int<81>& {
            if (i >= 3) throw py::index_error();
            return t(i);
        }, py::return_value_policy::reference_internal)
        .def("__setitem__", [](Tensor1D3& t, size_t i, const T81Int<81>& v) {
            if (i >= 3) throw py::index_error();
            t(i) = v;
        })
        .def("size", [](const Tensor1D3&) { return Tensor1D3::size(); })
        .def("rank", [](const Tensor1D3&) { return Tensor1D3::rank(); })
        .def("__add__", [](const Tensor1D3& a, const Tensor1D3& b) { return a + b; });

    // Bind T81Tensor<T81Int<81>, 2, 3, 3>
    using Tensor2D33 = T81Tensor<T81Int<81>, 2, 3, 3>;
    py::class_<Tensor2D33>(m, "Tensor2D33")
        .def(py::init<>())
        .def(py::init<T81Int<81>>())
        .def("__getitem__", [](Tensor2D33& t, py::tuple idx) -> T81Int<81>& {
            if (idx.size() != 2) throw py::value_error("Index must be a 2-tuple");
            size_t i = idx[0].cast<size_t>();
            size_t j = idx[1].cast<size_t>();
            if (i >= 3 || j >= 3) throw py::index_error();
            return t(i, j);
        }, py::return_value_policy::reference_internal)
        .def("__setitem__", [](Tensor2D33& t, py::tuple idx, const T81Int<81>& v) {
            if (idx.size() != 2) throw py::value_error("Index must be a 2-tuple");
            size_t i = idx[0].cast<size_t>();
            size_t j = idx[1].cast<size_t>();
            if (i >= 3 || j >= 3) throw py::index_error();
            t(i, j) = v;
        })
        .def("size", [](const Tensor2D33&) { return Tensor2D33::size(); })
        .def("rank", [](const Tensor2D33&) { return Tensor2D33::rank(); })
        .def("__add__", [](const Tensor2D33& a, const Tensor2D33& b) { return a + b; });
}
