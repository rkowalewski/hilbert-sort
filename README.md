# Hilbert Space Filling Curves

Prototypical implementation of a Hilbert Sort algorithm. The basic approach is 
to convert a n-dimensional point into 1-dimensional Hilbert space-filling 
curve. Passing this algorithm as a lambda to `std::sort` allows to sort 
n-dimensional points in a C++ container. This is useful in many scientific 
applications such as N-Body problems.

## Limitations

Currently only up to 3 dimensions are supported.

# External dependencies

- [libmorton](https://github.com/Forceflow/libmorton)
