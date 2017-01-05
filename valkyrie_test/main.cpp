#include <iostream>
#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "valkyrie.h"
#include "valkyrie/asset/gltf_asset.h"
#include "valkyrie/graphics_api_support/attribute.h"

using Valkyrie::MemoryChunk;
using Valkyrie::MemoryChunkPtr;
using Valkyrie::glTFBufferView;
using Valkyrie::glTFBufferViewPtr;
using Valkyrie::glTFAccessor;
using Valkyrie::glTFAccessorPtr;
using Valkyrie::GrpahicsAPIAttributeSupport;
using Valkyrie::GAPIAttributeSupportPtr;
using Valkyrie::GrpahicsAPIAttribute;
using Valkyrie::GLTF_TYPE;
using Valkyrie::GLTF_COMPONENT_TYPE;
using Valkyrie::ThreadManager;
using Valkyrie::AssetManager;

TEST(MemoryChunckCheck, Normal) {
	MemoryChunk c1;
	ASSERT_EQ(c1.getData(), nullptr);
	c1.allocate(128);
	void* prev_ptr = c1.getData();
	ASSERT_NE(c1.getData(), nullptr);
	c1.allocate(64);
	ASSERT_EQ(c1.getData(), prev_ptr);
}

TEST(MemoryChunckCheck, SharedPtr) {
	MemoryChunkPtr cptr = std::make_shared<MemoryChunk>();
	MemoryChunkPtr cptr_copy_1 = cptr;
	MemoryChunkPtr cptr_copy_2 = cptr_copy_1;
	ASSERT_EQ(cptr->getData(), nullptr);
	ASSERT_EQ(cptr_copy_1->getData(), nullptr);
	ASSERT_EQ(cptr_copy_2->getData(), nullptr);
	cptr->allocate(128);
	void* prev_ptr = cptr->getData();
	ASSERT_NE(cptr->getData(), nullptr);
	ASSERT_NE(cptr_copy_1->getData(), nullptr);
	ASSERT_NE(cptr_copy_2->getData(), nullptr);
	ASSERT_EQ(cptr->getData(), cptr_copy_1->getData());
	ASSERT_EQ(cptr_copy_1->getData(), cptr_copy_2->getData());
	cptr->allocate(64);
	ASSERT_EQ(cptr->getData(), prev_ptr);
	ASSERT_EQ(cptr_copy_1->getData(), prev_ptr);
	ASSERT_EQ(cptr_copy_2->getData(), prev_ptr);
}

TEST(glTFAssetCheck, Initialization) {
	MemoryChunkPtr cptr_1 = std::make_shared<Valkyrie::MemoryChunk>();
	MemoryChunkPtr cptr_2 = std::make_shared<Valkyrie::MemoryChunk>();
	ASSERT_EQ(cptr_1->getData(), nullptr);
	ASSERT_EQ(cptr_2->getData(), nullptr);
	glTFBufferViewPtr buffer_view_ptr_1 = std::make_shared<glTFBufferView>(cptr_1, 64, 0);
	glTFBufferViewPtr buffer_view_ptr_2 = std::make_shared<glTFBufferView>(cptr_1, 128, 64);
	glTFBufferViewPtr buffer_view_ptr_3 = std::make_shared<glTFBufferView>(cptr_1, 256, 256);
	glTFBufferViewPtr buffer_view_ptr_4 = std::make_shared<glTFBufferView>(cptr_2, 2200, 0);
	glTFBufferViewPtr buffer_view_ptr_5 = std::make_shared<glTFBufferView>(cptr_2, 64, 2200);
	cptr_1->allocate(512);
	cptr_2->allocate(2264);
	unsigned char* ptr1 = (unsigned char*)cptr_1->getData();
	unsigned char* ptr2 = (unsigned char*)cptr_2->getData();
	ASSERT_EQ(ptr1 + 64, buffer_view_ptr_2->getData());
	ASSERT_EQ(ptr1 + 256, buffer_view_ptr_3->getData());
	ASSERT_EQ(ptr2 + 2200, buffer_view_ptr_5->getData());
	const uint32_t count[7] = { 1, 16, 5, 21, 110, 110, 32 };
	GAPIAttributeSupportPtr gaas_ptr_1 = MAKE_SHARED(GrpahicsAPIAttribute<glm::mat4>)(buffer_view_ptr_1);
	GAPIAttributeSupportPtr gaas_ptr_2 = MAKE_SHARED(GrpahicsAPIAttribute<glm::float32>)(buffer_view_ptr_2, 0, 0, count[1]);
	GAPIAttributeSupportPtr gaas_ptr_3 = MAKE_SHARED(GrpahicsAPIAttribute<glm::float32>)(buffer_view_ptr_2, 64, 12, count[2]);
	GAPIAttributeSupportPtr gaas_ptr_4 = MAKE_SHARED(GrpahicsAPIAttribute<glm::vec3>)(buffer_view_ptr_3, 0, 0, count[3]);
	GAPIAttributeSupportPtr gaas_ptr_5 = MAKE_SHARED(GrpahicsAPIAttribute<glm::vec3>)(buffer_view_ptr_4, 0, 20, count[4]);
	GAPIAttributeSupportPtr gaas_ptr_6 = MAKE_SHARED(GrpahicsAPIAttribute<glm::vec2>)(buffer_view_ptr_4, 12, 20, count[5]);
	GAPIAttributeSupportPtr gaas_ptr_7 = MAKE_SHARED(GrpahicsAPIAttribute<uint16_t>)(buffer_view_ptr_5, 0, 2, count[6]);
	glTFAccessorPtr accessor_ptr_1 = MAKE_SHARED(glTFAccessor)(gaas_ptr_1, GLTF_TYPE::MAT4, (GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_2 = MAKE_SHARED(glTFAccessor)(gaas_ptr_2, GLTF_TYPE::SCALAR, (GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_3 = MAKE_SHARED(glTFAccessor)(gaas_ptr_3, GLTF_TYPE::SCALAR, (GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_4 = MAKE_SHARED(glTFAccessor)(gaas_ptr_4, GLTF_TYPE::VEC3, (GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_5 = MAKE_SHARED(glTFAccessor)(gaas_ptr_5, GLTF_TYPE::VEC3, (GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_6 = MAKE_SHARED(glTFAccessor)(gaas_ptr_6, GLTF_TYPE::VEC2, (GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_7 = MAKE_SHARED(glTFAccessor)(gaas_ptr_7, GLTF_TYPE::SCALAR, (GLTF_COMPONENT_TYPE)5123);
	unsigned char* bvptr_1 = (unsigned char*)buffer_view_ptr_1->getData();
	unsigned char* bvptr_2 = (unsigned char*)buffer_view_ptr_2->getData();
	unsigned char* bvptr_3 = (unsigned char*)buffer_view_ptr_3->getData();
	unsigned char* bvptr_4 = (unsigned char*)buffer_view_ptr_4->getData();
	unsigned char* bvptr_5 = (unsigned char*)buffer_view_ptr_5->getData();
	ASSERT_EQ(accessor_ptr_1->getData(), bvptr_1);
	ASSERT_EQ(accessor_ptr_2->getData(), bvptr_2);
	ASSERT_EQ(accessor_ptr_3->getData(), bvptr_2 + 64);
	ASSERT_EQ(accessor_ptr_4->getData(), bvptr_3);
	ASSERT_EQ(accessor_ptr_5->getData(), bvptr_4);
	ASSERT_EQ(accessor_ptr_6->getData(), bvptr_4 + 12);
	ASSERT_EQ(accessor_ptr_7->getData(), bvptr_5);
	for (int i = 0; i < count[6]; ++i) { *((uint16_t*)accessor_ptr_7->getInstance(count[6] - i - 1)) = i; }
	for (int i = 0; i < count[5]; ++i) { *((glm::vec2*)accessor_ptr_6->getInstance(count[5] - i - 1)) = glm::vec2((float)i, (float)i); }
	for (int i = 0; i < count[4]; ++i) { *((glm::vec3*)accessor_ptr_5->getInstance(count[4] - i - 1)) = glm::vec3((float)i, (float)i, (float)i); }
	for (int i = 0; i < count[3]; ++i) { *((glm::vec3*)accessor_ptr_4->getInstance(count[3] - i - 1)) = glm::vec3((float)i, (float)i, (float)i); }
	for (int i = 0; i < count[2]; ++i) { *((glm::float32*)accessor_ptr_3->getInstance(count[2] - i - 1)) = glm::float32((float)i); }
	for (int i = 0; i < count[1]; ++i) { *((glm::float32*)accessor_ptr_2->getInstance(count[1] - i - 1)) = glm::float32((float)i); }
	for (int i = 0; i < count[0]; ++i) { *((glm::mat4*)accessor_ptr_1->getInstance(count[0] - i - 1)) = glm::mat4((float)i); }
	ASSERT_EQ(*((uint16_t*)accessor_ptr_7->getInstance(count[6] - 2)), 1);
	ASSERT_EQ(*((uint16_t*)accessor_ptr_7->getInstance(0)), count[6] - 1);
	ASSERT_EQ(*((glm::vec2*)accessor_ptr_6->getInstance(count[5] - 2)), glm::vec2(1, 1));
	ASSERT_EQ(*((glm::vec2*)accessor_ptr_6->getInstance(0)), glm::vec2(count[5] - 1, count[5] - 1));
	ASSERT_EQ(*((glm::vec3*)accessor_ptr_5->getInstance(count[4] - 2)), glm::vec3(1, 1, 1));
	ASSERT_EQ(*((glm::vec3*)accessor_ptr_4->getInstance(count[3] - 2)), glm::vec3(1, 1, 1));
	ASSERT_EQ(*((glm::float32*)accessor_ptr_3->getInstance(count[2] - 2)), glm::float32(1));
	ASSERT_EQ(*((glm::float32*)accessor_ptr_2->getInstance(count[1] - 2)), glm::float32(1));
	ASSERT_EQ(*((glm::mat4*)accessor_ptr_1->getInstance(count[0] - 1)), glm::mat4(0));
};

TEST(FillMemoryCheck, File) {
	AssetManager::initialize();
	auto& asset_manager = *AssetManager::getGlobalAssetMangerPtr();

	const unsigned char test_bin[] = {
		0x12, 0x34, 0x56, 0x78, 0x9A,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xAB, 0xCD, 0xEF
	};
	FILE* p_file = fopen("assets/test.bin", "wb");
	fwrite(test_bin, 1, 13, p_file);
	fclose(p_file);
	MemoryChunkPtr cptr_1 = std::make_shared<Valkyrie::MemoryChunk>();
	MemoryChunkPtr cptr_2 = std::make_shared<Valkyrie::MemoryChunk>();
	MemoryChunkPtr cptr_3 = std::make_shared<Valkyrie::MemoryChunk>();
	ASSERT_FALSE(cptr_1->allocated());
	ASSERT_FALSE(cptr_2->allocated());
	ASSERT_FALSE(cptr_3->allocated());
	cptr_2->allocate(12);
	cptr_3->allocate(14);
	ASSERT_FALSE(cptr_1->allocated());
	ASSERT_TRUE(cptr_2->allocated());
	ASSERT_TRUE(cptr_3->allocated());
	asset_manager.load(cptr_1, "test.bin");
	EXPECT_THROW(asset_manager.load(cptr_2, "test.bin"), std::exception);
	asset_manager.load(cptr_3, "test.bin");
	ASSERT_TRUE(memcmp(cptr_1->getData(), test_bin, 13) == 0);
	ASSERT_TRUE(memcmp(cptr_3->getData(), test_bin, 13) == 0);
	ASSERT_TRUE(cptr_1->ready());
	ASSERT_FALSE(cptr_2->ready());
	ASSERT_TRUE(cptr_3->ready());
	cptr_1->unsetFlags(MemoryAccess::READY);
	cptr_3->unsetFlags(MemoryAccess::READY);
	ASSERT_TRUE(cptr_1->allocated());
	ASSERT_TRUE(cptr_3->allocated());
	ASSERT_FALSE(cptr_1->ready());
	ASSERT_FALSE(cptr_3->ready());
	cptr_1->unsetFlags(MemoryAccess::ALLOCATED);
	cptr_2->unsetFlags(MemoryAccess::ALLOCATED);
	cptr_3->unsetFlags(MemoryAccess::ALLOCATED);
	ASSERT_FALSE(cptr_1->allocated());
	ASSERT_FALSE(cptr_2->allocated());
	ASSERT_FALSE(cptr_3->allocated());
}

TEST(ThreadManager, Initialization) {
	ThreadManager::initialize();
	ThreadManager& thread_manager = *ThreadManager::getGlobalThreadManagerPtr();
	auto hardware_count = std::thread::hardware_concurrency();
	unsigned int count = hardware_count == 0 ? 2 : hardware_count;
	ASSERT_EQ(thread_manager.getThreadCount(), count);
}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}