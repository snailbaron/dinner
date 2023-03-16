#include "fs.hpp"

#include "error.hpp"

#include <concepts>
#include <utility>

#ifdef __linux__
#elif defined(_WIN32)
#include <ShlObj.h>
#endif

namespace fs = std::filesystem;

namespace paths {

namespace {

template <std::invocable F>
class Defer {
public:
    Defer(F&& action) : _action(std::forward<F>(action)) {}
    Defer(const Defer&) = delete;
    Defer(Defer&&) = delete;
    ~Defer() { _action(); }

private:
    F _action;
};

} // namespace

fs::path userConfigPath()
{
#ifdef __linux__
#elif defined(_WIN32)
    PWSTR localAppData;
    HRESULT result = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppData);
    const auto freePathString = Defer{[&localAppData] {
        CoTaskMemFree(localAppData);
    }};
    if (result != S_OK) {
        throw Error{} << "failed to get path to local AppData";
    }

    return fs::path{localAppData} / "Dinner" / "config.yaml";
#endif
}

fs::path globalConfigPath()
{
#ifdef __linux__
#elif defined(_WIN32)
    PWSTR programData;
    HRESULT result = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &programData);
    const auto freePathString = Defer{ [&programData] {
        CoTaskMemFree(programData);
    }};
    if (result != S_OK) {
        throw Error{} << "failed to get path to ProgramData";
    }

    return fs::path{programData} / "Dinner" / "config.yaml";
#endif
}

} // namespace paths
