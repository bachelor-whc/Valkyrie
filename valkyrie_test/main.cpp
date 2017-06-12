#include <iostream>
#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <tbb/tick_count.h>
#include <tbb/task_scheduler_init.h>
#include "valkyrie.h"
#include "valkyrie/utility/task_manager.h"
#include "valkyrie/graphics_api_support/attribute.h"
#include "valkyrie/factory/object.h"
#include "valkyrie/scene/object_manager.h"

using Valkyrie::MemoryChunk;
using Valkyrie::MemoryChunkPtr;
using Valkyrie::GrpahicsAPIAttributeSupport;
using Valkyrie::GAPIAttributeSupportPtr;
using Valkyrie::GrpahicsAPIAttribute;
using Valkyrie::TaskManager;
using Valkyrie::AssetManager;
using Valkyrie::ObjectManager;
using ValkyrieFactory::ObjectFactory;

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
	auto& asset_manager = AssetManager::instance();

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

TEST(ObjectFactory, CreateAndQueryObject) {
	ObjectManager::initialize();
	ObjectFactory::initialize();
	auto& manager = ObjectManager::instance();
	auto& factory = ObjectFactory::instance();
	auto& duck = factory.createObject();
	auto ID = duck->getID();
	auto& query = manager.getObject(ID);
	ASSERT_EQ(ID, query->getID());
	ASSERT_EQ(duck.get(), query.get());
	ASSERT_EQ(nullptr, manager.getObject(-1));
	ObjectFactory::close();
	ObjectManager::close();
}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}