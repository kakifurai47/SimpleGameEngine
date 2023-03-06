

git clone https://github.com/Microsoft/vcpkg.git externals/vcpkg

sh externals/vcpkg/bootstrap-vcpkg.sh

externals/vcpkg/vcpkg install fmt 							--recurse
externals/vcpkg/vcpkg install eastl			 				--recurse
externals/vcpkg/vcpkg install glew 			 				--recurse
externals/vcpkg/vcpkg install nlohmann-json	 				--recurse
externals/vcpkg/vcpkg install libpng		 			    --recurse
externals/vcpkg/vcpkg install imgui[dx11-binding] 			--recurse
externals/vcpkg/vcpkg install imgui[win32-binding] 			--recurse
externals/vcpkg/vcpkg install imgui[docking-experimental] 	--recurse

