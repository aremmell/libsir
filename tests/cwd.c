/*
#if !defined(_WIN32)
#   include <unistd.h>
#else
#   include <direct.h>
#endif
bool get_working_directory(sirchar_t* dir, size_t size) {
#if !defined(_WIN32)
    static const sirchar_t slash = '/';
    if (NULL == getcwd(dir, size))
        return false;
#else
    static const sirchar_t slash = '\\';
    if (NULL == _getcwd(dir, (int)size))
        return false;
#endif

    size_t len = strnlen(dir, size);
    bool ends_with_slash = dir[len - 1] == slash;

    if (ends_with_slash)
        return true;

    if (len + 1 < size) {
        dir[len] = slash;
        dir[len + 1] = '\0';
        return true;
    }

    return false;

}
*/