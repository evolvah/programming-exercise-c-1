#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// tested on OS X
#if defined(__APPLE__)
#include <sys/syslimits.h>
#elif defined(__linux__)
#include <linux/limits.h>
#else
#error Unsupported platform
#endif

typedef unsigned int uint;          // shorthand

char* normalize(const char* path) {
    char* output = (char*)malloc(PATH_MAX);  // use platform-specific max path length

    uint    readPos = 0;            // read position in the input string
    uint    writePos = 0;           // write position in the output string

    output[writePos++] = '/';       // put the leading slash in

    // loop until we are done going through the input string
    while (path[readPos] != 0) {
        if (output[writePos-1] != '/') output[writePos++] = '/';
        // skip slashes. it will also bail at the end of string
        while (path[readPos] == '/') readPos++;
        // be optimistic and copy the next token
        while (path[readPos] != '/' && path[readPos] != 0) output[writePos++] = path[readPos++];
        // rewind if we just copied a "."
        if (writePos > 1 && output[writePos-1] == '.' && output[writePos-2] == '/') writePos -= 1;
        // rewind twice if we just copied a ".."
        if (writePos > 2 && output[writePos-1] == '.' && output[writePos-2] == '.' && output[writePos-3] == '/') {
            writePos -= 3;
            // don't eat the leading slash when popping
            if (writePos == 0) writePos = 1;
            while ((writePos > 0) && (output[writePos-1] != '/')) writePos--;
        }
    }
    // corner case if we rewound all the way up, in which case we should return "/"
    if (writePos == 0) output[writePos++] = '/';
    // terminate the string
    output[writePos] = 0;

    return output;
}

int main(int argc, char const *argv[]) {
    // original tests
    assert(strcmp(normalize("../bar"), "/bar") == 0);
    assert(strcmp(normalize("/foo/bar"), "/foo/bar") == 0);
    assert(strcmp(normalize("/foo/bar/../baz"), "/foo/baz") == 0);
    assert(strcmp(normalize("/foo/bar/./baz/"), "/foo/bar/baz/") == 0);
    assert(strcmp(normalize("/foo/../../baz"), "/baz") == 0);

    // kika's tests
    assert(strcmp(normalize("/foo/../../baz.txt/foobar..doc"), "/baz.txt/foobar..doc") == 0);
    assert(strcmp(normalize("/a/b/../c"), "/a/c") == 0);
    assert(strcmp(normalize("/a/b/./c"), "/a/b/c") == 0);
    assert(strcmp(normalize("/foo/bar/./xyz/"), "/foo/bar/xyz/") == 0);
    assert(strcmp(normalize("/foo/bar/../baz"), "/foo/baz") == 0);
    assert(strcmp(normalize("../zyx/../foo/./bar/../baz"), "/foo/baz") == 0);
    assert(strcmp(normalize("/foo/bar"), "/foo/bar") == 0);
    assert(strcmp(normalize("../bar"), "/bar") == 0);
    assert(strcmp(normalize("/../bar"), "/bar") == 0);
    assert(strcmp(normalize("./bar"), "/bar") == 0);
 
    // additional tests
    assert(strcmp(normalize("a/b/c"), "/a/b/c") == 0);
    assert(strcmp(normalize("a/b/c/.."), "/a/b/") == 0);
    assert(strcmp(normalize("a/b../c"), "/a/b../c") == 0);
    assert(strcmp(normalize("a/b/../c"), "/a/c") == 0);
    assert(strcmp(normalize("a/b/../c/./"), "/a/c/") == 0);
    assert(strcmp(normalize("a/b/../c/."), "/a/c/") == 0);
    assert(strcmp(normalize("/foo/.../bar/blah.txt"), "/foo/.../bar/blah.txt") == 0);
    assert(strcmp(normalize("/foo/bar/."), "/foo/bar/") == 0);
    assert(strcmp(normalize("/foo/bar/.."), "/foo/") == 0);
    assert(strcmp(normalize("/foo/bar/../"), "/foo/") == 0);
    assert(strcmp(normalize("////foo////bar///..////."), "/foo/") == 0);
    assert(strcmp(normalize(""), "/") == 0);

    printf("All tests passed!\n");

    return 0;
}
