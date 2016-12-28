{
	"targets": [
		{
			"target_name": "valkyrie",
			"sources": [ "valkyrie.cpp" ],
			"include_dirs": [
				"../Valkyrie/include",
				"../Valkyrie/third_party",
				"C:/VulkanSDK/1.0.30.0/Include"
			],
			"conditions": [
				[
					"OS=='win'", {
						"libraries": [
							"Valkyrie.lib",
							"libpng.lib",
							"zlib.lib",
							"C:/VulkanSDK/1.0.30.0/Bin/vulkan-1.lib"
						],
						"defines": [
							"_CRT_SECURE_NO_WARNINGS",
							"_ELECTRON_WIN32"
						],
						"configurations": {
							"Debug": {
								"library_dirs": [ "../lib/x64/Debug/" ]
							},
							"Release": {
								"library_dirs": [ "../lib/x64/Release/" ]
							}
						}
					}
				]
			]
		},
		{
			"target_name": "test",
			"sources": [ "test.cpp" ]
		}
	]
}