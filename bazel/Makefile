generate:
	echo "You do not need to run this any more for C++ code, only if you modify go code"

generate_go:
	cd .. && bazelisk run //:gazelle -- update infra/bots/task_drivers modules/canvaskit/go

# This target should be run after the go.mod file is updated (e.g. version rolls or new updates)
gazelle_update_repo:
	cd .. && bazelisk run //:gazelle -- update-repos -from_file=go.mod -to_macro=go_repositories.bzl%go_repositories

# Run this target to test all known working Bazel builds
known_good_builds:
	bazelisk build //:skia_public
	bazelisk build //:skia_internal
	bazelisk build //experimental/bazel_test/...
	bazelisk run //experimental/bazel_test:bazel_test_exe
	bazelisk build //example:hello_world_gl
	bazelisk build //example:hello_world_vulkan
	bazelisk build //example:hello_world_dawn
	bazelisk build //example:vulkan_basic
	bazelisk build //src/sksl/lex:sksllex
	bazelisk build //tools/skdiff
	bazelisk build //tools/skslc
	bazelisk build //modules/canvaskit:canvaskit_wasm --config=ck_release

rbe_known_good_builds:
	bazelisk build //:skia_public --config=for_linux_x64_with_rbe --remote_download_minimal
	bazelisk build //:skia_internal --config=for_linux_x64_with_rbe --remote_download_minimal
	bazelisk build //experimental/bazel_test:bazel_test_exe --config=for_linux_x64_with_rbe --remote_download_minimal
	bazelisk build //example:hello_world_gl --config=for_linux_x64_with_rbe --remote_download_minimal
	bazelisk build //example:hello_world_vulkan --config=for_linux_x64_with_rbe --remote_download_minimal
	bazelisk build //example:hello_world_dawn --config=for_linux_x64_with_rbe --remote_download_minimal
	bazelisk build //example:vulkan_basic --config=for_linux_x64_with_rbe --remote_download_minimal
	bazelisk build //src/sksl/lex:sksllex --config=for_linux_x64_with_rbe --remote_download_minimal
	bazelisk build //tools/skdiff --config=for_linux_x64_with_rbe --remote_download_minimal
	bazelisk build //tools/skslc --config=for_linux_x64_with_rbe --remote_download_minimal
	## TODO(kjlubick) CanvasKit in release mode (i.e. with Closure) requires
	## https://github.com/emscripten-core/emscripten/pull/16640 to land
	bazelisk build //modules/canvaskit:canvaskit_wasm --config=linux_rbe --config=ck_debug \
		--remote_download_minimal

iwyu_rbe:
	bazelisk build //:skia_public --config=for_linux_x64_with_rbe --config=enforce_iwyu \
	 	--keep_going --remote_download_minimal
	bazelisk build //example:hello_world_gl --config=for_linux_x64_with_rbe --config=enforce_iwyu \
	 	--keep_going --remote_download_minimal
	bazelisk build //example:hello_world_vulkan --config=for_linux_x64_with_rbe --config=enforce_iwyu \
	 	--keep_going --remote_download_minimal
	bazelisk build //example:hello_world_dawn --config=for_linux_x64_with_rbe --config=enforce_iwyu \
	 	--keep_going --remote_download_minimal
	bazelisk build //example:vulkan_basic --config=for_linux_x64_with_rbe --config=enforce_iwyu \
	 	--keep_going --remote_download_minimal

iwyu:
	bazelisk build //:skia_public --config=enforce_iwyu \
	 	--keep_going
	bazelisk build //example:hello_world_gl --config=enforce_iwyu \
	 	--keep_going
	bazelisk build //example:hello_world_vulkan --config=enforce_iwyu \
	 	--keep_going
	bazelisk build //example:hello_world_dawn --config=enforce_iwyu \
	 	--keep_going
	bazelisk build //example:vulkan_basic --config=enforce_iwyu \
	 	--keep_going
