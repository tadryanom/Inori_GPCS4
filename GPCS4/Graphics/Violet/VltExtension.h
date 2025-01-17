#pragma once

#include "VltCommon.h"

#include <algorithm>
#include <map>
#include <vector>

namespace sce::vlt
{

	/**
     * \brief Vulkan extension mode
     * 
     * Defines whether an extension is
     * optional, required, or disabled.
     */
	enum class VltExtMode
	{
		Disabled,
		Optional,
		Required,
		Passive,
	};

	/**
     * \brief Vulkan extension info
     * 
     * Stores information for a single extension.
     * The renderer can use this information to
     * find out which extensions are enabled.
     */
	class VltExt
	{

	public:
		VltExt(
			const char* pName,
			VltExtMode mode) :
			m_name(pName),
			m_mode(mode)
		{
		}

		/**
         * \brief Extension name
         * \returns Extension name
         */
		const char* name() const
		{
			return m_name;
		}

		/**
         * \brief Extension mode
         * \returns Extension mode
         */
		VltExtMode mode() const
		{
			return m_mode;
		}

		/**
         * \brief Checks whether the extension is enabled
         * 
         * If an extension is enabled, the features
         * provided by the extension can be used.
         * \returns \c true if the extension is enabled
         */
		operator bool() const
		{
			return m_revision != 0;
		}

		/**
         * \brief Supported revision
         * \returns Supported revision
         */
		uint32_t revision() const
		{
			return m_revision;
		}

		/**
         * \brief Changes extension mode
         * 
         * In some cases, it may be useful to change the
         * default mode dynamically after initialization.
         */
		void setMode(VltExtMode mode)
		{
			m_mode = mode;
		}

		/**
         * \brief Enables the extension
         */
		void enable(uint32_t revision)
		{
			m_revision = revision;
		}

		/**
         * \brief Disables the extension
         */
		void disable()
		{
			m_revision = 0;
		}

	private:
		const char* m_name     = nullptr;
		VltExtMode  m_mode     = VltExtMode::Disabled;
		uint32_t    m_revision = 0;
	};

	/**
     * \brief Vulkan name list
     * 
     * A simple \c vector wrapper that can
     * be used to build a list of layer and
     * extension names.
     */
	class VltNameList
	{

	public:
		/**
         * \brief Adds a name
         * \param [in] pName The name
         */
		void add(const char* pName)
		{
			m_names.push_back(pName);
		}

		/**
         * \brief Number of names
         * \returns Name count
         */
		uint32_t count() const
		{
			return m_names.size();
		}

		/**
         * \brief Name list
         * \returns Name list
         */
		const char* const* names() const
		{
			return m_names.data();
		}

		/**
         * \brief Retrieves a single name
         * 
         * \param [in] index Name index
         * \returns The given name
         */
		const char* name(uint32_t index) const
		{
			return m_names.at(index);
		}

	private:
		std::vector<const char*> m_names;
	};

	/**
     * \brief Vulkan extension set
     * 
     * Stores a set of extensions or layers
     * supported by the Vulkan implementation.
     */
	class VltNameSet
	{

	public:
		VltNameSet();
		~VltNameSet();

		/**
         * \brief Adds a name to the set
         * \param [in] pName Extension name
         */
		void add(
			const char* pName);

		/**
         * \brief Merges two name sets
         * 
         * Adds all names from the given name set to
         * this name set, avoiding duplicate entries.
         * \param [in] names Name set to merge
         */
		void merge(
			const VltNameSet& names);

		/**
         * \brief Checks whether an extension is supported
         * 
         * \param [in] pName Extension name
         * \returns Supported revision, or zero
         */
		uint32_t supports(
			const char* pName) const;

		/**
         * \brief Enables requested extensions
         * 
         * Walks over a set of extensions and enables all
         * extensions that are supported and not disabled.
         * This also checks whether all required extensions
         * could be enabled, and returns \c false otherwise.
         * \param [in] numExtensions Number of extensions
         * \param [in] ppExtensions List of extensions
         * \param [out] nameSet Extension name set
         * \returns \c true on success
         */
		bool enableExtensions(
			uint32_t    numExtensions,
			VltExt**    ppExtensions,
			VltNameSet& nameSet) const;

		/**
         * \brief Disables given extension
         *
         * Removes the given extension from the set
         * and sets its revision to 0 (i.e. disabled).
         * \param [in,out] ext Extension to disable
         */
		void disableExtension(
			VltExt& ext);

		/**
         * \brief Creates name list from name set
         * 
         * Adds all names contained in the name set
         * to a name list, which can then be passed
         * to Vulkan functions.
         * \returns Name list
         */
		VltNameList toNameList() const;

		/**
         * \brief Enumerates instance layers
         * 
         * \returns Set of available instance layers
         */
		static VltNameSet enumInstanceLayers();

		/**
         * \brief Enumerates instance extensions
         * 
         * \returns Set of available instance extensions
         */
		static VltNameSet enumInstanceExtensions();

		/**
         * \brief Enumerates device extensions
         * 
         * \param [in] device The device to query
         * \returns Set of available device extensions
         */
		static VltNameSet enumDeviceExtensions(
			VkPhysicalDevice          device);

	private:
		std::map<std::string, uint32_t> m_names;
	};

	/**
     * \brief Device extensions
     * 
     * Lists all Vulkan extensions that are potentially
     * used by DXVK if supported by the implementation.
     */
	struct VltDeviceExtensions
	{
		VltExt amdMemoryOverallocationBehaviour  = { VK_AMD_MEMORY_OVERALLOCATION_BEHAVIOR_EXTENSION_NAME, VltExtMode::Optional };
		VltExt amdShaderFragmentMask             = { VK_AMD_SHADER_FRAGMENT_MASK_EXTENSION_NAME, VltExtMode::Optional };
		VltExt ext4444Formats                    = { VK_EXT_4444_FORMATS_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extConservativeRasterization      = { VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extCustomBorderColor              = { VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extDepthClipEnable                = { VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extExtendedDynamicState           = { VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME, VltExtMode::Optional };
		// This is a Win32 specific extension
		// VltExt extFullScreenExclusive            = { VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extHostQueryReset                 = { VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extMemoryBudget                   = { VK_EXT_MEMORY_BUDGET_EXTENSION_NAME, VltExtMode::Passive };
		VltExt extMemoryPriority                 = { VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extRobustness2                    = { VK_EXT_ROBUSTNESS_2_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extShaderDemoteToHelperInvocation = { VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extShaderStencilExport            = { VK_EXT_SHADER_STENCIL_EXPORT_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extShaderViewportIndexLayer       = { VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extTransformFeedback              = { VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME, VltExtMode::Optional };
		VltExt extVertexAttributeDivisor         = { VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME, VltExtMode::Optional };
		VltExt khrBufferDeviceAddress            = { VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, VltExtMode::Disabled };
		VltExt khrCreateRenderPass2              = { VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, VltExtMode::Optional };
		VltExt khrDepthStencilResolve            = { VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, VltExtMode::Optional };
		VltExt khrDrawIndirectCount              = { VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME, VltExtMode::Optional };
		VltExt khrDriverProperties               = { VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, VltExtMode::Optional };
		VltExt khrImageFormatList                = { VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME, VltExtMode::Required };
		VltExt khrSamplerMirrorClampToEdge       = { VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME, VltExtMode::Optional };
		VltExt khrShaderFloatControls            = { VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME, VltExtMode::Optional };
		VltExt khrSwapchain                      = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VltExtMode::Required };
		VltExt nvxBinaryImport                   = { VK_NVX_BINARY_IMPORT_EXTENSION_NAME, VltExtMode::Disabled };
		VltExt nvxImageViewHandle                = { VK_NVX_IMAGE_VIEW_HANDLE_EXTENSION_NAME, VltExtMode::Disabled };
	};

	/**
     * \brief Instance extensions
     * 
     * Lists all Vulkan extensions that are potentially
     * used by DXVK if supported by the implementation.
     */
	struct VltInstanceExtensions
	{
		VltExt extDebugUtils              = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VltExtMode::Optional };
		VltExt khrGetSurfaceCapabilities2 = { VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, VltExtMode::Optional };
		VltExt khrSurface                 = { VK_KHR_SURFACE_EXTENSION_NAME, VltExtMode::Required };
	};

}  // namespace sce::vlt