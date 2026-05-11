const std = @import("std");
const zcc = @import("compile_commands");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
        .link_libcpp = true,
    });

    exe_mod.addCSourceFiles(.{
        .files = &.{ "main.cpp", "Application.cpp", "FileLayer.cpp" },
        .flags = &.{ "-std=c++20", "-Wno-reorder", "-g" },
        .language = .cpp,
        .root = b.path("src/"),
    });

    exe_mod.addIncludePath(b.path("third_party/clay/include/"));
    exe_mod.addIncludePath(b.path("third_party/raylib/include/"));
    exe_mod.addLibraryPath(b.path("third_party/raylib/lib"));

    exe_mod.linkSystemLibrary("raylib", .{});

    const clay_mod = b.createModule(.{
        .target = target,
        .link_libc = true,
    });

    clay_mod.addCSourceFile(.{
        .file = b.path("src/renderer/clay_renderer_raylib.c"),
        .flags = &.{ "-std=c99", "-DCLAY_IMPLEMENTATION" },
        .language = .c,
    });
    clay_mod.addIncludePath(b.path("third_party/clay/include/"));
    clay_mod.addIncludePath(b.path("third_party/raylib/include/"));

    const clay_lib = b.addLibrary(.{
        .name = "clay",
        .root_module = clay_mod,
        .linkage = .static,
    });

    exe_mod.linkLibrary(clay_lib);

    const exe = b.addExecutable(.{
        .name = "HexViewer",
        .root_module = exe_mod,
    });

    if (target.result.os.tag == .macos) {
        exe_mod.linkFramework("Cocoa", .{});
        exe_mod.linkFramework("IOKit", .{});
        exe_mod.linkFramework("CoreVideo", .{});
    }

    if (target.result.os.tag == .windows) {
        exe_mod.linkSystemLibrary("gdi32", .{});
        exe_mod.linkSystemLibrary("user32", .{});
        exe_mod.linkSystemLibrary("shell32", .{});
    }

    const compile_steps = b.allocator.dupe(*std.Build.Step.Compile, &.{ clay_lib, exe }) catch @panic("OOM");
    zcc.createStep(b, "cdb", compile_steps);

    b.installArtifact(exe);
}
