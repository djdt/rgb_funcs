#!/usr/bin/env python
#
# Last modified 2015-01-12

import os
import re
import subprocess
import ycm_core


language = 'c++'
standard = 'c++1z'
packages = ['gl']
flags = [
    '-Wall',
    '-Wextra',
    '-Werror',
    '-fexceptions',
    '-DNDEBUG',
    '-std=' + standard,
    '-x', language,
    '-I', '.',
    '-I', 'include',
]


def LoadSystemIncludes(language):
    regex = re.compile(
        (r'(?:\#include \<...\> search starts here\:)'
            r'(?P<list>.*?)(?:End of search list)'), re.DOTALL)
    process = subprocess.Popen(
        ['clang', '-v', '-E', '-x', language, '-'],
        universal_newlines=True, stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    process_out, process_err = process.communicate('')
    output = process_out + process_err
    includes = []
    for p in re.search(regex, str(output)).group('list').split('\n'):
        p = p.strip()
        if len(p) > 0 and p.find('(framework directory)') < 0:
            includes.append('-isystem')
            includes.append(p)
    return includes


def LoadPackageIncludes(packages):
    cmd = 'pkg-config --cflags ' + ' '.join(packages)
    libs = subprocess.getoutput(cmd).split()
    includes = []
    for lib in libs:
        if lib.startswith('-I'):
            includes.append('-isystem')
            includes.append(lib.replace('-I', '', 1))
    return includes


flags = (flags + LoadSystemIncludes(language)
               + LoadPackageIncludes(packages))

compilation_database_folder = ''

if os.path.exists(compilation_database_folder):
    database = ycm_core.CompilationDatabase(compilation_database_folder)
else:
    database = None

SOURCE_EXTENSIONS = ['.cpp', '.cxx', '.cc', '.c', '.m', '.mm']


def DirectoryOfThisScript():
    return os.path.dirname(os.path.abspath(__file__))


def MakeRelativePathsInFlagsAbsolute(flags, working_directory):
    if not working_directory:
        return list(flags)
    new_flags = []
    make_next_absolute = False
    path_flags = ['-isystem', '-I', '-iquote', '--sysroot=']
    for flag in flags:
        new_flag = flag

        if make_next_absolute:
            make_next_absolute = False
            if not flag.startswith('/'):
                new_flag = os.path.join(working_directory, flag)

        for path_flag in path_flags:
            if flag == path_flag:
                make_next_absolute = True
                break

            if flag.startswith(path_flag):
                path = flag[len(path_flag):]
                new_flag = path_flag + os.path.join(working_directory, path)
                break

        if new_flag:
            new_flags.append(new_flag)
    return new_flags


def IsHeaderFile(filename):
    extension = os.path.splitext(filename)[1]
    return extension in ['.h', '.hxx', '.hpp', '.hh']


def GetCompilationInfoForFile(filename):
    if IsHeaderFile(filename):
        basename = os.path.splitext(filename)[0]
        for extension in SOURCE_EXTENSIONS:
            replacement_file = basename + extension
            if os.path.exists(replacement_file):
                compilation_info = database.GetCompilationInfoForFile(
                    replacement_file)
                if compilation_info.compiler_flags_:
                    return compilation_info
        return None
    return database.GetCompilationInfoForFile(filename)


def FlagsForFile(filename, **kwargs):
    if database:
        # Bear in mind that compilation_info.compiler_flags_ does NOT return a
        # python list, but a "list-like" StringVec object
        compilation_info = GetCompilationInfoForFile(filename)
        if not compilation_info:
            return None

        final_flags = MakeRelativePathsInFlagsAbsolute(
            compilation_info.compiler_flags_,
            compilation_info.compiler_working_dir_)
    else:
        relative_to = DirectoryOfThisScript()
        final_flags = MakeRelativePathsInFlagsAbsolute(flags, relative_to)

    return {
        'flags': final_flags,
        'do_cache': True
    }
