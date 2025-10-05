#pragma once

#include <QObject>
#include <type_traits>

struct SignalBlockerGuard
{
	QObject* obj;

	SignalBlockerGuard(QObject* o)
		: obj{ o } { obj->blockSignals(true); }
	~SignalBlockerGuard() { obj->blockSignals(false); }
};

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
