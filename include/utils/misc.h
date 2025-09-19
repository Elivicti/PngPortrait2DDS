#pragma once

#include <QString>
#include <type_traits>

template<typename T>
	requires requires (std::remove_cvref_t<T>* o) { o->blockSignals(true); }
struct SignalBlockerGuard
{
	using type = std::remove_cvref_t<T>;
	type* obj;

	SignalBlockerGuard(type* o)
		: obj{ o } { obj->blockSignals(true); }
	~SignalBlockerGuard() { obj->blockSignals(false); }
};

template<typename T>
SignalBlockerGuard(T* o) -> SignalBlockerGuard<T>;


template<typename Pred = std::less<int>>
struct CaseInsensitiveComparer
{
	bool operator()(const QString& lhs, const QString& rhs) const
	{
		return Pred{}(lhs.compare(rhs, Qt::CaseInsensitive), 0);
	}
};
template<typename T>
	requires (!std::is_reference_v<T>)
using optional_ref = std::optional<std::reference_wrapper<T>>;
