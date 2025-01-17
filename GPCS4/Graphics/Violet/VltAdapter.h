#pragma once

#include "VltCommon.h"
#include "VltDeviceInfo.h"
#include "VltExtension.h"

#include <array>

namespace sce::vlt
{
	class VltInstance;
	class VltDevice;

	/**
     * \brief Adapter memory heap info
     * 
     * Stores info about a heap, and the amount
     * of memory allocated from it by the app.
     */
	struct VltAdapterMemoryHeapInfo
	{
		VkMemoryHeapFlags heapFlags;
		VkDeviceSize      memoryBudget;
		VkDeviceSize      memoryAllocated;
	};

	/**
     * \brief Adapter memory info
     * 
     * Stores properties and allocation
     * info of each available heap.
     */
	struct VltAdapterMemoryInfo
	{
		uint32_t                 heapCount;
		VltAdapterMemoryHeapInfo heaps[VK_MAX_MEMORY_HEAPS];
	};

	/**
     * \brief Retrieves queue indices
     */
	struct VltAdapterQueueIndices
	{
		uint32_t graphics;
		uint32_t compute;
		uint32_t transfer;
	};

	/**
     * \brief VLT adapter
     * 
     * Corresponds to a physical device in Vulkan. Provides
     * all kinds of information about the device itself and
     * the supported feature set.
     */
	class VltAdapter : public RcObject
	{

	public:
		VltAdapter(VkPhysicalDevice handle);
		virtual ~VltAdapter();

		/**
        * \brief Physical device handle
        * \returns The adapter handle
        */
		VkPhysicalDevice handle() const
		{
			return m_handle;
		}

		/**
        * \brief Physical device properties
        * 
        * Returns a read-only reference to the core
        * properties of the Vulkan physical device.
        * \returns Physical device core properties
        */
		const VkPhysicalDeviceProperties& deviceProperties() const
		{
			return m_deviceInfo.core.properties;
		}

		/**
        * \brief Device info
        * 
        * Returns a read-only reference to the full
        * device info structure, including extended
        * properties.
        * \returns Device info struct
        */
		const VltDeviceInfo& devicePropertiesExt() const
		{
			return m_deviceInfo;
		}

		/**
        * \brief Supportred device features
        * 
        * Queries the supported device features.
        * \returns Device features
        */
		const VltDeviceFeatures& features() const
		{
			return m_deviceFeatures;
		}

		/**
        * \brief Retrieves memory heap info
        * 
        * Returns properties of all available memory heaps,
        * both device-local and non-local heaps, and the
        * amount of memory allocated from those heaps by
        * logical devices.
        * \returns Memory heap info
        */
		VltAdapterMemoryInfo getMemoryHeapInfo() const;

		/**
        * \brief Memory properties
        * 
        * Queries the memory types and memory heaps of
        * the device. This is useful for memory allocators.
        * \returns Device memory properties
        */
		VkPhysicalDeviceMemoryProperties memoryProperties() const;

		/**
        * \brief Queries format support
        * 
        * \param [in] format The format to query
        * \returns Format support info
        */
		VkFormatProperties formatProperties(
			VkFormat format) const;

		/**
        * \brief Queries image format support
        * 
        * \param [in] format Format to query
        * \param [in] type Image type
        * \param [in] tiling Image tiling
        * \param [in] usage Image usage flags
        * \param [in] flags Image create flags
        * \param [out] properties Format properties
        * \returns \c VK_SUCCESS or \c VK_ERROR_FORMAT_NOT_SUPPORTED
        */
		VkResult imageFormatProperties(
			VkFormat                 format,
			VkImageType              type,
			VkImageTiling            tiling,
			VkImageUsageFlags        usage,
			VkImageCreateFlags       flags,
			VkImageFormatProperties& properties) const;

		/**
        * \brief Retrieves queue family indices
        * \returns Indices for all queue families
        */
		VltAdapterQueueIndices findQueueFamilies() const;

		/**
        * \brief Enables extensions for this adapter
        *
        * When creating a device, all extensions that
        * are added using this method will be enabled
        * in addition to the ones required by DXVK.
        * This is used for OpenVR support.
        */
		void enableExtensions(
			const VltNameSet& extensions);

		/**
	  	  * \brief Creates a DXVK device
	  	  * 
	  	  * Creates a logical device for this adapter.
	  	  * \param [in] instance Parent instance
	  	  * \returns Device handle
	  	  */
		Rc<VltDevice> createDevice(
			const Rc<VltInstance>& instance);

		/**
        * \brief Registers memory allocation
        * 
        * Updates memory alloc info accordingly.
        * \param [in] heap Memory heap index
        * \param [in] bytes Allocation size
        */
		void notifyHeapMemoryAlloc(
			uint32_t     heap,
			VkDeviceSize bytes);

		/**
        * \brief Registers memory deallocation
        * 
        * Updates memory alloc info accordingly.
        * \param [in] heap Memory heap index
        * \param [in] bytes Allocation size
        */
		void notifyHeapMemoryFree(
			uint32_t     heap,
			VkDeviceSize bytes);

		/**
        * \brief Logs VLT adapter info
        * 
        * May be useful for bug reports
        * and general troubleshooting.
        */
		void logAdapterInfo() const;

		/**
        * \brief Checks whether this is a UMA system
        *
        * Basically tests whether all heaps are device-local.
        * Can be used for various optimizations in client APIs.
        * \returns \c true if the system has unified memory.
        */
		bool isUnifiedMemoryArchitecture() const;

	private:
		void initHeapAllocInfo();
		void queryExtensions();
		void queryDeviceInfo();
		void queryDeviceFeatures();
		void queryDeviceQueues();

		uint32_t findQueueFamily(
			VkQueueFlags mask,
			VkQueueFlags flags) const;

		bool checkFeatureSupport(
			const VltDeviceFeatures& required) const;

		VltDeviceFeatures getRequestFeatures();

		static void logNameList(const VltNameList& names);
		static void logFeatures(const VltDeviceFeatures& features);
		static void logQueueFamilies(const VltAdapterQueueIndices& queues);

	private:
		VkPhysicalDevice m_handle;

		VltNameSet        m_extraExtensions;
		VltNameSet        m_deviceExtensions;
		VltDeviceInfo     m_deviceInfo;
		VltDeviceFeatures m_deviceFeatures;

		bool m_hasMemoryBudget;

		std::vector<VkQueueFamilyProperties> m_queueFamilies;

		std::array<std::atomic<VkDeviceSize>, VK_MAX_MEMORY_HEAPS> m_heapAlloc;
	};

}  // namespace sce::vlt