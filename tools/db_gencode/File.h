#ifndef _FILE_H_
#define _FILE_H_

#include <common.h>

class File
{
public:
    // Check if the file exists.
    static bool Exists(const string& name);

    // Read an entire file to a string.  Return true if successful, false
    // otherwise.
    static int ReadFileToString(const string& name, string* output);

    // Same as above, but crash on failure.
    static void ReadFileToStringOrDie(const string& name, string* output);

    // Create a file and write a string to it.
    static int WriteStringToFile(const string& contents, const string& name);

    // Same as above, but crash on failure.
    static void WriteStringToFileOrDie(const string& contents, const string& name);

    // Create a directory.
    static bool CreateDir(const string& name, int mode);

    // Create a directory and all parent directories if necessary.
    static bool RecursivelyCreateDir(const string& path, int mode);

    // If "name" is a file, we delete it.  If it is a directory, we
    // call DeleteRecursively() for each file or directory (other than
    // dot and double-dot) within it, and then delete the directory itself.
    // The "dummy" parameters have a meaning in the original version of this
    // method but they are not used anywhere in protocol buffers.
    static void DeleteRecursively(const string& name, void* dummy1, void* dummy2);

    static bool GetContents(const string& name, string* output)
    {
        return ReadFileToString(name, output);
    }

    static bool SetContents(const string& name, const string& contents)
    {
        return WriteStringToFile(contents, name);
    }
};

#endif