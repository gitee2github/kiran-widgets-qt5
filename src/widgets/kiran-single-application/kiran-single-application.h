// The MIT License (MIT)
//
// Copyright (c) Itay Grudev 2015 - 2018
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef SINGLE_APPLICATION_H
#define SINGLE_APPLICATION_H

#include <QtCore/QtGlobal>
#include <QtNetwork/QLocalSocket>
#include <kiran-application.h>

class KiranSingleApplicationPrivate;
/**
 * @brief The KiranSingleApplication class handles multiple instances of the same
 * Application
 * @see QCoreApplication
 */
class KiranSingleApplication : public KiranApplication
{
    Q_OBJECT

    using app_t = KiranApplication;

public:
    /**
     * @brief Mode of operation of KiranSingleApplication.
     * Whether the block should be user-wide or system-wide and whether the
     * primary instance should be notified when a secondary instance had been
     * started.
     * @note Operating system can restrict the shared memory blocks to the same
     * user, in which case the User/System modes will have no effect and the
     * block will be user wide.
     * @enum
     */
    enum Mode {
        User                    = 1 << 0,
        System                  = 1 << 1,
        SecondaryNotification   = 1 << 2,
        ExcludeAppVersion       = 1 << 3,
        ExcludeAppPath          = 1 << 4
    };
    Q_DECLARE_FLAGS(Options, Mode)

    /**
     * @brief Intitializes a KiranSingleApplication instance with argc command line
     * arguments in argv
     * @arg {int &} argc - Number of arguments in argv
     * @arg {const char *[]} argv - Supplied command line arguments
     * @arg {bool} allowSecondary - Whether to start the instance as secondary
     * if there is already a primary instance.
     * @arg {Mode} mode - Whether for the KiranSingleApplication block to be applied
     * User wide or System wide.
     * @arg {int} timeout - Timeout to wait in milliseconds.
     * @note argc and argv may be changed as Qt removes arguments that it
     * recognizes
     * @note Mode::SecondaryNotification only works if set on both the primary
     * instance and the secondary instance.
     * @note The timeout is just a hint for the maximum time of blocking
     * operations. It does not guarantee that the KiranSingleApplication
     * initialisation will be completed in given time, though is a good hint.
     * Usually 4*timeout would be the worst case (fail) scenario.
     * @see See the corresponding QAPPLICATION_CLASS constructor for reference
     */
    explicit KiranSingleApplication( int &argc, char *argv[], bool allowSecondary = false, Options options = Mode::User, int timeout = 1000 );
    ~KiranSingleApplication() override;

    /**
     * @brief Returns if the instance is the primary instance
     * @returns {bool}
     */
    bool isPrimary() const;

    /**
     * @brief Returns if the instance is a secondary instance
     * @returns {bool}
     */
    bool isSecondary() const;

    /**
     * @brief Returns a unique identifier for the current instance
     * @returns {qint32}
     */
    quint32 instanceId() const;

    /**
     * @brief Returns the process ID (PID) of the primary instance
     * @returns {qint64}
     */
    qint64 primaryPid() const;

    /**
     * @brief Returns the username of the user running the primary instance
     * @returns {QString}
     */
    QString primaryUser() const;

    /**
     * @brief Returns the username of the current user
     * @returns {QString}
     */
    QString currentUser() const;

    /**
     * @brief Sends a message to the primary instance. Returns true on success.
     * @param {int} timeout - Timeout for connecting
     * @returns {bool}
     * @note sendMessage() will return false if invoked from the primary
     * instance.
     */
    bool sendMessage( const QByteArray &message, int timeout = 100 );

    /**
     * @brief Get the set user data.
     * @returns {QStringList}
     */
    QStringList userData() const;

    /**
     * @brief 添加在构造函数中生成应用程序ID的用户数据
     * 应用程序ID是判断进程是否单例的关键,该方法需要在调用KiranSingleApplication构造之前使用
     */
    static void addApplicationIDUserData(const QString& userData);

Q_SIGNALS:
    void instanceStarted();
    void receivedMessage( quint32 instanceId, QByteArray message );

private:
    KiranSingleApplicationPrivate *d_ptr;
    Q_DECLARE_PRIVATE(KiranSingleApplication)
    void abortSafely();
    static QStringList m_userDataLists;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KiranSingleApplication::Options)

#endif // SINGLE_APPLICATION_H