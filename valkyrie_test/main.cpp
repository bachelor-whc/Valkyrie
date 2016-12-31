#include <iostream>
#include <gtest/gtest.h>
#include "valkyrie.h"
#include "valkyrie/asset/gltf_asset.h"

using Valkyrie::MemoryChunk;
using Valkyrie::MemoryChunkPtr;
using Valkyrie::glTFBuffer;
using Valkyrie::glTFBufferPtr;
using Valkyrie::glTFBufferView;
using Valkyrie::glTFBufferViewPtr;
using Valkyrie::glTFAccessor;
using Valkyrie::glTFAccessorPtr;

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
	MemoryChunkPtr cptr = std::make_shared<Valkyrie::MemoryChunk>();
	glTFBufferPtr buffer_ptr = std::make_shared<glTFBuffer>(cptr);
	glTFBufferViewPtr buffer_view_ptr_1 = std::make_shared<glTFBufferView>(buffer_ptr, 48, 0);
	glTFBufferViewPtr buffer_view_ptr_2 = std::make_shared<glTFBufferView>(buffer_ptr, 128, 48);
	glTFBufferViewPtr buffer_view_ptr_3 = std::make_shared<glTFBufferView>(buffer_ptr, 256, 256);
	glTFAccessorPtr accessor_ptr_1 = std::make_shared<glTFAccessor>(buffer_view_ptr_1, 0, 0, )
	//cptr
};

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}