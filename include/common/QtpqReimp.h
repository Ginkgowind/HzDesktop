#pragma once

#include <QObject>
#include <QWidget>

/*
 * Qt源码里的p、q指针，在QObject的构造函数中需要是
 * QObjectPrivate的子类，而此类不是公开的，故此处自己实现一个
 */
class HzDesktopPublic;

class HzDesktopPrivate : public QWidget/*QObject*/
{
    Q_DISABLE_COPY(HzDesktopPrivate)
public:
    HzDesktopPrivate() = default;
    virtual ~HzDesktopPrivate() = 0 {}
    HzDesktopPublic* hzq_ptr;
};

class HzDesktopPublic
{
public:
    HzDesktopPublic(HzDesktopPrivate* d_ptr)
        : hzd_ptr(d_ptr) {
        hzd_ptr->hzq_ptr = this;
    }
    virtual ~HzDesktopPublic() = 0 {}
    QScopedPointer <HzDesktopPrivate> hzd_ptr;
};

#define HZQ_D(Class) Class##Private * const d = d_func()
#define HZQ_Q(Class) Class * const q = q_func()

#define HZQ_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() \
    { Q_CAST_IGNORE_ALIGN(return reinterpret_cast<Class##Private *>(qGetPtrHelper(hzd_ptr));) } \
    inline const Class##Private* d_func() const \
    { Q_CAST_IGNORE_ALIGN(return reinterpret_cast<const Class##Private *>(qGetPtrHelper(hzd_ptr));) } \
    friend class Class##Private;

#define HZQ_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(hzq_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(hzq_ptr); } \
    friend class Class;