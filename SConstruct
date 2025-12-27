#!/usr/bin/env python
import os
import sys

from methods import print_error


libname = "OpenCV-GDExtension"
projectdir = "demo"

localEnv = Environment(tools=["default"], PLATFORM="")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})



# OPENCV Stuff start

# set this path to your library files. This is the location of dll, dylib and so files.
opencv_library_path = [
    "opencv/build/install/x64/vc17/lib",
    #'YourCustomOpencvFolder/build/x64/vc16/lib'
]

# tweak this if you want to use different folders, or more folders, to store your source code in.

opencv_header_files = [
    "src/",
    "opencv/build/install/include",
    #"YourCustomOpencvFolder/build/include"
]

opencv_library_files = {
    'windows': [
        'opencv_world4120.lib',
#        'opencv_world4120d.lib'
    ],
    'macos': [
        'libopencv_core.dylib',
        'libopencv_imgcodecs.dylib',
        'libopencv_imgproc.dylib',
        'libopencv_videoio.dylib',
        'libopencv_objdetect.dylib',
        'libopencv_video.dylib',
        'libopencv_tracking.dylib'
    ],
    'linux': [
        'libopencv_core.so',
        'libopencv_imgcodecs.so',
        'libopencv_imgproc.so',
        'libopencv_videoio.so',
        'libopencv_objdetect.so',
        'libopencv_video.so',
        'libopencv_tracking.so'
    ]
}

env.Append(CPPPATH=opencv_header_files)
env.Append(LIBPATH=opencv_library_path)
env.Append(LIBS=opencv_library_files[env["platform"]])

# OPENCV Stuff end


env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/bin/{}/".format(projectdir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)
