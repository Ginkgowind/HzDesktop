#pragma once

#include <QObject>

/*
 * Qt源码里的p、q指针，在QObject的构造函数中需要是
 * QObjectPrivate的子类，而此类不是公开的，故此处自己实现一个
 */
class HzDesktopPrivate : public QObject
{
	Q_DISABLE_COPY(HzDesktopPrivate)
public:
	HzDesktopPrivate() = default;
	virtual ~HzDesktopPrivate() = 0;
	QObject* hzq_ptr;
};

#define HZQ_D(Class) Class##Private * const d = d_func()
#define HZQ_Q(Class) Class * const q = q_func()

#define Q_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() \
    { Q_CAST_IGNORE_ALIGN(return reinterpret_cast<Class##Private *>(qGetPtrHelper(d_ptr));) } \
    inline const Class##Private* d_func() const \
    { Q_CAST_IGNORE_ALIGN(return reinterpret_cast<const Class##Private *>(qGetPtrHelper(d_ptr));) } \
    friend class Class##Private;