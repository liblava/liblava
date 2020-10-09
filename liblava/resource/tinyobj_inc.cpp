// This is the central include for the tinyobjloader IMPLEMENTATION
// to not have multiple includes of the library

#ifdef _WIN32
#    pragma warning(push, 4)
#else
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#ifdef _WIN32
#    pragma warning(pop)
#else
#    pragma GCC diagnostic pop
#endif
