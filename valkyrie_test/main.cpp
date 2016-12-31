#include <iostream>
#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "valkyrie.h"
#include "valkyrie/asset/gltf_asset.h"
#include "valkyrie/graphics_api_support/attribute.h"

using Valkyrie::MemoryChunk;
using Valkyrie::MemoryChunkPtr;
using Valkyrie::glTFBuffer;
using Valkyrie::glTFBufferPtr;
using Valkyrie::glTFBufferView;
using Valkyrie::glTFBufferViewPtr;
using Valkyrie::glTFAccessor;
using Valkyrie::glTFAccessorPtr;
using Valkyrie::GrpahicsAPIAttributeSupport;
using Valkyrie::GAPIAttributeSupportPtr;
using Valkyrie::GrpahicsAPIAttribute;

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
	glTFBufferPtr buffer_ptr_1 = std::make_shared<glTFBuffer>(cptr_1);
	glTFBufferPtr buffer_ptr_2 = std::make_shared<glTFBuffer>(cptr_2);
	glTFBufferViewPtr buffer_view_ptr_1 = std::make_shared<glTFBufferView>(buffer_ptr_1, 64, 0);
	glTFBufferViewPtr buffer_view_ptr_2 = std::make_shared<glTFBufferView>(buffer_ptr_1, 128, 64);
	glTFBufferViewPtr buffer_view_ptr_3 = std::make_shared<glTFBufferView>(buffer_ptr_1, 256, 256);
	glTFBufferViewPtr buffer_view_ptr_4 = std::make_shared<glTFBufferView>(buffer_ptr_2, 2200, 0);
	glTFBufferViewPtr buffer_view_ptr_5 = std::make_shared<glTFBufferView>(buffer_ptr_2, 64, 2200);
	cptr_1->allocate(512);
	cptr_2->allocate(2264);
	unsigned char* ptr1 = (unsigned char*)cptr_1->getData();
	unsigned char* ptr2 = (unsigned char*)cptr_2->getData();
	ASSERT_NE(cptr_1->getData(), cptr_2->getData());
	ASSERT_EQ(cptr_1->getData(), buffer_ptr_1->getData());
	ASSERT_EQ(cptr_2->getData(), buffer_ptr_2->getData());
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
	glTFAccessorPtr accessor_ptr_1 = MAKE_SHARED(glTFAccessor)(buffer_view_ptr_1, gaas_ptr_1, glTFAccessor::GLTF_TYPE::MAT4, (glTFAccessor::GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_2 = MAKE_SHARED(glTFAccessor)(buffer_view_ptr_2, gaas_ptr_2, glTFAccessor::GLTF_TYPE::SCALAR, (glTFAccessor::GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_3 = MAKE_SHARED(glTFAccessor)(buffer_view_ptr_2, gaas_ptr_3, glTFAccessor::GLTF_TYPE::SCALAR, (glTFAccessor::GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_4 = MAKE_SHARED(glTFAccessor)(buffer_view_ptr_3, gaas_ptr_4, glTFAccessor::GLTF_TYPE::VEC3, (glTFAccessor::GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_5 = MAKE_SHARED(glTFAccessor)(buffer_view_ptr_4, gaas_ptr_5, glTFAccessor::GLTF_TYPE::VEC3, (glTFAccessor::GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_6 = MAKE_SHARED(glTFAccessor)(buffer_view_ptr_4, gaas_ptr_6, glTFAccessor::GLTF_TYPE::VEC2, (glTFAccessor::GLTF_COMPONENT_TYPE)5126);
	glTFAccessorPtr accessor_ptr_7 = MAKE_SHARED(glTFAccessor)(buffer_view_ptr_5, gaas_ptr_7, glTFAccessor::GLTF_TYPE::SCALAR, (glTFAccessor::GLTF_COMPONENT_TYPE)5123);
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

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}