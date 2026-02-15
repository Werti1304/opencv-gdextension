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

# WEB Build Exclusive Start

if(env['platform'] == "web"):
    env.Append(CCFLAGS=["-msimd128"])
    env.Append(LINKFLAGS=["-msimd128"])

# WEB Build End


# OPENCV Stuff start

env.Append(CPPPATH=["src/", "include/"])
env.Append(LIBS=["opencv_world4130"])
env.Append(LIBPATH=["lib/opencv"])

# OPENCV Stuff end


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


# Copy opencv dll
if(env['platform'] == "web"):
    opencv_dll = "libopencv_world4130.a"
elif env["target"] in ["editor", "template_debug"]:
    opencv_dll = "opencv_world4130.dll" # We only use release dll from opencv
else:
    opencv_dll = "opencv_world4130.dll"

opencv_bin_dir = "lib/opencv/"
opencv_dll_path = os.path.join(opencv_bin_dir, opencv_dll)
opencv_copy = env.Install(
    "{}/bin/{}/".format(projectdir, env["platform"]),
    opencv_dll_path
)

default_args = [library, copy, opencv_copy]
Default(*default_args)
