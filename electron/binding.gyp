{
	"targets": [
		{
			"target_name": "valkyrie",
			"sources": [ "valkyrie.cpp" ],
			'include_dirs': [
				'../Valkyrie/include',
				'../Valkyrie/third_party',
				'C:/VulkanSDK/1.0.30.0/Include'
			],
			'libraries' : [
				'../../lib/x64/Release/Valkyrie.lib',
				'../../lib/x64/Release/libpng.lib',
				'../../lib/x64/Release/zlib.lib',
				'C:/VulkanSDK/1.0.30.0/Bin/vulkan-1.lib'
			],
			'conditions': [
				[
					'OS=="win"', {
						'defines': [
							'_CRT_SECURE_NO_WARNINGS',
							'_ELECTRON_WIN32'
						]
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