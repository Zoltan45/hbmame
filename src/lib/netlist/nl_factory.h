// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nl_factory.h
 *
 *
 */

#ifndef NLFACTORY_H_
#define NLFACTORY_H_

#include <vector>

#include "plib/palloc.h"
#include "plib/ptypes.h"

#define NETLIB_DEVICE_IMPL_ALIAS(p_alias, chip, p_name, p_def_param) \
	NETLIB_DEVICE_IMPL_BASE(devices, p_alias, chip, p_name, p_def_param) \

#define NETLIB_DEVICE_IMPL(chip, p_name, p_def_param) \
	NETLIB_DEVICE_IMPL_NS(devices, chip, p_name, p_def_param)

#define NETLIB_DEVICE_IMPL_NS(ns, chip, p_name, p_def_param) \
	NETLIB_DEVICE_IMPL_BASE(ns, chip, chip, p_name, p_def_param) \

#define NETLIB_DEVICE_IMPL_BASE(ns, p_alias, chip, p_name, p_def_param) \
	static std::unique_ptr<factory::element_t> NETLIB_NAME(p_alias ## _c) \
			(const pstring &classname) \
	{ \
		return std::unique_ptr<factory::element_t>(plib::palloc<factory::device_element_t<ns :: NETLIB_NAME(chip)>>(p_name, classname, p_def_param, pstring(__FILE__))); \
	} \
	\
	factory::constructor_ptr_t decl_ ## p_alias = NETLIB_NAME(p_alias ## _c);

namespace netlist {
	class device_t;
	class setup_t;
	class netlist_state_t;

namespace factory {
	// -----------------------------------------------------------------------------
	// net_dev class factory
	// -----------------------------------------------------------------------------

	class element_t : plib::nocopyassignmove
	{
	public:
		element_t(const pstring &name, const pstring &classname,
				const pstring &def_param);
		element_t(const pstring &name, const pstring &classname,
				const pstring &def_param, const pstring &sourcefile);
		virtual ~element_t() = default;

		virtual plib::owned_ptr<device_t> Create(netlist_state_t &anetlist, const pstring &name) = 0;
		virtual void macro_actions(netlist_state_t &anetlist, const pstring &name)
		{
			plib::unused_var(anetlist);
			plib::unused_var(name);
		}

		const pstring &name() const { return m_name; }
		const pstring &classname() const { return m_classname; }
		const pstring &param_desc() const { return m_def_param; }
		const pstring &sourcefile() const { return m_sourcefile; }

	private:
		pstring m_name;                             /* device name */
		pstring m_classname;                        /* device class name */
		pstring m_def_param;                        /* default parameter */
		pstring m_sourcefile;                       /* source file */
	};

	template <class C>
	class device_element_t : public element_t
	{
	public:
		device_element_t(const pstring &name, const pstring &classname,
				const pstring &def_param)
		: element_t(name, classname, def_param) { }
		device_element_t(const pstring &name, const pstring &classname,
				const pstring &def_param, const pstring &sourcefile)
		: element_t(name, classname, def_param, sourcefile) { }

		plib::owned_ptr<device_t> Create(netlist_state_t &anetlist, const pstring &name) override
		{
			return plib::owned_ptr<device_t>::Create<C>(anetlist, name);
		}
	};

	class list_t : public std::vector<std::unique_ptr<element_t>>
	{
	public:
		explicit list_t(setup_t &m_setup);
		~list_t();

		template<class device_class>
		void register_device(const pstring &name, const pstring &classname,
			const pstring &def_param)
		{
			register_device(std::unique_ptr<element_t>(plib::palloc<device_element_t<device_class>>(name, classname, def_param)));
		}

		void register_device(std::unique_ptr<element_t> &&factory);

		element_t * factory_by_name(const pstring &devname);

		template <class C>
		bool is_class(element_t *f)
		{
			return dynamic_cast<device_element_t<C> *>(f) != nullptr;
		}

	private:
		setup_t &m_setup;
	};

	// -----------------------------------------------------------------------------
	// factory_creator_ptr_t
	// -----------------------------------------------------------------------------

	using constructor_ptr_t = std::unique_ptr<element_t> (*)(const pstring &classname);

	template <typename T>
	std::unique_ptr<element_t> constructor_t(const pstring &name, const pstring &classname,
			const pstring &def_param)
	{
		return std::unique_ptr<element_t>(plib::palloc<device_element_t<T>>(name, classname, def_param));
	}

	// -----------------------------------------------------------------------------
	// factory_lib_entry_t: factory class to wrap macro based chips/elements
	// -----------------------------------------------------------------------------

	class library_element_t : public element_t
	{
	public:

		library_element_t(setup_t &setup, const pstring &name, const pstring &classname,
				const pstring &def_param, const pstring &source)
		: element_t(name, classname, def_param, source)
		{
			// FIXME: if it is not used, remove it
			plib::unused_var(setup);
		}

		plib::owned_ptr<device_t> Create(netlist_state_t &anetlist, const pstring &name) override;

		void macro_actions(netlist_state_t &anetlist, const pstring &name) override;

	private:
	};

	} // namespace factory

	namespace devices {
		void initialize_factory(factory::list_t &factory);
	} // namespace devices
} // namespace netlist

#endif /* NLFACTORY_H_ */
