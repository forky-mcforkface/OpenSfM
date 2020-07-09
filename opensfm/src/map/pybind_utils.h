#pragma once
#include <pybind11/pybind11.h>
#include <map/shot.h>
NAMESPACE_BEGIN(PYBIND11_NAMESPACE)
NAMESPACE_BEGIN(detail)
enum IteratorType
{
    // KeyIterator,
    ValueIterator,
    ItemIterator,
    UniquePtrValueIterator,
    UniquePtrIterator
};
template <typename Iterator, typename Sentinel, IteratorType it_type, return_value_policy Policy>
struct sfm_iterator_state {
    Iterator it;
    Sentinel end;
    bool first_or_done;
};
NAMESPACE_END(detail)

/// Makes an python iterator over the keys (`.first`) of a iterator over pairs from a
/// first and past-the-end InputIterator.
template <return_value_policy Policy = return_value_policy::reference_internal,
          typename Iterator,
          typename Sentinel,
          typename KeyType = decltype((*std::declval<Iterator>()).second),
          typename... Extra>
iterator make_value_iterator(Iterator first, Sentinel last, Extra &&... extra) {
    typedef detail::sfm_iterator_state<Iterator, Sentinel, detail::ValueIterator, Policy> state;

    if (!detail::get_type_info(typeid(state), false)) {
        class_<state>(handle(), "iterator", pybind11::module_local())
            .def("__iter__", [](state &s) -> state& { return s; })
            .def("__next__", [](state &s) -> KeyType {
                if (!s.first_or_done)
                    ++s.it;
                else
                    s.first_or_done = false;
                if (s.it == s.end) {
                    s.first_or_done = true;
                    throw stop_iteration();
                }
                return (*s.it).second;
            }, std::forward<Extra>(extra)..., Policy);
    }

    return cast(state{first, last, true});
}

template <return_value_policy Policy = return_value_policy::reference_internal,
          typename Iterator,
          typename Sentinel,
          typename KeyType = decltype(&((*std::declval<Iterator>()).second)),
          typename... Extra>
iterator make_ref_value_iterator(Iterator first, Sentinel last, Extra &&... extra) {
    typedef detail::sfm_iterator_state<Iterator, Sentinel, detail::ValueIterator, Policy> state;

    if (!detail::get_type_info(typeid(state), false)) {
        class_<state>(handle(), "ref_value_iterator", pybind11::module_local())
            .def("__iter__", [](state &s) -> state& { return s; })
            .def("__next__", [](state &s) -> KeyType {
                if (!s.first_or_done)
                    ++s.it;
                else
                    s.first_or_done = false;
                if (s.it == s.end) {
                    s.first_or_done = true;
                    throw stop_iteration();
                }
                return &(s.it->second);
            }, std::forward<Extra>(extra)..., Policy);
    }

    return cast(state{first, last, true});
}

template <return_value_policy Policy = return_value_policy::reference_internal,
          typename Iterator,
          typename Sentinel,
          typename ValueType = pybind11::tuple, 
          typename... Extra>
iterator make_unique_ptr_iterator(Iterator first, Sentinel last,
                                  Extra &&... extra) {
typedef detail::sfm_iterator_state<Iterator, Sentinel, detail::UniquePtrIterator, Policy> state;

  const std::type_index tp = typeid(state);
  if (!detail::get_type_info(typeid(state), false)) {
    class_<state>(handle(), "unique_ptr_iterator", pybind11::module_local())
        .def("__iter__", [](state &s) -> state & { return s; })
        .def("__next__",
            [](state &s) -> ValueType {
              if (!s.first_or_done)
                ++s.it;
              else
                s.first_or_done = false;
              if (s.it == s.end) {
                s.first_or_done = true;
                throw stop_iteration();
              }
              return pybind11::make_tuple(s.it->first, s.it->second.get());
            },
            std::forward<Extra>(extra)..., Policy);
  }

  return cast(state{first, last, true});
}

template <return_value_policy Policy = return_value_policy::reference_internal,
          typename Iterator,
          typename Sentinel,
          typename ValueType = decltype((*std::declval<Iterator>()).second.get()),
          typename... Extra>
iterator make_unique_ptr_value_iterator(Iterator first, Sentinel last, Extra &&... extra) {
    typedef detail::sfm_iterator_state<Iterator, Sentinel, detail::UniquePtrValueIterator, Policy> state;
    if (!detail::get_type_info(typeid(state), false)) {
        class_<state>(handle(), "unique_ptr_value_iterator", pybind11::module_local())
            .def("__iter__", [](state &s) -> state& { return s; })
            .def("__next__", [](state &s) -> ValueType {
                if (!s.first_or_done)
                    ++s.it;
                else
                    s.first_or_done = false;
                if (s.it == s.end) {
                    s.first_or_done = true;
                    throw stop_iteration();
                }
                return s.it->second.get();
            }, std::forward<Extra>(extra)..., Policy);
    }
    return cast(state{first, last, true});
}

// template <return_value_policy Policy = return_value_policy::reference_internal,
//           typename Iterator,
//           typename Sentinel,
//           typename ValueType = decltype(std::declval<Iterator>()->second),
//           typename... Extra>
// iterator make_ptr_value_iterator(Iterator first, Sentinel last, Extra &&... extra) {
//     typedef detail::sfm_iterator_state<Iterator, Sentinel, detail::UniquePtrValueIterator, Policy> state;
//     if (!detail::get_type_info(typeid(state), false)) {
//         class_<state>(handle(), "ptr_value_iterator", pybind11::module_local())
//             .def("__iter__", [](state &s) -> state& { return s; })
//             .def("__next__", [](state &s) -> ValueType {
//                 if (!s.first_or_done)
//                     ++s.it;
//                 else
//                     s.first_or_done = false;
//                 if (s.it == s.end) {
//                     s.first_or_done = true;
//                     throw stop_iteration();
//                 }
//                 return s.it->second;
//             }, std::forward<Extra>(extra)..., Policy);
//     }

//     return cast(state{first, last, true});
// }


/// Makes a python iterator from a first and past-the-end C++ InputIterator.
template <return_value_policy Policy = return_value_policy::reference_internal,
          typename Iterator,
          typename Sentinel,
          typename ValueType = decltype(std::declval<Iterator>()),
          typename... Extra>
iterator make_ptr_iterator(Iterator first, Sentinel last, Extra &&... extra) {
    typedef detail::iterator_state<Iterator, Sentinel, false, Policy> state;

    if (!detail::get_type_info(typeid(state), false)) {
        class_<state>(handle(), "iterator", pybind11::module_local())
            .def("__iter__", [](state &s) -> state& { return s; })
            .def("__next__", [](state &s) -> ValueType {
                if (!s.first_or_done)
                    ++s.it;
                else
                    s.first_or_done = false;
                if (s.it == s.end) {
                    s.first_or_done = true;
                    throw stop_iteration();
                }
                return s.it;
            }, std::forward<Extra>(extra)..., Policy);
    }

    return cast(state{first, last, true});
}



/// Makes an iterator over the keys (`.first`) of a stl map-like container supporting
/// `std::begin()`/`std::end()`
template <return_value_policy Policy = return_value_policy::reference_internal,
          typename Type, typename... Extra> iterator make_value_iterator(Type &value, Extra&&... extra) {
    return make_value_iterator<Policy>(std::begin(value), std::end(value), extra...);
}
template <return_value_policy Policy = return_value_policy::reference_internal,
          typename Type, typename... Extra> iterator make_unique_ptr_value_iterator(Type &value, Extra&&... extra) {
    return make_unique_ptr_value_iterator<Policy>(std::begin(value), std::end(value), extra...);
}
template <return_value_policy Policy = return_value_policy::reference_internal,
          typename Type, typename... Extra> iterator make_unique_ptr_iterator(Type &value, Extra&&... extra) {
    return make_unique_ptr_iterator<Policy>(std::begin(value), std::end(value), extra...);
}

template <return_value_policy Policy = return_value_policy::reference_internal,
          typename Type, typename... Extra> iterator make_ref_value_iterator(Type &value, Extra&&... extra) {
    return make_ref_value_iterator<Policy>(std::begin(value), std::end(value), extra...);
}
NAMESPACE_END(PYBIND11_NAMESPACE)
