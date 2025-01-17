#pragma once

#include "VltCommon.h"
#include "UtilSync.h"

namespace sce::vlt
{
	enum class VltAccess
	{
		Read  = 0,
		Write = 1,
		None  = 2,
	};

	using VltAccessFlags = util::Flags<VltAccess>;

	/**
     * \brief VLT resource
     * 
     * Keeps track of whether the resource is currently in use
     * by the GPU. As soon as a command that uses the resource
     * is recorded, it will be marked as 'in use'.
     */
	class VltResource : public RcObject
	{

	public:
		virtual ~VltResource();

		/**
         * \brief Checks whether resource is in use
         * 
         * Returns \c true if there are pending accesses to
         * the resource by the GPU matching the given access
         * type. Note that checking for reads will also return
         * \c true if the resource is being written to.
         * \param [in] access Access type to check for
         * \returns \c true if the resource is in use
         */
		bool isInUse(VltAccess access = VltAccess::Read) const
		{
			bool result = m_useCountW.load() != 0;
			if (access == VltAccess::Read)
				result |= m_useCountR.load() != 0;
			return result;
		}

		/**
         * \brief Acquires resource
         * 
         * Increments use count for the given access type.
         * \param Access Resource access type
         */
		void acquire(VltAccess access)
		{
			if (access != VltAccess::None)
			{
				(access == VltAccess::Read
					 ? m_useCountR
					 : m_useCountW) += 1;
			}
		}

		/**
         * \brief Releases resource
         * 
         * Decrements use count for the given access type.
         * \param Access Resource access type
         */
		void release(VltAccess access)
		{
			if (access != VltAccess::None)
			{
				(access == VltAccess::Read
					 ? m_useCountR
					 : m_useCountW) -= 1;
			}
		}

		/**
         * \brief Waits for resource to become unused
         *
         * Blocks calling thread until the GPU finishes
         * using the resource with the given access type.
         * \param [in] access Access type to check for
         */
		void waitIdle(VltAccess access = VltAccess::Read) const
		{
			util::sync::spin(50000, [this, access]
					   { return !isInUse(access); });
		}

	private:
		std::atomic<uint32_t> m_useCountR = { 0u };
		std::atomic<uint32_t> m_useCountW = { 0u };
	};
}  // namespace sce::vlt