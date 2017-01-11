#include <iostream>
#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <tbb/tbb.h>
#include "valkyrie.h"
#include "valkyrie/graphics_api_support/attribute.h"

using Valkyrie::MemoryChunk;
using Valkyrie::MemoryChunkPtr;
using Valkyrie::GrpahicsAPIAttributeSupport;
using Valkyrie::GAPIAttributeSupportPtr;
using Valkyrie::GrpahicsAPIAttribute;
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

TEST(FillMemoryCheck, File) {
	AssetManager::initialize();
	auto& asset_manager = *AssetManager::getGlobalAssetMangerPtr();

	const unsigned char test_bin[] = {
		0x12, 0x34, 0x56, 0x78, 0x9A,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xAB, 0xCD, 0xEF
	};

	const float floats_bin[3] = {
		35.02259826660156,
		89.38739776611328,
		23.373199462890625
	};

	FILE* p_file = fopen("assets/test.bin", "wb");
	fwrite(test_bin, 1, 13, p_file);
	fclose(p_file);
	asset_manager.load("test.bin");
	asset_manager.load("floats.bin");
	auto& cptr = std::static_pointer_cast<MemoryChunk>(asset_manager.getAsset("test.bin"));
	auto& cptr_floats = std::static_pointer_cast<MemoryChunk>(asset_manager.getAsset("floats.bin"));
	ASSERT_TRUE(memcmp(cptr->getData(), test_bin, 13) == 0);
	ASSERT_TRUE(memcmp(cptr_floats->getData(), floats_bin, 12) == 0);
	ASSERT_TRUE(cptr->ready());
	ASSERT_TRUE(cptr->allocated());
	ASSERT_TRUE(cptr_floats->ready());
	ASSERT_TRUE(cptr_floats->allocated());
	cptr->unsetFlags(MemoryAccess::READY);
	ASSERT_TRUE(cptr->allocated());
	ASSERT_FALSE(cptr->ready());
	cptr->unsetFlags(MemoryAccess::ALLOCATED);
	ASSERT_FALSE(cptr->allocated());
}

TEST(ThreadManager, Initialization) {
	ThreadManager::initialize();
	ThreadManager& thread_manager = *ThreadManager::getGlobalThreadManagerPtr();
	auto hardware_count = std::thread::hardware_concurrency();
	unsigned int count = hardware_count == 0 ? 2 : hardware_count;
	ASSERT_EQ(thread_manager.getThreadCount(), count);
}

TEST(TBB, Flask) {
	int sum = 0;
	tbb::parallel_for(0, 101, [=, &sum](int i) {sum += i; }); // 0 ~ 100
	ASSERT_TRUE(sum == 5050);
}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}